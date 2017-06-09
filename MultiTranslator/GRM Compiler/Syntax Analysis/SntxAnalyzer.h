/********************************************************************
	created:	2003/06/01
	created:	1:6:2003   10:33
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\sntxanalyzer.h
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	sntxanalyzer
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_SNTXANALYZER_H__2CF293E9_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_SNTXANALYZER_H__2CF293E9_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once
#include <vector>
#include <tree.h>

#include "BaseStructs.h"

#define GE_HASH_TABLE_SIZE		ALPHABET_SIZE

//Ведение статистики "верных" ветвей в каждом узле
//Позволяет подсчитать, какие ветви используются чаще и "удачно"
//#define _STATISTIC_CHILDREN

#define	_NO_BACK_OPTIMIZATION
//#define	_SEARCH_HASH_OPTIMIZATION_1
#define	_SEARCH_HASH_OPTIMIZATION_2
#define	_SEARCH_RECURSE_OPTIMIZATION

//------------------------------------------------------------------------
// CSntxVariant
//------------------------------------------------------------------------
class CSntxVariant:public structs::sntx_variant
{
	typedef structs::grm_term	ge;
	typedef structs::sntx_error	ew;

	void AddEnd(pf_sntx_func pFunc);
public:
	CSntxVariant(){
	};

	#define GRM_PRMS_1 const ge& GE1
	#define GRM_PRMS_2 GRM_PRMS_1,const ge& GE2
	#define GRM_PRMS_3 GRM_PRMS_2,const ge& GE3
	#define GRM_PRMS_4 GRM_PRMS_3,const ge& GE4
	#define GRM_PRMS_5 GRM_PRMS_4,const ge& GE5
	#define GRM_PRMS_6 GRM_PRMS_5,const ge& GE6
	#define GRM_PRMS_7 GRM_PRMS_6,const ge& GE7
	#define GRM_PRMS_8 GRM_PRMS_7,const ge& GE8
	#define GRM_PRMS_9 GRM_PRMS_8,const ge& GE9
	#define GRM_PRMS_10 GRM_PRMS_9,const ge& GE10
	#define GRM_PRMS_11 GRM_PRMS_10,const ge& GE11
	#define GRM_PRMS_12 GRM_PRMS_11,const ge& GE12
	#define GRM_PRMS_13 GRM_PRMS_12,const ge& GE13
	#define GRM_PRMS_14 GRM_PRMS_13,const ge& GE14
	#define GRM_PRMS_15 GRM_PRMS_14,const ge& GE15
	#define GRM_PRMS_16 GRM_PRMS_15,const ge& GE16
	#define GRM_PRMS_17 GRM_PRMS_16,const ge& GE17

	CSntxVariant (const CSntxVariant &);
	CSntxVariant (GRM_PRMS_1,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_2,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_3,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_4,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_5,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_6,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_7,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_8,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_9,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_10,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_11,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_12,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_13,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_14,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_15,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_16,const ew &, pf_sntx_func pF=NULL);
	CSntxVariant (GRM_PRMS_17,const ew &, pf_sntx_func pF=NULL);
};


//------------------------------------------------------------------------
// CSntxRule
//------------------------------------------------------------------------
class CSntxRule: public structs::sntx_rule
{
public:
	typedef std::tree<structs::grm_term>	tree_gramms;
	typedef tree_gramms::row_iterator		tree_gramms_it;
	typedef tree_gramms::tree_iterator		tree_gramms_tit;
private:
	void AnalyzeError(const structs::grm_term & CurGE,structs::sntx_analyze_context & Context);	//Попытка определить ошибку на текущем этапе
protected:
	//Структура для сохранения состояния списков в процессе разбора текста
	struct chain_info{
		long m_nChainPos;
		long m_nLexPos;
		chain_info (long nChainState, long nLexState):m_nChainPos(nChainState), m_nLexPos(nLexState){
		}
	};

	//Структура для оптимизации поиска на основе хеширования, вариант 1
	struct hash_info1{
		bool	m_bPassed;
		bool	m_bInited;

		bool	m_Table[GE_HASH_TABLE_SIZE];
		bool	m_bID; 
		bool	m_bConst;
		bool	m_bWord; 
		bool	m_bOperator;

		hash_info1(): m_bPassed(false), m_bInited(false),m_bID(false),m_bConst(false),m_bWord(false),m_bOperator(false) {
			memset(m_Table,0,GE_HASH_TABLE_SIZE);
		}

		void unite(const hash_info1 & src){
			for (size_t i=0; i<sizeof(m_Table);i++)
				m_Table[i]|=src.m_Table[i];
			m_bID		|= src.m_bID; 
			m_bConst	|= src.m_bConst;
			m_bWord		|= src.m_bWord; 
			m_bOperator	|= src.m_bOperator;
		}

		void add(cstr szName){
			m_Table[*szName]=true;
		}

		bool appropriate(const structs::lexeme & lex) const {
			if (lex.IsConstClass())
				return m_bConst;
			else if (lex.IsIDClass())
				return m_bID;
			else 
				return m_Table[*lex.GetSzText()];
		}

		void dump(stream & s){
			for (int j=0; j<GE_HASH_TABLE_SIZE;j++) 
				if (m_Table[j])
					s<<(char)(j);

			s<<"| ";
			if (m_bID) s<<"ID ";
			if (m_bOperator) s<<"Op ";
			if (m_bWord) s<<"W";
		}
	};

	//Структура для оптимизации поиска на основе хеширования, вариант 2
	struct hash_info2{
		bool	m_bPassed;
		bool	m_bInited;

		typedef map<string_ptr,bool> map_str;

		map_str	m_Table;
		bool	m_bID; 
		bool	m_bConst;
		bool	m_bWord; 
		bool	m_bOperator;

		hash_info2(): m_bPassed(false), m_bInited(false),m_bID(false),m_bConst(false),m_bWord(false),m_bOperator(false) {
		}

		void unite(const hash_info2 & src){
			for (map_str::const_iterator i=src.m_Table.begin(); i!=src.m_Table.end();++i)
				m_Table[(*i).first]=true;
			m_bID		|= src.m_bID; 
			m_bConst	|= src.m_bConst;
			m_bWord		|= src.m_bWord; 
			m_bOperator	|= src.m_bOperator;
		}

		void add(cstr szName){
			m_Table[szName]=true;
		}

		bool appropriate(const structs::lexeme & lex) const {
			if (lex.IsConstClass())
				return m_bConst;
			else if (lex.IsIDClass())
				return m_bID;
			else 
				return m_Table.find(lex.GetSzText())!=m_Table.end();
		}

		void dump(stream & s){
			for (map_str::const_iterator i=m_Table.begin(); i!=m_Table.end();++i)
				s<<(*i).first.c_str()<<" ";

			s<<"| ";
			if (m_bID) s<<"ID ";
			if (m_bOperator) s<<"Op ";
			if (m_bWord) s<<"W";
		}
	};
private:
	ref_type<tree_gramms> *	m_pTree;
protected:
	bool		m_bRecursionToCycle;	//The sign that the recursion link was transmitted to cycle

	// Optimization
	#ifdef _SEARCH_HASH_OPTIMIZATION_1
	hash_info1 m_HashInfo;
	#endif
	#ifdef _SEARCH_HASH_OPTIMIZATION_2
	hash_info2 m_HashInfo;
	#endif


	//Количество прямых нетерминальных дочерних вершин
	size_t GetNonTerminalRootsCount();

	friend class CSntxAnalyzer;
public:
	#define HUB_PRMS1 cstr szName,const structs::sntx_error & Err, const CSntxVariant& V1
	#define HUB_PRMS2 HUB_PRMS1, const CSntxVariant& V2
	#define HUB_PRMS3 HUB_PRMS2, const CSntxVariant& V3
	#define HUB_PRMS4 HUB_PRMS3, const CSntxVariant& V4
	#define HUB_PRMS5 HUB_PRMS4, const CSntxVariant& V5
	#define HUB_PRMS6 HUB_PRMS5, const CSntxVariant& V6
	#define HUB_PRMS7 HUB_PRMS6, const CSntxVariant& V7
	#define HUB_PRMS8 HUB_PRMS7, const CSntxVariant& V8
	#define HUB_PRMS9 HUB_PRMS8, const CSntxVariant& V9
	#define HUB_PRMS10 HUB_PRMS9, const CSntxVariant& V10
	#define HUB_PRMS11 HUB_PRMS10, const CSntxVariant& V11
	#define HUB_PRMS12 HUB_PRMS11, const CSntxVariant& V12
	#define HUB_PRMS13 HUB_PRMS12, const CSntxVariant& V13
	#define HUB_PRMS14 HUB_PRMS13, const CSntxVariant& V14

	CSntxRule(HUB_PRMS1,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS2,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS3,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS4,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS5,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS6,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS7,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS8,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS9,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS10,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS11,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS12,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS13,void* Dummy=NULL);  
	CSntxRule(HUB_PRMS14,void* Dummy=NULL);  
	
	CSntxRule(const CSntxRule &src);
	CSntxRule();
	virtual ~CSntxRule();

	void AddVariant(const CSntxVariant & src);
	void OptimizeRecursing();
	void InitHashTable();

	//The reference to internal syntax tree of the hub
	tree_gramms & GetTree();

	//Returns string representation of the tree
	string_smart GetTreeDump();

	//Returns string representation of grammatic in XBNF form
	string_smart GetXBNF();

	bool AnalyzeStructure	(structs::sntx_analyze_context & Context);
	
	CSntxRule & operator = (const CSntxRule& A);
};

//------------------------------------------------------------------------
// CSntxAnalyzer  
//------------------------------------------------------------------------
class CSntxAnalyzer : public structs::grm_sntx_analyzer
{
private:
	#ifdef _NO_BACK_OPTIMIZATION
	struct terminal_data{
		size_t m_nRefs;
		structs::grm_term * m_pData;
		terminal_data(): m_nRefs(0),m_pData(NULL){
		}
	};
	#endif

	static string_smart					m_strCurFileName;
	clock_t								m_nstartClock;
	structs::sntx_analyze_context		m_AnalyzeContext;
protected:
public:
	//------------------------------------------------------------------------
	string_smart GetTreeDump() const;

	//Returns string representation of grammatic in XBNF form
	string_smart GetXBNF();

	bool Rebuild();
	bool Analyze(const ILexList & LexList, ISntxList & SntxList);
	
	static const cstr GetCurFile() 	{return m_strCurFileName.c_str();}

	CSntxAnalyzer(const IBuildContext & BuildContext, cstr szFileName);
	virtual ~CSntxAnalyzer();
};

#endif // !defined(AFX_SNTXANALYZER_H__2CF293E9_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
