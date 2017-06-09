/********************************************************************
	created:	2002/10/05
	created:	5:10:2002	10:31
	filename:	$(Projects)\sources\grmkrnlexports.h
	file path:	$(Projects)\sources
	file base:	grmkrnlexports
	file ext:	h
	author:		Fadeyev	R.V.
	
	purpose:	
*********************************************************************/
#include <BaseUtils.h>
#include <Functors.h>

#include <GrmStructs.h>

#pragma	once
using namespace	base_utils;

#ifdef _GRM_KRNL_
  #ifdef _USRDLL 
		#pragma	message("\n--- Exporting grmkrnl functions... ---\n")
		#define	GRM_KRNL_DLL_EXPORT	 _declspec(dllexport)
  #else
	  #define GRM_KRNL_DLL_EXPORT
  #endif
#else
  #define GRM_KRNL_DLL_EXPORT  _declspec(dllimport)
#endif

//Примечание. В эспорте необходимо использовать стандартные Windows-типы - WORD, DWORD, BYTE  и пр
//так как они являются переносимыми между компиляторами
namespace grmkrnl_exports
{ 
	struct parse_info_packet
	{
		enum ECmd {nfoFOUND, nfoENTER, nfoEXIT_TRUE, nfoROLLBACK, nfoNEXT_VARIANT, nfoMSG};
		
		cstr 	szMessage;
		cstr 	szText;
		cstr	szCurHubName;
		ETermClass			Type;
		ELexemeCLass		LexemeType;
		EStdTerminalClass	StandardTerminal;// нужен для обеспечения некоторых заранее определенных терминалов, как то ID, CONST ...

		//Current Position of Lexical Analyzer
		cstr	szFileName;
		DWORD	dwLine;
		DWORD	dwPos;
		cstr	szTextAhead;
		cstr	szTextBehind;

		ECmd	dwCmd;
		DWORD	dwRecursionLevel;
		parse_info_packet(){
			ZeroMemory(this,sizeof(*this));
		}
	};

	struct gen_info_packet
	{
		DWORD	dwItemID;
		cstr	szText;
	};

	struct analyze_data
	{
		DWORD	dwLine;
		DWORD	dwPos;
		bool	bPossibleError;
		char_t  szErrorRuleName[1024];	//не очень красиво, зато никаких проблем
		char_t	szErrorText[1024];		//с выделением освобождением памяти в разных модулях

		void clear()	{dwLine=dwPos=0;bPossibleError=false;szErrorRuleName[0]=szErrorText[0]=0;}
	};

	// Функтор void	(info_packet &). Через него назначается обратная 
	// функция получения данных
	typedef functors::functor<void, ARGS_1(parse_info_packet &)> parse_info_functor;

	// Функтор void	(info_packet &). Через него назначается обратная 
	// функция получения данных
	typedef functors::functor<void, ARGS_1(gen_info_packet &)> gen_info_functor;

	typedef	void (*pSubRoutineHandlerFunc) (DWORD SubRoutineAddr);
	
	typedef size_t GRM_HANDLE;
	interface IGrammaticalInference: public IUnknown
	{
		struct word_repetition_data{
			string m_strWord;
			size_t m_nRepetiton;
		};
		typedef list<word_repetition_data> list_wrd;

		//Grammatical Inference. Before call this func, you may set parse callback function 
		//in this case you will receive notifications about parsing
		virtual void STDMETHODCALLTYPE ParseLexical(cstr szDumpFile) PURE;
		
		//Найти соответствия некоторых частей текста в других грамматиках
		virtual void STDMETHODCALLTYPE FindCorrespondence(cstr szDumpFile) PURE; 	

		//this function builds the simplest grammar from the text, which 
		//has been proccessed by ParseLexical
		virtual void STDMETHODCALLTYPE BuildSyntaxStructure(cstr szDumpFile) PURE;

		//this function searches for word repetition  in the text, using results provided 
		//by BuildSyntaxStructure 
		virtual list_wrd * STDMETHODCALLTYPE SearchWordRepetition(cstr szDumpFile) PURE;
	};

	interface IGrmKrnl: public IUnknown
	{	

