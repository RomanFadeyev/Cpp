// GrammsDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <map>

#include "GrmKrnl.h"
#include "SntxAnalyzer.h"
//#include "GrammaticalInference.h"

#include <BaseUtils.h>
#include <AppUtils.h>
#include <GrmKrnlExports.h>


#ifdef _USRDLL
BOOL APIENTRY DllMain(HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	//Инициализация 
	switch  (ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
			{	
				_RPT0(_CRT_WARN,"grmkrnl DLL: process attached");

				//Do not remove!!! We must reference to ApplicationData during initialization
				if (*ApplicationData::APP_NAME==0) 
					throw "Internal Error. Global parameters were not initialized";

				ApplicationData::APP_INSTANCE=(HINSTANCE)hModule;
				break;
			}
		case DLL_PROCESS_DETACH:
			_RPT0(_CRT_WARN,"grmkrnl DLL: process detached");
			
			LOG_PROTECT_CODE_BEGIN
			base_utils::cleanup();
			_ASSERT(_CrtCheckMemory());
			LOG_PROTECT_CODE_END

			break;
	}
	return TRUE;
}
#endif    

using namespace grmkrnl_structs;

namespace grmkrnl_exports {

//реализация интерфейса IGrmKrnl
class CGrmKrnl: public IGrmKrnl
{	
private:
	int m_nRefCnt;

	CSntxAnalyzer	m_SntxAnalyzer;
	CLexSource		m_LexSource;
	grmkrnl_exports::parse_info_functor m_ParseCallBckFunc;
	grmkrnl_exports::gen_info_functor	m_GenCallBckFunc;


	//Временные контейнеры при формировании грамматик
	typedef list <CSntxVariant> list_variants;
	list <CSntxVariant>  lst_Vs;
	CSntxVariant V; 

	//Карта регулярных выражений и их имен
	typedef map<string_std,string_std> map_regexp;
	map_regexp mapRegExp;

	GRM_HANDLE GetTermID();
public:
	override HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {
		return E_NOTIMPL;
	}
	override ULONG   STDMETHODCALLTYPE AddRef(void) {
		m_nRefCnt++; return m_nRefCnt;
	}
	override ULONG   STDMETHODCALLTYPE Release(void){
		m_nRefCnt--; if (m_nRefCnt==0) delete this; return m_nRefCnt;
	}
	CGrmKrnl(): m_nRefCnt(0){
	}
	
	//Прототипы	функций
	override GRM_HANDLE STDMETHODCALLTYPE AddTerminal(cstr  Name, cstr szErrorText, cstr szFuncName, cstr szPreFuncName, BYTE Flags) ;
	override GRM_HANDLE STDMETHODCALLTYPE AddStdTerminal(EStdTerminalClass Terminal, cstr szErrorText, cstr szFuncName,cstr szPreFuncName, BYTE Flags) ;
	override GRM_HANDLE STDMETHODCALLTYPE AddRegExpTerminal(cstr szRegExpName, cstr szErrorText, cstr szFuncName,cstr szPreFuncName, BYTE Flags) ;
	override GRM_HANDLE STDMETHODCALLTYPE AddNonTerminal(cstr  Name, cstr szFuncName, cstr szPreFuncName, BYTE Flags) ;
	override GRM_HANDLE STDMETHODCALLTYPE AddVariant(cstr szFuncName, cstr szPreFuncName) ;
	override GRM_HANDLE STDMETHODCALLTYPE AddRule(cstr szRuleName, cstr szErrorText, cstr szFuncName, E3State nSolitaryWords, E3State nIgnoreWhiteSpaces) ;
	override GRM_HANDLE STDMETHODCALLTYPE AddRegExp(cstr szLexRule, cstr szRegExp) ;
	override bool STDMETHODCALLTYPE ReplaceFunctionNameToAddr(cstr szFuncName, DWORD dwAddr) ;
	override bool STDMETHODCALLTYPE CheckOnReplacement() ;
	override bool STDMETHODCALLTYPE BuildTree() ;
	override bool STDMETHODCALLTYPE CheckOnUnresolvedRules(cstr & szUnresolvedRule) ;
	override bool STDMETHODCALLTYPE CheckOnLeftSidedRecursion(cstr & szViolatedRule);

