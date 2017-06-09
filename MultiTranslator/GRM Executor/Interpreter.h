/********************************************************************
	created:	2002/10/02
	created:	2:10:2002   18:21
	filename: 	$(Projects)\sources\grm executor\code.h
	file path:	$(Projects)\sources\grm executor
	file base:	code
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	������������� ����
*********************************************************************/

#if !defined(AFX_CODE_H__9ACBD54C_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_)
#define AFX_CODE_H__9ACBD54C_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_

#pragma once

#include <OutStreams.h>
#include <BaseUtils.h>
#include <ExitCodes.h>	//����� ��� ���� �������� ���������
#include <GrmExecExports.h>
#include <AppMessages.h>

#include "BaseStructs.h"
#include "CodeStorage.h"
#include "StdLibs.h"
#include "EngineUtils.h"
#include "DLLSupport.h"

using namespace base_utils;
using namespace out_streams;
using namespace exit_codes;
using namespace engine_utils;
using namespace std_libs;

class CInterpreter  
{
private:
	raw_buffer<string_cstr>	m_argv;

	static void SubRoutineHandler(DWORD SubrAddr);		
	static	CInterpreter*	pCurCode;
	
	bool RunFromTo(DWORD StartAddr, ECodeOperation EndOperation, DWORD & CommandPointer);
	bool ProcessCommand(SCommand & Cmd, size_t & nNextIP);
	bool isSuccessFullyLoaded;

	bool							m_bParsing;
	
	//�����, ���������� � ������������ � ������������ ����������
	//��� ������� ������ ����������� (��������� NEWA, NEW...)
	class CDeclaredVariables{
		// � ��������, ������ ������� SVar � �������� �� ����������, �.�.
		// ��� ����� ����� ����������� �����������, � ��� ������� - �����������
		// ������� � ������ ������ �� ������ �� ������������ �������
		class  CSmartVarPtr{	
			SVar * m_pVar;
		public:
			CSmartVarPtr(SVar &src){
				m_pVar=new SVar(src);
			}
			CSmartVarPtr(const CSmartVarPtr & src):m_pVar(NULL){
				this->operator = (src);
			}
			~CSmartVarPtr(){
				if (m_pVar){
					_ASSERT(_CrtIsValidHeapPointer(m_pVar));
					delete m_pVar;
				}
			}
			operator SVar & () {
				_ASSERT(m_pVar);
				return *m_pVar;
			}
			CSmartVarPtr & operator = (const CSmartVarPtr & src){
				_ASSERT(m_pVar==NULL);
				_ASSERT(src.m_pVar);
				m_pVar=src.m_pVar;
				const_cast <CSmartVarPtr&> (src).m_pVar=NULL;
				return *this;
			}
		};
		//----------------------------------------------------------------
		typedef vector<CSmartVarPtr>vector_vars;
		typedef vector<vector_vars> vector_frames;
		typedef vector<size_t>		vector_frame_ids;

		vector_frames		m_VarLevels;		
		vector_frame_ids	m_FramesStack;

		size_t m_dwCurFrame;	//����������� "�������" �������, �� �������� 
								//������������� ������ ��� ���������� (����� ��������)
		//----------------------------------------------------------------
	public:
		CDeclaredVariables(){
		}
		
		//������ � �������� (��� �������� � ������ �������)
		void PushFrame(){
			m_FramesStack.push_back(m_dwCurFrame); //������ �������
			m_dwCurFrame=TopLevel(); 
			m_FramesStack.push_back(m_dwCurFrame);//������� �������
		}

		//������ � �������� (��� �������� � ������ �������)
		void PopFrame(){
			_ASSERT(m_FramesStack.size()>0);
			m_dwCurFrame = m_FramesStack.back();m_FramesStack.pop_back(); //������� �������
			while (m_dwCurFrame!=TopLevel()) // ���������� ������ ������ ���������
				m_VarLevels.pop_back();
			_ASSERTE(m_VarLevels.size()>0);
			m_dwCurFrame = m_FramesStack.back();m_FramesStack.pop_back();	//������ �������
		}		
		//���������� ������ ������ ��� ���������� ���������� (���� � �������}
		void PushLevel(){
			m_VarLevels.push_back(vector_vars());
		}

		//����� �� ��������� ������� ���������
		void PopLevel()	{
			_ASSERT(m_VarLevels.size()>0); m_VarLevels.pop_back();
		}

		//������� ������� �������
		size_t	TopLevel()	{return m_VarLevels.size()-1;}
		
