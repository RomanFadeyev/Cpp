/********************************************************************
	created:	2003/05/31
	created:	31:5:2003   23:26
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lexanalyzer.h
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lexanalyzer
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_LEXANALYZER_H__2CF293EA_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_LEXANALYZER_H__2CF293EA_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include <vector>
#include <list>
#include <BaseUtils.h>

#include <BaseStructs.h>

using namespace base_utils;

class CLexHub;
class CLexAutomat;
class CLexAnalyzer;

//------------------------------------------------------------------------
// class: LexVariant
//------------------------------------------------------------------------
class CLexVariant
{
protected:
	//����� ��� �������� ���������� �� ������, ������� ������
	class err_w_container{
	protected:
		virtual void raise(const structs::text_location & pos) const =0;
		virtual err_w_container * copy() const =0;
		friend class CLexVariant;
	};

	typedef std::list<const CLexAutomat*>	list_automats;
	typedef ret_t (*lex_func1) ();
	typedef ret_t (*lex_func2) (const ILexAnalyzeContext &);
public:
	typedef	structs::litera::lex_tag lex_tag;
	typedef	structs::litera litera;

	class lex_func: public ILexAnalyzeContext
	{
		mutable structs::litera m_Etalon;
		int	 m_nType;
		union{	
			lex_func1 m_pFunc1;
			lex_func2 m_pFunc2;
		};
	public:
		lex_func(lex_func1 f): m_nType(1),m_pFunc1(f){}
		lex_func(lex_func2 f): m_nType(2), m_pFunc2(f){}
		
		bool empty() const {
			return m_pFunc1!=NULL;
		}
		ret_t invoke(const structs::litera & etalon) const{
			m_Etalon=etalon;
			switch(m_nType) {
				case 1:	return m_pFunc1();
				case 2:	return m_pFunc2(*this);
				default:
					_ASSERT(false);
			};
			return false;
		}
		//ILexAnalyzeContext
		override ILexSource &	GetLexSource()				const;
		override ILexList&		GetLexList()				const;
		override ILexTable&	GetLexTables()				const;
		override const structs::litera & GetEtalonLitera()	const {return m_Etalon;};

	};
	typedef std::list<lex_func>	list_func;
	
	//����� ��� �������� ���������� �� ������, ��������� �������������
	//��� ������� ����. � ����������� ������ ������ ������ ���� 
	//����������� T(const erros::structs::text_pos & pos)
	template <class T>
	class err_container:public err_w_container
	{
	protected:
		void raise(const structs::text_location & pos) const{
			T tmp(pos);
		}
		virtual err_w_container * copy() const {
			return new err_container<T>();
		}
	public:
	};

	//����������� ����� - ��� ������
	class err_no_container:public err_w_container
	{
	protected:
		void raise(const structs::text_location & pos) const{
			_ASSERT(false);
		}
		virtual err_w_container * copy() const {
			return NULL;
		}
	};

protected:
	structs::litera	m_LexicChar;
	list_func		m_Functions;
	list_automats		m_NextLAU;
	err_w_container*m_pEW;
	int				m_nNextHub;
public:
	CLexVariant (char_t ch,  int Next_Hub, const err_w_container & EW, lex_func* F1=NULL, lex_func* F2=NULL, lex_func* F3=NULL);
	CLexVariant (lex_tag Tag,int Next_Hub, const err_w_container & EW, lex_func* F1=NULL, lex_func* F2=NULL, lex_func* F3=NULL);
	CLexVariant	(const CLexAutomat* pNextLAU1, const CLexAutomat* pNextLAU2=NULL, const CLexAutomat* pNextLAU3=NULL);
	CLexVariant (const CLexVariant & src);
	~CLexVariant ();

	//���������� ���������� ������ �� ������� "������������" � ����� ��������
	bool IsProper(const structs::litera & l) const{ 
		return (m_LexicChar.GetName()==0) ? (l.Tag & m_LexicChar.Tag)!=0 : l.Name==m_LexicChar.Name;
	}
	
	//�������� �� ��� ������� ������� ������� �� ������ ����
	bool IsLinkToRule() const{
		return m_NextLAU.size()>0;
	}
	
	//���� ������ �� ��������� ���� � ������ ��������� ����������� ������� � ������ ����
	int  NextHub() const{
		return m_nNextHub;
	}

	//���� ��� ������� �������� ������������ �� ��������� ����, �� �� �������� 
	//�������� ���������� ����
	bool PassToHub(const structs::litera & l) const;

	//�������� ���� ������������������ ������ �������
	void InvokeFunctions() const;
	//�������� ��������� �� ������� ��� ���������������, ���� ������� �������
	void RaiseErrWarns() const;

	CLexVariant & operator=(const CLexVariant& src);	
};

//------------------------------------------------------------------------
// class: LexHub
//------------------------------------------------------------------------
class CLexHub  
{
protected:
	typedef std::list<CLexVariant> list_variant; 
	list_variant m_lstVariants;	
public:
	CLexHub	();
	CLexHub	(const CLexHub &src);
	
	CLexHub (const CLexVariant & src);
	CLexHub (const CLexVariant & src1,const CLexVariant & src2);
	CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3);
	CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3, const CLexVariant & src4);
	CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3, const CLexVariant & src4,const CLexVariant & src5);
	CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3, const CLexVariant & src4,const CLexVariant & src5, const CLexVariant & src6);

	virtual ~CLexHub();

	void AddVariant	 (const CLexVariant & src);
	int  Search		 (const structs::litera &) const; // return index of next hub or EXIT_TRUE or EXIT_FALSE;
	bool TryTranslate(const structs::litera &) const;
	
	CLexHub & operator = (const CLexHub& A);
};

//------------------------------------------------------------------------
// class: LexAutomat
//------------------------------------------------------------------------
class CLexAutomat
{
protected:
	typedef std::vector<CLexHub> list_hubs; 
	list_hubs	m_lstHubs;	
	string_std	m_strName;
public:
	CLexAutomat(cstr szName): m_strName(szName){
	}
 
	//������ ����� � ����������� ���� � ��������� �� � ������
	CLexHub &  AddHub	(const CLexHub & src){
		m_lstHubs.push_back(src);
		return (*m_lstHubs.rbegin());
	}
	//��������� ������ ����
	CLexHub &  AddHub	(){
		m_lstHubs.push_back(CLexHub());
		return (*m_lstHubs.rbegin());
	}
	size_t  HubCount() const {
		return m_lstHubs.size();
	}
	const CLexHub & GetHub(size_t nIndex) const {
		_ASSERT(nIndex<m_lstHubs.size());
		return m_lstHubs[nIndex];
	}
	bool TryTranslate (const ILexSource & LexSource){
		return GetHub(0).TryTranslate(LexSource.GetCurSymbol());
	}
	bool Translate(const ILexSource & LexSource) const;
};

//------------------------------------------------------------------------
// class: CLexAnalyzer
//------------------------------------------------------------------------
class CLexAnalyzer  
{
protected:
	static ILexSource * m_pCurLexSource;
	static ILexList	*	m_pCurLexList;
	static ILexTable*	m_pCurLexTables;

	typedef vector<CLexAutomat> list_automats;
	list_automats m_lstAutomats;
public:
	static ILexSource & CurLexSource()	{_ASSERT(m_pCurLexSource); return *m_pCurLexSource;}
	static ILexList & CurLexList()		{_ASSERT(m_pCurLexList); return *m_pCurLexList;}
	static ILexTable & CurLexTables()	{_ASSERT(m_pCurLexTables); return *m_pCurLexTables;}

	bool Analyze(ILexSource & LexSource, ILexList & LexList, ILexTable & LexTables);
	//�������� ����� ����������� �������� � ������ � ���������� �� ��� ������
	CLexAutomat& AddAutomat(const CLexAutomat & LA);
	//������� ������ �������
	CLexAutomat& AddAutomat(cstr szName);
	//���������� ������������� ������ �� ��������� �������
	CLexAutomat& GetAutomat(size_t nIndex) {_ASSERT(nIndex<m_lstAutomats.size()); return m_lstAutomats[nIndex];}

	void Clear(){
		m_lstAutomats.clear();
	}
	CLexAnalyzer();
	virtual ~CLexAnalyzer();
};

#endif // !defined(AFX_LEXANALYZER_H__2CF293EA_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