	override bool STDMETHODCALLTYPE StoreRules(base_utils::stream & Stream) ;
	override bool STDMETHODCALLTYPE LoadRules (base_utils::stream & Stream) ;

	override bool STDMETHODCALLTYPE OpenSource(cstr szName) ;
	override bool STDMETHODCALLTYPE IgnoreBlock(cstr szStart, cstr szEnd) ;
	override bool STDMETHODCALLTYPE ReplaceBlock(cstr szStart, cstr szEnd) ;
	override bool STDMETHODCALLTYPE IgnoreWhiteSpaces(BYTE YesNoAuto) ;
	override bool STDMETHODCALLTYPE SolitaryWords(BYTE YesNoAuto) ;
	override bool STDMETHODCALLTYPE IgnoreCase(BYTE YesNoAuto) ;
	override bool STDMETHODCALLTYPE Delimiters(cstr  szListOfDelimiters) ;
	override bool STDMETHODCALLTYPE StopAnalyze() ;
	override bool STDMETHODCALLTYPE SetLanguage(DWORD Lang) ;
	override bool STDMETHODCALLTYPE Analyze(pSubRoutineHandlerFunc pSubRoutineHandler, DWORD dwStartRule, analyze_data & Data) ;
	override bool STDMETHODCALLTYPE SaveStatistic(cstr szFileName)  ;
	override bool STDMETHODCALLTYPE GetRuleIndex(cstr szName, DWORD &Index);
	override cstr STDMETHODCALLTYPE GetCurrentText();
	override void STDMETHODCALLTYPE SetParseCallBck(const parse_info_functor & Func);
	override void STDMETHODCALLTYPE SetTextGenCallBck(const gen_info_functor & Func);

	//получение различной информации о грамматике
	override bool STDMETHODCALLTYPE GetGrammarInfo(base_utils::stream & Stream);

