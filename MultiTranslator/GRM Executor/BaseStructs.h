// BaseStructs.h: interface for the CBaseStructs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASESTRUCTS_H__9ACBD54B_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_)
#define AFX_BASESTRUCTS_H__9ACBD54B_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OutStreams.h>
#include <GrmStructs.h>

#include "..\GRM Compiler\Define.h"
#include "..\GRM Compiler\DefineCommands.h" // enum ECodeOperation { NOP,PUSH,....

using namespace grm_structs;

enum EExtEventClass { EVENT_RUN_PARSE,EVENT_END_PARSE,EVENT_STOP_PARSE,HALT_PROCESS,HALT_PROCESS_BY_ERROR};

// ���� enum ������ ��������� � enum ELexemeClass � Lexicon'e (BaseStructs.h)
typedef ELexemeCLass ECommandClass;
					
using namespace out_streams;

struct SCommand
{
	struct SCommandProp
	{
		SCommandProp()
		{
			Index=0;
			Class=NO_CLASS;
		}
		SCommandProp & operator = (const SCommandProp & src)
		{
			this->Class=src.Class;
			this->Index=src.Index;
			this->m_strSrcText=src.m_strSrcText.c_str();
			return *this;
		}
		ECommandClass	Class;
		DWORD			Index;
		string_smart	m_strSrcText;
	};

	SCommand ()
	{
		dwSrcLine=-1;
		Operation=NOP;
	}
	SCommand(SCommandProp Op1,SCommandProp Op2,SCommandProp Op3, ECodeOperation Operation)
	{
		this->dwSrcLine=-1;
	
		this->Op1=Op1;
		this->Op2=Op2;
		this->Op3=Op3;
		this->Operation=Operation;
	}
	SCommand(SCommandProp Op1,SCommandProp Op2,SCommandProp Op3, SCommandProp Op4,ECodeOperation Operation)
	{
		this->dwSrcLine=-1;
	
		this->Op1=Op1;
		this->Op2=Op2;
		this->Op3=Op3;
		this->Op4=Op4;
		this->Operation=Operation;
	}
	SCommand & operator = (const SCommand & src)
	{
		this->Op1=src.Op1;
		this->Op2=src.Op2;
		this->Op3=src.Op3;
		this->Op4=src.Op4;
		this->Operation=src.Operation;
		this->dwSrcLine=src.dwSrcLine;
		this->m_strModuleName=src.m_strModuleName;
		return * this;
	}

	SCommandProp	Op1;
	SCommandProp	Op2;
	SCommandProp	Op3;
	SCommandProp	Op4;
	ECodeOperation	Operation;
	
	DWORD			dwSrcLine;		//==-1 - �� ����������������
	string_std		m_strModuleName;	//���� �� ������������
};
//------------------------------------------------------------------------
class SType: public grm_structs::type
{
public:
	struct SMember
	{
		DWORD nDim;	
		DWORD nTypeIndex;	
		SType * m_pSrc;
	
		SMember ():	nDim(0),nTypeIndex(-1),m_pSrc(NULL){
		}
	    SMember (const SMember & src){
        *this=src;
		}
		SMember & operator = (const SMember & src){
			nDim=src.nDim;
			nTypeIndex=src.nTypeIndex;
			m_pSrc=src.m_pSrc;
			return *this;
		}
	};

	SType()	{}
	~SType(){Members.clear(); }
	
	vector <SMember> Members;
public:
	override size_t GetFieldCount() const{
		return Members.size();
	}
	SType & GetMemberType(size_t nMemberIdx){
		_ASSERT(nMemberIdx<GetFieldCount());
		return *Members[nMemberIdx].m_pSrc;
	}
	DWORD GetMemberDim(size_t nMemberIdx){
		_ASSERT(nMemberIdx<GetFieldCount());
		return Members[nMemberIdx].nDim;
	}
	
	void SetTypeClass(type_class aClass){
		m_nClass = aClass;
	}
	void SetType(object::type aType){
		m_nType = aType;
	}
	void SetMemSize(size_t nMemSize){
		m_nMemSize= nMemSize;
	}
};
//------------------------------------------------------------------------
//����� ����������� ������� �������� ������� � ��������� 
//������� ������������, ����� �� �������� ������ ��� ������
//�����, ���������� ���, ��� ��� ��������� � ����� ���������
//�� ������ ����� �� m_pSrc � ����������, ����������� � ������� ����������
//���� ��������.
class SVar: public base_utils::variant
{
private:
	cstr	pStrSrc;
	bool	m_bLValueSign;	//��� �������: ����������, ���������� � �.�.
	SVar  * m_pAlias;		//��� �������� �� ������
	SVar  * m_pSrc;			//��������� �� ������� ������������ ������������������ ������, ��������� ��� ���������� ������������	
	void SetLValueSrc(SVar  & src);
public:
	SType * m_pType;		//��� ����������
	vector <SVar *> Neighbours;	// c����� � �������; ����������� ������ � ������ �������� �� �������
	vector <SVar *> Members;		// ����� ���������


