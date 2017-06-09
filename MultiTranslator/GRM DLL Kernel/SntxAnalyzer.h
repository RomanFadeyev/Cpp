/********************************************************************
	created:	2002/10/06
	created:	6:10:2002   23:27
	filename: 	$(Projects)\sources\grm dll kernel\sntxhub.h
	file path:	$(Projects)\sources\grm dll kernel
	file base:	sntxhub
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Основная единица трансляции - правило
*********************************************************************/

#if !defined(AFX_SNTXHUB_H__2CF293E6_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_SNTXHUB_H__2CF293E6_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include "BaseStructs.h"
#include "LexSource.h"
#include "GrmKrnlExports.h"

class CSntxAnalyzer;
//--------------------------------------------------------------------
//sntx_analyze_context
//--------------------------------------------------------------------
typedef list <grm_symbol> CSntxList;
typedef CLexSource CLexList;

typedef grm_structs::sntx_analyze_context<grmkrnl_structs::lexeme, grmkrnl_structs::grm_symbol, CSntxRule> sntx_analyze_context_t;
class sntx_analyze_context:public sntx_analyze_context_t
{
private:
	map<string_std,bool>	m_mapKeywords;
	bool			m_bIgnoreCase;
public:
	CLexList	*	m_pLexList; //ReadOnly
	CSntxAnalyzer*	m_pAnalyzer;
	CSntxList		m_ResultChain;
	bool			m_bStopComplete;

	grmkrnl_exports::parse_info_functor		m_ParseCallBckFunc;
	grmkrnl_exports::gen_info_functor		m_GenCallBckFunc;

	bool IsKeyword(cstr szText)
	{
#pragma message(__MSG_PREFIX__"11.04.2006 THE BUG. THERE IS NO CASE ISENCE SUPPORT")
		//11.04.2006. Не учитывался реестр букв
		if (m_bIgnoreCase) 
			_ASSERTE(false);
		else
			return m_mapKeywords.find(szText)!=m_mapKeywords.end();
	}

	void SetIgnoreCase (bool bValue)		
	{
		m_bIgnoreCase=bValue;
	}

	void AddKeyword(cstr szKeyword)
	{
		m_mapKeywords[szKeyword]=true;
	}

	//Не очищает m_ParseCallBckFunc и m_GenCallBckFunc!!!
	void Clear(){
		sntx_analyze_context_t::Clear();
		m_bStopComplete=false;
		m_pLexList=NULL;
		m_pAnalyzer=NULL;
		m_mapKeywords.clear();
	}
	
	CLexList & GetLexList() const { 
		_ASSERT(m_pLexList); return *m_pLexList;
	}

	CSntxList & GetSntxList() { 
		return m_ResultChain;
	}
	CSntxAnalyzer & GetAnalyzer() { 
		_ASSERT(m_pAnalyzer);
		return * m_pAnalyzer;
	}
};

//------------------------------------------------------------------------
// CSntxVariant
//------------------------------------------------------------------------
class CSntxVariant: public sntx_variant
{
};

//------------------------------------------------------------------------
// CSntxRule
//------------------------------------------------------------------------
class CSntxRule: public grmkrnl_structs::sntx_rule
{
protected:
	size_t		m_nFuncAddr;			// Здесь будет хранится имя метки функции
	string_std	m_strFuncName;			// а после меняться на адрес
	E3State		m_nSolitaryWords;
	E3State		m_nIgnoreWhiteSpaces;

	void AnalyzeError(const grmkrnl_structs::grm_symbol & CurGE,sntx_analyze_context & Context);
private:
	void Clear();
	void SendParsePacket(grm_parse_packet & Msg, sntx_analyze_context & Context);

	void ReportEnterToRule(sntx_analyze_context & Context);
	void ReportFoundTerm(const grm_structs::text_pos & pos,sntx_analyze_context & Context);
	void ReportLeaveRule(sntx_analyze_context & Context);
	void ReportSearchAlternative(cstr szText, sntx_analyze_context & Context);
	void ReportRollbackFromRule(cstr szText, sntx_analyze_context & Context);
protected:
public:
	//--------------------------------------------------------------------