		//�� ��������� ������ ��������� �������� �� ���������� ������� ����������
		SVar & Get(DWORD dwLevelIndex, DWORD dwVarIndex){
			DWORD dwLvl = (dwLevelIndex==0) ? 0 : m_dwCurFrame+dwLevelIndex;
			_ASSERT(dwLvl<m_VarLevels.size());
			_ASSERT(dwVarIndex<m_VarLevels[dwLvl].size());
			SVar & res = m_VarLevels[dwLvl][dwVarIndex];
			_ASSERT(res.IsLValue());
			return res;
		}
		//� ��������� ������� ��������� ��������� �������� ����� ����������
		//���������� �� ������ � �������
		size_t Add(DWORD dwLevelIndex, SVar & var){
			_ASSERTE(var.GetLValueSrc()==NULL);
			_ASSERTE(var.GetAlias()==NULL);
			var.SetLValueSign(true);	//��� ���������� ������� ������ ������� ���� �����������
			DWORD dwLvl = (dwLevelIndex==0) ? 0 : m_dwCurFrame+dwLevelIndex;
			_ASSERT(dwLvl==m_VarLevels.size()-1);
			m_VarLevels[dwLvl].push_back(var);
			return m_VarLevels[dwLvl].size()-1;
		}
		size_t	Size()						{
			return m_VarLevels.size();}
		
		size_t	TopFrameSize()				{
			return m_VarLevels.size()-m_dwCurFrame;}
		
		size_t	LevelSize(size_t nIndex)	{
			DWORD dwLvl = (nIndex==0) ? 0 : m_dwCurFrame+nIndex;
			_ASSERT(dwLvl<m_VarLevels.size());
			return m_VarLevels[dwLvl].size();}
		
		void Clear()						{m_VarLevels.clear();}
		bool IsEmpty()						{return m_VarLevels.empty();}
	};
	
	//�����, ���������� � ���������� � �����
	//�������� � ���� ���� ������ (��� ������ ����� ������� ��������� ���� 
	//�� NEW_LEVEL, EXIT_LEVEL
	class CLocalStack{
		typedef vector<SVar>		vector_vars;
		typedef vector<vector_vars> vector_frames;
		typedef vector<size_t>		vector_frame_ids;
		vector_frames		m_VarLevels;		
		vector_frame_ids	m_FramesStack;	//�������� ������, ������ �����
	public:
		CLocalStack(){
		}
		
		//������ � �������� (��� �������� � ������ �������)
		void PushFrame(){
			m_FramesStack.push_back(m_VarLevels.size());
		}

		//������ � �������� (��� �������� � ������ �������)
		void PopFrame(){
			_ASSERT(m_FramesStack.size()>0);
			size_t nLen = m_FramesStack.back();m_FramesStack.pop_back();
			while (nLen!=m_VarLevels.size()) // ���������� ������ ������ ���������
				PopLevel();					 //��� ��������� � ��� ������, ����� ���� return �� ������� ������� ������ ��������� �������� ���������
		}		
		//���������� ������ ������ ��� ���������� ���������� (���� � �������}
		void PushLevel(){m_VarLevels.push_back(vector_vars());}

		//����� �� ��������� ������� ���������
		void PopLevel()	{m_VarLevels.pop_back();}

		//������� ������� �������
		size_t	TopLevel()	{return m_VarLevels.size()-1;}

		//�� ��������� ������ ��������� ����� ��������� �������
		void Get(SVar & var){
			var = m_VarLevels.back().back();
		}
		//�� ��������� ������ ��������� ����� ��������� �������
		SVar & Get(){
			return m_VarLevels.back().back();
		}
		//�� ��������� ������ ��������� ����� ��������� �������
		SVar Pop(){
			SVar var; Pop(var);
			return var;
		}
		//�� ��������� ������ ��������� ����� ��������� �������
		void Pop(SVar & var){
			_ASSERT(m_VarLevels.size()>0);
			_ASSERT(m_VarLevels.back().size()>0);
			var = m_VarLevels.back().back(); m_VarLevels.back().pop_back();
		}
		//�������� ������� ������� �������� ������
		void Delete(){
			m_VarLevels.back().pop_back();
		}
		//�������� � ������� ������� ����� �������
		void Push(const SVar & var){
			m_VarLevels.back().push_back(var);
		}
		//������� ������� ������� �����
		void ClearTopLevel()	{
			m_VarLevels.back().clear();
		}

		void Push(const long nVal)		{Push(SVar(nVal));}
		void Push(const DWORD nVal)		{Push(SVar((long)nVal));}		
		void Push(const size_t nVal)	{Push(SVar((long)nVal));}		
		void Push(const cstr szVal)		{Push(SVar(szVal));}
		void Push(const bool bVal)		{Push(SVar(bVal));}
		void Push(const variant & vVal)	{Push(SVar(vVal));}

		size_t	Size()	{return m_VarLevels.size();}
		void Clear()	{m_VarLevels.clear();}
		bool IsEmpty()	{return m_VarLevels.empty();}
	};
	
	//�������� ���� ���������. �������� � ���������� Push, Pop
	class CDataStack{
		typedef vector<SVar>		vector_vars;
		vector_vars	m_Stack;
	public:
		void Push(SVar & var)	{m_Stack.push_back(var);}
		void Pop(SVar & var)	{_ASSERT(m_Stack.size()); var=m_Stack.back();m_Stack.pop_back();}
		SVar Pop()				{SVar var; return Pop(var), var;}
		SVar & Get()			{return m_Stack.back();}
		bool IsEmpty()			{return m_Stack.empty();}
		void Clear()			{m_Stack.clear();}
	};
	