		//Прототипы	самих функций
		virtual GRM_HANDLE STDMETHODCALLTYPE AddTerminal(cstr  Name, cstr szErrorText, cstr szFuncName, cstr szPreFuncName, BYTE Flags) =0;
		virtual GRM_HANDLE STDMETHODCALLTYPE AddStdTerminal(EStdTerminalClass Terminal, cstr szErrorText, cstr szFuncName,cstr szPreFuncName, BYTE Flags) =0;
		virtual GRM_HANDLE STDMETHODCALLTYPE AddRegExpTerminal(cstr szRegExpName, cstr szErrorText, cstr szFuncName,cstr szPreFuncName, BYTE Flags) =0;
		virtual GRM_HANDLE STDMETHODCALLTYPE AddNonTerminal(cstr  Name, cstr szFuncName, cstr szPreFuncName, BYTE Flags) =0;
		virtual GRM_HANDLE STDMETHODCALLTYPE AddVariant(cstr szFuncName, cstr szPreFuncName) =0;
		virtual GRM_HANDLE STDMETHODCALLTYPE AddRule(cstr szRuleName, cstr szErrorText, cstr szFuncName, E3State nSolitaryWords, E3State nIgnoreWhiteSpaces) =0;
		virtual GRM_HANDLE STDMETHODCALLTYPE AddRegExp(cstr szLexRule, cstr szRegExp) =0;
		virtual bool STDMETHODCALLTYPE ReplaceFunctionNameToAddr(cstr szFuncName, DWORD dwAddr) =0;
		virtual bool STDMETHODCALLTYPE CheckOnReplacement() =0;
		virtual bool STDMETHODCALLTYPE BuildTree() =0;
		virtual bool STDMETHODCALLTYPE CheckOnUnresolvedRules(cstr & szUnresolvedRule) =0;
		//Надо переделать, чтобы возвращался указатель, чтобы можно было собрать информацию о строке и проч
		virtual bool STDMETHODCALLTYPE CheckOnLeftSidedRecursion(cstr & szViolatedRule) =0;
				
		virtual bool STDMETHODCALLTYPE StoreRules(base_utils::stream & Stream) =0;
		virtual bool STDMETHODCALLTYPE LoadRules (base_utils::stream & Stream) =0;
		
		virtual bool STDMETHODCALLTYPE OpenSource(cstr szName) =0;
		virtual bool STDMETHODCALLTYPE IgnoreBlock(cstr szStart, cstr szEnd) =0;
		virtual bool STDMETHODCALLTYPE ReplaceBlock(cstr szStart, cstr szEnd) =0;
		virtual bool STDMETHODCALLTYPE IgnoreWhiteSpaces(BYTE YesNoAuto) =0;
		virtual bool STDMETHODCALLTYPE SolitaryWords(BYTE YesNoAuto) =0;
		virtual bool STDMETHODCALLTYPE IgnoreCase(BYTE YesNoAuto) =0;
		virtual bool STDMETHODCALLTYPE Delimiters(cstr  szListOfDelimiters) =0;
		virtual bool STDMETHODCALLTYPE StopAnalyze() =0;
		virtual bool STDMETHODCALLTYPE SetLanguage(DWORD Lang) =0;
		virtual bool STDMETHODCALLTYPE Analyze(pSubRoutineHandlerFunc pSubRoutineHandler, DWORD dwStartRule, analyze_data & Data) =0;
		virtual bool STDMETHODCALLTYPE SaveStatistic(cstr szFileName) =0 ;
		virtual bool STDMETHODCALLTYPE GetRuleIndex(cstr szName, DWORD &Index)=0;
		virtual cstr STDMETHODCALLTYPE GetCurrentText()=0;
		virtual void STDMETHODCALLTYPE SetParseCallBck(const parse_info_functor & Func)=0;
		virtual void STDMETHODCALLTYPE SetTextGenCallBck(const gen_info_functor & Func)=0;

		//получение различной информации о грамматике
		virtual bool STDMETHODCALLTYPE GetGrammarInfo(base_utils::stream & Stream) =0;

		//Это пока не живет нормально...
		//virtual IGrammaticalInference* STDMETHODCALLTYPE GetGrammaticalInferenceInterface() = 0;
	};
	
	//Единственная экспортируемая функция, посредством которой выдается интерфейс IGrmKrnl
	const cstr szf_GetGrmKrnlInterface = "GetGrmKrnlInterface";
	typedef	IGrmKrnl* (*pf_GetGrmKrnlInterface) ();
	extern "C" GRM_KRNL_DLL_EXPORT	IGrmKrnl * GetGrmKrnlInterface();
}		