	//override IGrammaticalInference* STDMETHODCALLTYPE GetGrammaticalInferenceInterface();
};



//------------------------------------------------------------------------
GRM_HANDLE CGrmKrnl::GetTermID()
{
	return ((V.GetTermCount()+1) |  //
			(lst_Vs.size()<<16) |
			(m_SntxAnalyzer.GetRuleCount()<<24));
}
//------------------------------------------------------------------------
// ErrorCode представляет из себя индекс константы в таблице констант
// Function может быть как типа FuNC_TYPE, так и приведенный cstr. См union в SGrammatialElement
GRM_HANDLE CGrmKrnl::AddTerminal(cstr  Name, cstr szErrorText, cstr szFuncName, cstr szPreFuncName, BYTE Flags)
{
	GRM_HANDLE nID = GetTermID();
	V.AddTerm(grm_symbol(lexeme(Name),szErrorText,szFuncName,szPreFuncName,Flags,nID));
	return nID;
}
//------------------------------------------------------------------------
GRM_HANDLE CGrmKrnl::AddNonTerminal(cstr  Name, cstr szFuncName, cstr szPreFuncName, BYTE Flags)
{
	GRM_HANDLE nID = GetTermID();
	V.AddTerm(grm_symbol(Name,szFuncName,szPreFuncName,Flags,nID));
	return nID;
}
//------------------------------------------------------------------------
GRM_HANDLE CGrmKrnl::AddStdTerminal(EStdTerminalClass Terminal, cstr szErrorText, cstr szFuncName,cstr szPreFuncName, BYTE Flags)
{
	GRM_HANDLE nID = GetTermID();
	V.AddTerm(grm_symbol(Terminal,szErrorText,szFuncName,szPreFuncName,Flags,nID));
	return nID;
}
//------------------------------------------------------------------------
GRM_HANDLE CGrmKrnl::AddRegExpTerminal(cstr szRegExpName, cstr szErrorText, cstr szFuncName,cstr szPreFuncName, BYTE Flags)
{
	GRM_HANDLE nID = GetTermID();
	V.AddTerm(grm_symbol(mapRegExp[szRegExpName].c_str(),1.0f, szErrorText,szFuncName,szPreFuncName,Flags,nID));
	return nID;
}
//------------------------------------------------------------------------
GRM_HANDLE CGrmKrnl::AddVariant(cstr szFuncName, cstr szPreFuncName)
{
	V.SetFunction(szFuncName,szPreFuncName);
	lst_Vs.push_back(V);
	V.Clear();
	return 1;
}  
//------------------------------------------------------------------------
GRM_HANDLE CGrmKrnl::AddRule(cstr szRuleName, cstr szErrorText, cstr szFuncName, E3State nSolitaryWords, E3State nIgnoreWhiteSpaces)
{
	CSntxRule Rule(szRuleName, szErrorText,szFuncName,nSolitaryWords,nIgnoreWhiteSpaces);
	
	for (list_variants::iterator i=lst_Vs.begin();i!=lst_Vs.end();++i)
		Rule.AddVariant((*i));

	lst_Vs.clear();
	m_SntxAnalyzer.AddRule(Rule);
	return 1;
}
//------------------------------------------------------------------------
GRM_HANDLE CGrmKrnl::AddRegExp(cstr szLexRule, cstr szRegExp)
{
	mapRegExp[szLexRule]=szRegExp;
	return 1;
}
//------------------------------------------------------------------------
// Предполагается, что каждому правилу соответствует своя уникальная функция, и с другими правилами она не используется
bool CGrmKrnl::ReplaceFunctionNameToAddr(cstr  szFuncName, DWORD dwAddr)
{
	if (szFuncName[0]=='@') // Префункция
	{
		for (size_t i=0;i<m_SntxAnalyzer.GetRuleCount();i++)
		{
			/* У CSntxHUB нет префункции */

			for (size_t j=0;j<m_SntxAnalyzer.GetRule(i).GetVariantCount();j++)
				for (size_t k = 0; k<m_SntxAnalyzer.GetRule(i).GetVariant(j).GetTermCount();k++)
				{
					grmkrnl_structs::grm_symbol & GE = m_SntxAnalyzer.GetRule(i).GetVariant(j).GetTerm(k);

					if ((!GE.IsPreFuncLinked()) &&	(GE.IsHasPreFunc()) && 	(GE.CmpPreFuncName(szFuncName)))
					{
						GE.LinkPreFunc(dwAddr);
						return true;
					}
				}
		}
		return false;
	}
	else
	{
		for (size_t i=0;i<m_SntxAnalyzer.GetRuleCount();i++)
		{
			CSntxRule & Rule = m_SntxAnalyzer.GetRule(i);
			if ((!Rule.IsFuncLinked())&& (Rule.CmpFuncName(szFuncName)))
			{
				Rule.LinkFunc(dwAddr);
				return true;
			}
	
			for (size_t j=0;j<Rule.GetVariantCount();j++)
				for (size_t k = 0; k<Rule.GetVariant(j).GetTermCount();k++)
				{
					grmkrnl_structs::grm_symbol & GE = Rule.GetVariant(j).GetTerm(k);

					if ((!GE.IsFuncLinked()) && (GE.CmpFuncName(szFuncName)))
					{
						GE.LinkFunc(dwAddr);
						return true;
					}
				}
		}
		return false;
	}
} 
//------------------------------------------------------------------------
// Проверяет на то, чтобы все элементы имели замещение имени функции на алрес
bool CGrmKrnl::CheckOnReplacement()
{
	for (size_t i=0;i<m_SntxAnalyzer.GetRuleCount();i++)
	{
		CSntxRule & Rule = m_SntxAnalyzer.GetRule(i);
		
		if (!Rule.IsFuncLinked())
			return false;
	
		for (size_t j=0;j<Rule.GetVariantCount();j++)
			for (size_t k = 0; k<Rule.GetVariant(j).GetTermCount();k++)
				if (!Rule.GetVariant(j).GetTerm(k).IsFuncLinked())
					return false;
				
	}
	return true;	
}
//------------------------------------------------------------------------
bool CGrmKrnl::BuildTree()
{
	return m_SntxAnalyzer.Rebuild();
}
//------------------------------------------------------------------------
bool CGrmKrnl::CheckOnUnresolvedRules(cstr  & szUnresolvedRule)
{
	for (size_t i=0;i<m_SntxAnalyzer.GetRuleCount();i++)
		for (size_t j=0;j<m_SntxAnalyzer.GetRule(i).GetVariantCount();j++)
			for (size_t k = 0; k<m_SntxAnalyzer.GetRule(i).GetVariant(j).GetTermCount();k++)
			{
				grmkrnl_structs::grm_symbol & GE = m_SntxAnalyzer.GetRule(i).GetVariant(j).GetTerm(k);
				if (GE.IsNonterminalLinkByName()) 
				{
					szUnresolvedRule=GE.GetLinkedRuleName();
					return false;
				}
			}

	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::CheckOnLeftSidedRecursion(cstr & szViolatedRule)
{
	for (size_t i=0;i<m_SntxAnalyzer.GetRuleCount();i++){
		grmkrnl_structs::sntx_rule_t * pRule = m_SntxAnalyzer.GetRule(i).DetectLeftSidedRecursion();
		if (pRule){
			szViolatedRule = pRule->GetName();
			return false;
		}
	}
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::StoreRules(base_utils::stream & Stream)
{	
	m_SntxAnalyzer.write_dmp(Stream);
	return true;
} 
//------------------------------------------------------------------------
bool CGrmKrnl::LoadRules(base_utils::stream & Stream)
{
	m_SntxAnalyzer.read_dmp(Stream);
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::OpenSource(cstr  Name)
{
	if (!m_LexSource.OpenFile(Name))
		return false;
	return true;
} 
//------------------------------------------------------------------------
bool CGrmKrnl::IgnoreBlock(cstr  Start, cstr  End)
{
	m_LexSource.AddIgnoreBlock(Start,End);
	return true;
} 
//------------------------------------------------------------------------
bool CGrmKrnl::ReplaceBlock(cstr  Start, cstr  End)
{
	m_LexSource.AddReplaceBlock(Start,End);
	return true;
}
//------------------------------------------------------------------------
#pragma warning (disable: 4800)
bool CGrmKrnl::IgnoreWhiteSpaces(BYTE YesNoAuto)
{
	_ASSERT(oneof((E3State)YesNoAuto,VAL_ON,VAL_OFF,VAL_AUTO));
	if (YesNoAuto!=VAL_AUTO)
		m_LexSource.SetIgnoreWhiteSpaces(YesNoAuto);
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::SolitaryWords(BYTE YesNoAuto)
{
	_ASSERT(oneof((E3State)YesNoAuto,VAL_ON,VAL_OFF,VAL_AUTO));
	if (YesNoAuto!=VAL_AUTO)
		m_LexSource.SetSolitaryWords(YesNoAuto);
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::IgnoreCase(BYTE YesNoAuto)
{
	_ASSERT(oneof((E3State)YesNoAuto,VAL_ON,VAL_OFF,VAL_AUTO));
	if (YesNoAuto!=VAL_AUTO)
		m_LexSource.SetIgnoreCase(YesNoAuto);
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::Delimiters(cstr  ListOfDelimiters)
{
	m_LexSource.SetDelimiters(ListOfDelimiters);
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::StopAnalyze()
{
	m_SntxAnalyzer.StopAnalyze();
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::SetLanguage(DWORD Lang)
{
	Language=Lang;
	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::Analyze(pSubRoutineHandlerFunc pSubRoutineHandler, DWORD dwStartRule, analyze_data & Data)
{
	Data.clear();	

	if ((!m_SntxAnalyzer.Analyze(m_LexSource,pSubRoutineHandler,dwStartRule)) 
		|| (!m_LexSource.isEnd()))	
	{
		if (m_SntxAnalyzer.IsSyntaxError())
		{
			grm_structs::sntx_error  err = m_SntxAnalyzer.GetSyntaxError();
			
			Data.szErrorText[sizeof(Data.szErrorText)-1]=0;
			strncpy(Data.szErrorText,err.GetText().c_str(),sizeof(Data.szErrorText));

			Data.szErrorRuleName[sizeof(Data.szErrorRuleName)-1]=0;
			strncpy(Data.szErrorRuleName,m_SntxAnalyzer.GetErrorRuleName().c_str(),sizeof(Data.szErrorRuleName));

			Data.dwLine = err.GetPos().m_nLine;
			Data.dwPos = err.GetPos().m_nCol;
			Data.bPossibleError=false;
		}
		else
		{	//Остальные поля в Data заполнены нулями
			Data.bPossibleError=true;
		}
		return false;
	}

	if (m_ParseCallBckFunc)
	{
		string_res strText(IDS_REPORT_EXEC_ACTIONS,ApplicationData::APP_INSTANCE);
		
		grm_parse_packet Msg;
		Msg.Type=GE_NULL;
		Msg.szMessage=strText.c_str();
		Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoMSG;
		Msg.szMessage=strText.c_str();

		m_ParseCallBckFunc(Msg);
	}
	return  m_SntxAnalyzer.CompleteChain();
}
#pragma warning (default: 4800)
//------------------------------------------------------------------------
bool CGrmKrnl::SaveStatistic(cstr szFileName)
{
	base_utils::file_std stream;
	
	if (!stream.open(szFileName,"wb"))
		return false;
	
	try
	{
		stream<<m_SntxAnalyzer.GetRuleCount();
		for (size_t i=0;i<m_SntxAnalyzer.GetRuleCount();i++)
			if (!m_SntxAnalyzer.GetRule(i).SaveUsingStatistic(stream))
				return false;
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//------------------------------------------------------------------------
bool CGrmKrnl::GetRuleIndex(cstr szName, DWORD &Index)
{
	for (Index=0;Index<(DWORD)m_SntxAnalyzer.GetRuleCount();Index++)
		if (_tcscmp(szName, m_SntxAnalyzer.GetRule(Index).GetName())==0)
			return true;
	return false;
}
//------------------------------------------------------------------------
cstr CGrmKrnl::GetCurrentText()
{
	return CSntxAnalyzer::szCurrentText;
}
//------------------------------------------------------------------------
void CGrmKrnl::SetParseCallBck(const parse_info_functor & Func)
{
	m_ParseCallBckFunc=Func;
	m_SntxAnalyzer.SetParseCallBck(m_ParseCallBckFunc);
	m_LexSource.SetParseCallBck(m_ParseCallBckFunc);
}
//------------------------------------------------------------------------
//Назначение функции для мониторинга процесса генерации текста (после парсинга)
void CGrmKrnl::SetTextGenCallBck(const gen_info_functor  & Func)
{
	m_GenCallBckFunc=Func;
	m_SntxAnalyzer.SetTextGenCallBck(m_GenCallBckFunc);
}
//------------------------------------------------------------------------
bool CGrmKrnl::GetGrammarInfo(base_utils::stream & Stream)
{
	m_SntxAnalyzer.GatherInfo(Stream);
	return true;
}
//------------------------------------------------------------------------
/*IGrammaticalInference* CGrmKrnl::GetGrammaticalInferenceInterface()
{
	return new CGrammaticalInference(m_SntxAnalyzer,m_LexSource);
}
*/
}; //namespace grmkrnl_exports

//------------------------------------------------------------------------
grmkrnl_exports::IGrmKrnl* grmkrnl_exports::GetGrmKrnlInterface()
{
	return new CGrmKrnl;
}