	//���� �������
	class CAddressStack{
	private:
		struct addr_block{
			size_t nIP;
			cstr szFunctionName;
			addr_block(size_t addr, cstr szFN):nIP(addr),szFunctionName(szFN){};
		};
		typedef vector<addr_block>	vector_addr;
		vector_addr m_Stack;
	public:
		void Push(size_t addr, cstr szFN)	{m_Stack.push_back(addr_block(addr,szFN));}
		void Pop(size_t & addr)	{_ASSERT(!IsEmpty()); addr=m_Stack.back().nIP;m_Stack.pop_back();}
		size_t Pop()			{_ASSERT(!IsEmpty()); size_t addr; return Pop(addr), addr;}
		size_t & Get()			{return m_Stack.back().nIP;}
		size_t & GetAt(size_t n){_ASSERT(n<Size());return m_Stack[n].nIP;}		
		cstr GetName()			{return m_Stack.back().szFunctionName;}				
		cstr GetNameAt(size_t n){_ASSERT(n<Size());return m_Stack[n].szFunctionName;}		
		size_t	Size()			{return m_Stack.size();}				
		bool IsEmpty()			{return m_Stack.empty();}
		void Clear()			{m_Stack.clear();}
	};
	//���������� object_info
	class CObjectInfo: public grmexec_exports::object_info{
	public:
		#ifdef _DEBUG
		string_local_alloc		m_strName;
		string_local_alloc		m_strContext;
		#else
		string_smart			m_strName;
		string_smart			m_strContext;
		#endif
		kind					m_nKind;
		variant					m_vValue;
		vector<CObjectInfo>		m_Items;
		override ~CObjectInfo(){
			m_strName.erase();		// ���� �� �������
			m_strContext.erase();	// ������� �������� � ������������ ������ ������ 
			m_Items.clear();		// � �������� ���������� ��� ������ ����� ������� ��� Dll
			m_vValue.clear();
		};
		override cstr GetName()		{return m_strName.c_str();}
		override cstr GetContext()	{return m_strContext.c_str();}
		override kind GetKind()		{return m_nKind;}		
		override const variant & GetValue()	{return m_vValue;} //���������� ����� ������ �� ������. ��� ������� � ��������� ��������� � ������������ ������ � ������ ������� (DLL � EXE)
		override size_t	ItemCount()	{return m_Items.size();}
		override CObjectInfo& GetItem(size_t & nIndex) {return m_Items[nIndex];}
	};
	
	struct imp_func_data: public grm_structs::func_data{
		dll_function<FARPROC> m_Func;
		imp_func_data(const grm_structs::func_data & src, const string_list & strAuxFolders= string_list()):grm_structs::func_data(src){
			m_Func.attach(find_full_filepath(m_szLibName,strAuxFolders).c_str(),m_szFuncName);
		}
	};

	grmexec_exports::step_functor m_fStepCallBack;	//�������� �������, ���������� �� ������ ����
	
	CAddressStack			m_AddrStack;			//���� �������
	CDataStack				m_DataStack;			//����� ����
	CDeclaredVariables		m_DeclaredVariables;	//��� ���������� ���������� �� ������ ������� ���������
	CLocalStack				m_LocalStack;			//��������� ���� � �������� ������� ���������
	CCodeStorage			m_Code;
	
	std_libs::text_container m_Container;			//��������� ��� ������ � ����
	library_manager			m_LibraryManager;


	typedef vector<imp_func_data>	vector_imp_funcs;
	vector_imp_funcs	m_ImportFuncs;				//������ ������������� �������

	//������� ��� ���������� �������, � ������� �������� ���������� ����� ����������
	void	ClearInternalStructures(); 
	static  FillObjectInfo(CObjectInfo* pInfo, SVar* pVar);
public:
	CInterpreter();
	virtual ~CInterpreter();
	void	SetStepCallback(grmexec_exports::step_functor fStepCallback);
	bool	IsReady()	{return m_Code.IsLoaded();}
	void	Clear();
	//��������
	ERunnerExitCodes Load(cstr szFileName);
	//������
	ERunnerExitCodes Run(size_t argc, cstr argv[],int & iExitCode);
	//������� ��� ������
	cstr LoadedModuleName()	const {_ASSERT(m_Code.IsLoaded());return m_Code.GetFileName();}
	//��������� ��������� ������� � �������� ���� �� ����������� ��������
	bool CheckBreakPointOnUsed(cstr szModulePath, size_t dwLine);
	//��������� ���������� � ������ ���� ��������� �������� ���������� ����. ����� ������, ������ ������� � ������� ���������� '\n'
	string_smart GetObjectNames(grmexec_exports::get_oi_filter filter);
	//��� ���������� ������� (����������) ���������� ��� ���������� � ���
	grmexec_exports::object_info * GetObjectInfo(cstr szObjectName,grmexec_exports::get_oi_result& result);
};

#endif // !defined(AFX_CODE_H__9ACBD54C_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_)
