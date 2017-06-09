/********************************************************************
	created:	2002/10/02
	created:	2:10:2002   15:48
	filename: 	$(Projects)\sources\grm executor\codestorage.h
	file path:	$(Projects)\sources\grm executor
	file base:	codestorage
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include <OutStreams.h>
#include <BaseUtils.h>
#include <AppUtils.h>
#include <ExitCodes.h>	//����� ��� ���� �������� ���������

#include "BaseStructs.h"
#include "DLLSupport.h"

using namespace std;
using namespace base_utils;
using namespace out_streams;
using namespace exit_codes;

class CCodeStorage
{
private:
	bool	m_bDebugInfo;
	bool	m_bStartPoint;
	DWORD	m_dwStartPoint;
	DWORD	m_dwCurrentPoint;
	string_smart m_strFileName;		//��� ������������ ������
	string_smart m_strDir;

	//������ ������������� �������
	typedef grm_structs::func_data func_data;
	typedef vector <func_data>	list_func; 
	list_func	m_Funcs;
	
	//������ ������
	typedef vector <SCommand> list_cmd;
	list_cmd	m_Commands;
	
	//������ ��������
	typedef	vector <variant> list_const;		
	list_const	m_Consts;
	
	//������ �����
	typedef vector <SType> list_type;
	list_type	m_Types; 

	SCommand * pCurCmd;
public:
	CCodeStorage():pCurCmd(NULL){}
	~CCodeStorage(){Clear();}

	//������� �����������
	void Clear();
	//��������� ���� � �����
	ERunnerExitCodes Load(cstr szFileName);
	//��� �������� ������������ ������
	cstr GetFileName()		const	{return m_strFileName.c_str();}
	//��� ��������
	cstr GetDir()			const	{return m_strDir.c_str();}
	//������� ����, ��� ����� ����� ��������
	bool IsLoaded()			const	{return !m_strFileName.isempty();}
	//���� �� ����� �����
	bool  IsStartPoint()	const	{return m_bStartPoint;}
	DWORD GetStartPoint()	const	{return m_dwStartPoint;}

	//����� ������� �������
	void SetIP(size_t dwAddr);
	void IncIP();
	DWORD GetIP()			const	{return m_dwCurrentPoint;}
	
	//����
	SType &	GetType(size_t dwIndex)		{_ASSERT(dwIndex<(size_t )m_Types.size());return m_Types[dwIndex];}
	variant & GetConst(size_t dwIndex)	{_ASSERT(dwIndex<(size_t )m_Consts.size());return m_Consts[dwIndex];}
	
	//��� ��������� ������� ���� DebugInfo. �� ����������� �������� dwSrcLine!
	bool	GetSrcData(size_t nIndex,cstr & szModule, size_t & nSrcLine);
	
	//��� ��������� ������� ���� DebugInfo ��� ������ �����. �� �������������, ���
	//�������� DebugInfo ����� ������ ������ �� ���� �������
	bool	FindNearestSrcData(size_t nIndex, cstr & szModule, size_t & nSrcLine);
	
	//������ � ������� ������������� �������
	size_t GetImportFuncCount() const					{return m_Funcs.size();}
	const func_data & GetImportFunc(size_t nIndex)const	{return m_Funcs[nIndex];}

	SCommand & CurCmd()			const	{return *pCurCmd;}
	
	//�������� ����������
	size_t GetSize()			const	{return m_Commands.size();}
	SCommand & operator[](size_t nIndex){return  m_Commands[nIndex];}
};