	CSntxRule(cstr szRuleName,  cstr szErrorText, cstr szFuncName, E3State nSolitaryWords, E3State nIgnoreWhiteSpaces);
	CSntxRule(const CSntxRule &src);
	CSntxRule();
	virtual ~CSntxRule();
	
	bool	IsFuncLinked() const				{return m_nFuncAddr!=UNDEFINED_ADDR;}
	void	LinkFunc(size_t nAddr)				{m_nFuncAddr=nAddr;}
	bool	CmpFuncName(cstr szFuncName) const	{return m_strFuncName.cmp(szFuncName);}

	//Read general properties
	cstr	GetFuncName()		const		{return m_strFuncName.c_str();}
	size_t	GetFuncAddr()		const		{return m_nFuncAddr;}
	E3State GetSolitaryWords()	const {
		_ASSERT(oneof(m_nSolitaryWords,VAL_ON,VAL_OFF,VAL_AUTO));		return m_nSolitaryWords;}
	E3State GetIgnoreWhiteSpaces() const {
		_ASSERT(oneof(m_nIgnoreWhiteSpaces,VAL_ON,VAL_OFF,VAL_AUTO));	return m_nIgnoreWhiteSpaces;}

	//Write general properties
	void SetSolitaryWords(E3State SolitaryWords) {
		_ASSERT(oneof(SolitaryWords,VAL_ON,VAL_OFF,VAL_AUTO));			m_nSolitaryWords=SolitaryWords;}
	void SetIgnoreWhiteSpaces(E3State IgnoreWhiteSpaces) {
		_ASSERT(oneof(IgnoreWhiteSpaces,VAL_ON,VAL_OFF,VAL_AUTO));		m_nIgnoreWhiteSpaces=IgnoreWhiteSpaces;}
	

	void AddVariant(CSntxVariant & src);
	size_t ReplaceNameLink(CSntxRule & aRule, size_t nRuleNumber);
	
	bool AnalyzeStructure(sntx_analyze_context & Context);
	bool SaveUsingStatistic(stream & Stream);


	override void read_dmp(base_utils::stream & Stream);
	override void write_dmp(base_utils::stream & Stream);

	CSntxRule & operator = (const CSntxRule& A);
};

//------------------------------------------------------------------------
// CSntxAnalyzer  
//------------------------------------------------------------------------
class CSntxAnalyzer:public grmkrnl_structs::grm_sntx_analyzer
{
private:
	clock_t					m_nstartClock;
	sntx_analyze_context	m_AnalyzeContext;
	typedef grm_structs::sntx_error sntx_error;
	
	grmkrnl_exports::pSubRoutineHandlerFunc	m_pSubRoutineHandler;
public:
	static cstr szCurrentText;


	bool			IsSyntaxError()	const	{return !m_AnalyzeContext.m_ConjecturalErrorTerms.empty();}
	string_list		GetErrorRuleName()const;
	const sntx_error GetSyntaxError();

	void SetTextGenCallBck(const grmkrnl_exports::gen_info_functor  & Func)	{
		m_AnalyzeContext.m_GenCallBckFunc = Func;}
	void SetParseCallBck(const grmkrnl_exports::parse_info_functor & Func)	{
		m_AnalyzeContext.m_ParseCallBckFunc = Func;}	

	bool Rebuild();

	bool Analyze(CLexSource & LexList, grmkrnl_exports::pSubRoutineHandlerFunc pFunc, size_t nStartRule);

	void StopAnalyze();

	bool CompleteChain();

	//Собирает всякую всячину и записывает ее в указанный stream
	//stream строковый
	void GatherInfo(stream & Stream);

	CSntxAnalyzer();
	virtual ~CSntxAnalyzer();
};

#endif // !defined(AFX_SNTXHUB_H__2CF293E6_0302_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