	SVar():m_pAlias(NULL),m_pSrc(NULL),m_pType(NULL),m_bLValueSign(false),pStrSrc(NULL){
	}

	//for cont INT
	explicit SVar (long lVal):m_pAlias(NULL),m_pSrc(NULL),m_pType(NULL),m_bLValueSign(false),pStrSrc(NULL),
					 variant(lVal){
	}
	explicit SVar (bool bVal):m_pAlias(NULL),m_pSrc(NULL),m_pType(NULL),m_bLValueSign(false),pStrSrc(NULL),
					 variant(bVal){
	}

	explicit SVar (cstr szVal):m_pAlias(NULL),m_pSrc(NULL),m_pType(NULL),m_bLValueSign(false),pStrSrc(NULL),
					 variant(szVal){
	}

	// for constants 
	explicit SVar (const variant & ConstData):m_pAlias(NULL),m_pSrc(NULL),m_pType(NULL),m_bLValueSign(false),pStrSrc(NULL),
									  variant(ConstData){
	}
	
	// for variables 
	explicit SVar (SType & Type, LPCTSTR pStr=NULL):m_pAlias(NULL),m_pSrc(NULL),m_pType(&Type),m_bLValueSign(false),pStrSrc(pStr)
	{
		if (m_pType->IsSimple())
		{
			variant::set_type(object::object2variant(m_pType->GetType()));
		}
		else
		{
			for (size_t i=0;i<m_pType->GetFieldCount();i++)
			{
				this->Members.push_back(new SVar(m_pType->GetMemberType(i)));
				this->Members.back()->CreateArray(m_pType->GetMemberDim(i));
			}
		}
	}

	SVar(const SVar & src): m_pAlias(NULL){
		*this=src;
	}

	~SVar(){
		if (m_pAlias)
			this->m_pAlias->operator =(*this);
		Clear();
	}
	

	//���������� true, ���� ������ �������� ���� (�� ���������)
	bool IsSimple(){
		return (m_pType==NULL) || (m_pType->IsSimple());
	}
	//������ � Alias
	void SetAlias(SVar * pvar){
		_ASSERT(m_pAlias==NULL);
		m_pAlias=pvar;
	}
	SVar * GetAlias(){
		return m_pAlias;
	}
	//������ ��������� LValue. ������������ ������ ��� �������
	void SetLValueSign(bool bSet){
		m_bLValueSign=bSet;
		for (size_t i=0;i<Members.size();i++)
			Members[i]->SetLValueSign(bSet);
		for (size_t i=1;i<Neighbours.size();i++)	// c 1!
			Neighbours[i]->SetLValueSign(bSet);
	}
	bool IsLValue(){
		return m_bLValueSign;
	}
	SVar  * GetLValueSrc(){
		_ASSERT((m_pSrc==NULL) || (_CrtIsValidHeapPointer(m_pSrc)));
		return m_pSrc;
	}
	//����������� ������ src, �� ��� ���� �������� �� � ������ ����
	SVar & Assign(const SVar & src);
	//����������� ����� ��������, �� ��� ���� ������� ��� ������ ���� vtINT
	//� ������ ������ ���� IsSimple
	SVar & Assign(const int iVal);
	//����������� ������, �� ��� ���� ������� ��� ������ ���� vtSTRING
	//� ������ ������ ���� IsSimple
	SVar & Assign(cstr szVal);
	
	//�������� ��� ������ � ���������� m_pSrc=src
	SVar & CopyRef(SVar & src);
	
	//������� �������� ��� ���� �� src, �� ����������� Alias
	//Alias ��� ����������� ������ ��������������� � NULL
	SVar & operator = (const SVar & src);

	void CreateArray(long HowMany);
	void DestroyArray();
	size_t GetArrayDim()	const {return Neighbours.size();}

	void Clear();
	void Print(out_stream & Msg);
	void SPrint(string_smart & str);
	cstr GetDebugInfo()		const {return (pStrSrc) ?pStrSrc :"";}
};
#pragma warning (default:4018)
//------------------------------------------------------------------------

#endif // !defined(AFX_BASESTRUCTS_H__9ACBD54B_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_)
