// Functions2.cpp: implementation of the CFunctions class. - ”правление грамматиками
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Functions.h"
#include "SntxTables.h"
#include "EWQueue.h"
#include "FunctionsTable.h"
#include "Rulestable.h"
#include "DLLSupport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


string_smart		CFunctions::m_strCurrentRuleName;
string_smart		CFunctions::m_strCurrentVariantName;
string_smart		CFunctions::m_strCurrentTermName;

size_t				CFunctions::VariantsInCurrentHub;
size_t				CFunctions::TermsInCurrentVariant;
string_std			CFunctions::m_strCurRuleError;	
string_std			CFunctions::m_strCurTermError;	

E3State				CFunctions::CurrentRuleIgnoreWhiteSpaces;
E3State				CFunctions::CurrentRuleSolitaryWords;

BYTE				CFunctions::CurrentTermFlags		=0;
CRulesTable			CFunctions::m_RuleTable;
CLexRuleTable		CFunctions::m_LexRuleTable;
stack_ex<structs::lexeme>	CFunctions::m_GrammsStack;
//------------------------------------------------------------------------
ret_t CFunctions::PreCreateRule	( const ISmntAnalyzeContext & Context)	
{
	LeftOP=Context.GetLexeme();
	
	_ASSERT(LeftOP.IsConstClass());
	_ASSERT(LeftOP.pType);
	
	if (!LeftOP.pType->IsString()) {
		errors::e_need_string_const(LeftOP,0);
		return false;
	}

	m_strCurrentRuleName=LeftOP.GetSzPureText(); 
	VariantsInCurrentHub=0;
	
	CurrentRuleSolitaryWords=VAL_AUTO;
	CurrentRuleIgnoreWhiteSpaces=VAL_AUTO;
	m_strCurRuleError.erase();

	size_t Index;
	if (!(m_RuleTable.RegisterRule(m_strCurrentRuleName.c_str(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return FALSE;
	}
	

	LeftOP.Set(SUBROUTINE_CLASS_ID, Index, m_RuleTable.GetRuleName(Index));
	PushIntoGrammsStack(LeftOP); // ƒл€ CreateRule и CreateRuleSubRoutine
	
	_ASSERT(!m_bInSubRoutine);	
	_ASSERT(!m_bInFunction);	
	m_bInSubRoutine=true;
	
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateRuleSubRoutine(const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(LeftOP);
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,SUBROUTINE));
	PushIntoGrammsStack(LeftOP);// ƒл€ CreateRule 
	IncLevel(Context);

	_ASSERT(!m_bInFunction);
	_ASSERT(m_bInSubRoutine);

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormENDSUBR( const ISmntAnalyzeContext & Context)
{
	DecLevel(Context);
	Context.GetPCodeList().Add(structs::code_cmd(ENDSUBR));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateRule( const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(LeftOP);					// здесь лежит им€ терминала. ѕоложено PreCreateTeraminal

	_ASSERT (LeftOP.GetClass()==SUBROUTINE_CLASS_ID);
												// Here Ѕрать нужно именно LeftOP и ни в коем случае не m_strCurrentRuleName
	(pIGrmKrnl->AddRule)(m_strCurrentRuleName.c_str(), m_strCurRuleError.c_str(), LeftOP.GetSzText(), CurrentRuleSolitaryWords, CurrentRuleIgnoreWhiteSpaces);

	_ASSERT(!m_bInFunction);
	_ASSERT(m_bInSubRoutine);

	m_bInSubRoutine=false;

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::PreCreateVariant( const ISmntAnalyzeContext & Context)
{
	static char_t S[32];
	itoa(VariantsInCurrentHub,S,10);
	m_strCurrentVariantName=S;

	string_smart VariantName(m_strCurrentRuleName.c_str(),"@",m_strCurrentVariantName.c_str());

	VariantsInCurrentHub++;
	TermsInCurrentVariant=0;

	size_t Index;
	if (!(m_RuleTable.RegisterRule(VariantName.c_str(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),Context.GetLexeme());
		return FALSE;
	}
	
	LeftOP.Set(SUBROUTINE_CLASS_ID,Index,m_RuleTable.GetRuleName(Index));
	PushIntoGrammsStack(LeftOP); // ƒл€ CreateVAriant & CreateVariantSubRoutine

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreatePreVarSubroutine ( const ISmntAnalyzeContext & Context)	
{
	static char_t S[32];
	itoa(VariantsInCurrentHub,S,10);
	m_strCurrentVariantName=S;
	//ѕрепроцедура отмечаетс€ перед фактическим именем знаком @
	string_smart PreVariantSubroutineName("@",m_strCurrentRuleName.c_str(),"@",m_strCurrentVariantName.c_str());

	size_t Index;
	if (!(m_RuleTable.RegisterRule(PreVariantSubroutineName.c_str(),Index)))
	{
		_ASSERT(0);
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),Context.GetLexeme());
		return FALSE;
	}
	

	LeftOP.Set(SUBROUTINE_CLASS_ID,Index,m_RuleTable.GetRuleName(Index));
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,SUBROUTINE));
	PushIntoGrammsStack(LeftOP);// ƒл€ CreateVariantWithPreSub
	IncLevel(Context);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateVariantSubRoutine( const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(LeftOP);
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,SUBROUTINE));
	PushIntoGrammsStack(LeftOP);// ƒл€ CreateVariant
	IncLevel(Context);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateVariantWithPreSub( const ISmntAnalyzeContext & Context)
{
	PopFromGrammsStack(LeftOP);					// здесь лежит им€ варианта  
	PopFromGrammsStack(RightOP);					// здесь лежит им€ префункции
	_ASSERT (LeftOP.GetClass()==SUBROUTINE_CLASS_ID);
	(pIGrmKrnl->AddVariant)(LeftOP.GetSzText(), RightOP.GetSzText());
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateVariant( const ISmntAnalyzeContext & Context)
{
	PopFromGrammsStack(LeftOP);					// здесь лежит им€ варианта  
	_ASSERT (LeftOP.GetClass()==SUBROUTINE_CLASS_ID);
	(pIGrmKrnl->AddVariant)(LeftOP.GetSzText(), NULL);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::SetOptionalTermFlag( const ISmntAnalyzeContext & Context)	
{
	CurrentTermFlags|=structs::grm_term::flagOptional;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::SetChidOptionalTermFlag( const ISmntAnalyzeContext & Context)	
{
	CurrentTermFlags|=structs::grm_term::flagChildOptional;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::ClearTermFlags( const ISmntAnalyzeContext & Context)	
{
	CurrentTermFlags=0;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreatePreTermSubroutine( const ISmntAnalyzeContext & Context)	
{
	LeftOP=Context.GetLexeme();
	static char_t S[32];
	itoa(TermsInCurrentVariant,S,10);

	m_strCurrentTermName=LeftOP.GetSzText();
	string_smart TermName; 
	TermName.cat("@").cat(m_strCurrentRuleName.c_str()).cat("@").cat(m_strCurrentVariantName.c_str()).
			 cat("@").cat(m_strCurrentTermName.c_str()).cat("@").cat("T").cat(S);
	
	size_t Index;
	if (!(m_RuleTable.RegisterRule(TermName.c_str(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return FALSE;
	}

	LeftOP.Set(SUBROUTINE_CLASS_ID,Index,m_RuleTable.GetRuleName(Index));
	PushIntoGrammsStack(LeftOP); // ƒл€ CreateTerminal With PreSub
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,SUBROUTINE));
	IncLevel(Context);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::PreCreateTerminal		( const ISmntAnalyzeContext & Context)	
{
	m_strCurTermError.erase();
	CurrentTermType=NO_STD_TERMINAL;
	LeftOP=Context.GetLexeme();
	//Std terminal
	if (LeftOP.IsWordClass())
	{
		// можно было просто сравнить имена лексем, но такой подход безопаснее на случай, если такие зарезервированные слова будут удалены 
		if (LeftOP.Index== Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_ID"))
			CurrentTermType=STD_ID;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_INT_CONST"))
			CurrentTermType=STD_INT_CONST;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_NUM_CONST"))
			CurrentTermType=STD_NUM_CONST;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_SQMSTR_CONST"))
			CurrentTermType=STD_SQMSTR_CONST;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_DQMSTR_CONST"))
			CurrentTermType=STD_DQMSTR_CONST;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_STR_CONST"))
			CurrentTermType=STD_STR_CONST;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_COMMON_CONST"))	
			CurrentTermType=STD_COMMON_CONST;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_LINE"))
			CurrentTermType=STD_LINE;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_ALPHA"))
			CurrentTermType=STD_ALPHA;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_NUM"))
			CurrentTermType=STD_NUM;			
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_ALNUM"))
			CurrentTermType=STD_ALNUM;
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_PUNCT"))
			CurrentTermType=STD_PUNCT;			
		else if (LeftOP.Index==Context.GetLexTable().GetIndex(WORD_CLASS_ID,"STD_CNTRL"))
			CurrentTermType=STD_CNTRL;			
		else
			_ASSERT(0);
	}
	//Terminal (string constant)
	else if (LeftOP.GetClass()==CONST_CLASS_ID)
	{
			if (!LeftOP.pType->IsString()) {
			errors::e_need_string_const(LeftOP,0);
			return false;
		}
	}
	//Regular Expression
	else 
	{
		_ASSERT(LeftOP.GetClass()==ID_CLASS_ID);
		if (!m_LexRuleTable.FindRule(LeftOP.GetSzPureText())){
			errors::e_symbol_undefined(LeftOP,0);
			return false;
		}
		CurrentTermType=EStdTerminalClass(-GE_REGEXP_TERMINAL);		// ривовато
	}
	
	//_ASSERT(LeftOP.Name[0]=='"');
	static char_t S[32];
	itoa(TermsInCurrentVariant,S,10);

	m_strCurrentTermName=LeftOP.GetSzPureText();
	string_smart TermName(m_strCurrentRuleName.c_str(),"@",m_strCurrentVariantName.c_str(),"@",m_strCurrentTermName.c_str(),"@","T",S);
	
	TermsInCurrentVariant++;

	size_t Index;
	if (!(m_RuleTable.RegisterRule(TermName.c_str(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return FALSE;
	}

	LeftOP.Set(SUBROUTINE_CLASS_ID,Index,m_RuleTable.GetRuleName(Index));
	PushIntoGrammsStack(LeftOP); // ƒл€ CreateTerminal
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,SUBROUTINE));
	IncLevel(Context);

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateTerminal	( const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(LeftOP);					// здесь лежит им€ собранное им€ терминала. ѕоложено PreCreateTerminal
	_ASSERT (LeftOP.GetClass()==SUBROUTINE_CLASS_ID);

	grmkrnl_exports::GRM_HANDLE hID;
	if (CurrentTermType==NO_STD_TERMINAL)
		hID=(pIGrmKrnl->AddTerminal)(m_strCurrentTermName.c_str(),m_strCurTermError.c_str(),LeftOP.GetSzText(),NULL,CurrentTermFlags);
	else if (CurrentTermType==EStdTerminalClass(-GE_REGEXP_TERMINAL))
		hID=(pIGrmKrnl->AddRegExpTerminal)(m_strCurrentTermName.c_str(),m_strCurTermError.c_str(),LeftOP.GetSzText(),NULL, CurrentTermFlags);
	else
		hID=(pIGrmKrnl->AddStdTerminal)(CurrentTermType,m_strCurTermError.c_str(),LeftOP.GetSzText(),NULL, CurrentTermFlags);

	_ASSERT(hID!=NULL);
	Context.GetLangGenTable().OnGrmItem(m_strCurrentRuleName.c_str(), 
										m_strCurrentVariantName.c_str(),
										m_strCurrentTermName.c_str(),hID);

	if (m_strCurrentTermName.quant(' ')>0)
	{
		structs::text_container s(m_strCurrentTermName.c_str(),LeftOP.GetLocation());
		errors::w_strange_terminal(s,0);
		return false;
	}
	ClearTermFlags(Context);
	
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateTerminalWithPreSub		( const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(LeftOP);					// здесь лежит им€ собранное им€ терминала. ѕоложено PreCreateTeraminal
	PopFromGrammsStack(RightOP);				// PreSub

	_ASSERT (LeftOP.GetClass()==SUBROUTINE_CLASS_ID);
	grmkrnl_exports::GRM_HANDLE hID;

	if (CurrentTermType==NO_STD_TERMINAL)
		hID = (pIGrmKrnl->AddTerminal)(m_strCurrentTermName.c_str(),m_strCurTermError.c_str(),LeftOP.GetSzText(),RightOP.GetSzText(),CurrentTermFlags);
	else if (CurrentTermType==EStdTerminalClass(-GE_REGEXP_TERMINAL))
		hID = (pIGrmKrnl->AddRegExpTerminal)(m_strCurrentTermName.c_str(),m_strCurTermError.c_str(),LeftOP.GetSzText(),NULL, CurrentTermFlags);
	else
		hID = (pIGrmKrnl->AddStdTerminal)(CurrentTermType,m_strCurTermError.c_str(),LeftOP.GetSzText(), RightOP.GetSzText(),CurrentTermFlags);
		

	_ASSERT(hID!=NULL);
	Context.GetLangGenTable().OnGrmItem(m_strCurrentRuleName.c_str(), 
		m_strCurrentVariantName.c_str(),
		m_strCurrentTermName.c_str(),hID);

	if (m_strCurrentTermName.quant(' ')>0)
	{
		structs::text_container s(m_strCurrentTermName.c_str(),LeftOP.GetLocation());
		errors::w_strange_terminal(s,0);
		return FALSE;
	}
	ClearTermFlags(Context);
	
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreatePreUTermSubroutine		( const ISmntAnalyzeContext & Context)	
{
	LeftOP=Context.GetLexeme();
	
	static char_t S[32];
	itoa(TermsInCurrentVariant,S,10);

	m_strCurrentTermName=LeftOP.GetSzPureText();
	string_smart TermName("@",m_strCurrentRuleName.c_str(),"@",m_strCurrentVariantName.c_str(),"@",m_strCurrentTermName.c_str(),"@","UT",S);
	
	size_t Index;
	if (!(m_RuleTable.RegisterRule(TermName.c_str(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return FALSE;
	}

	LeftOP.Set(SUBROUTINE_CLASS_ID,Index,m_RuleTable.GetRuleName(Index));
	PushIntoGrammsStack(LeftOP); // ƒл€ CreateNonterminalWithPreSub
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,SUBROUTINE));
	IncLevel(Context);

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::PreCreateNonterminal		( const ISmntAnalyzeContext & Context)	
{
	LeftOP=Context.GetLexeme();
	
	_ASSERT(LeftOP.IsConstClass());
	if (!LeftOP.pType->IsString()) {
		errors::e_need_string_const(LeftOP,0);
		return false;
	}

	static char_t S[32];
	itoa(TermsInCurrentVariant,S,10);

	m_strCurrentTermName=LeftOP.GetSzPureText();
	string_smart TermName(m_strCurrentRuleName.c_str(),"@",m_strCurrentVariantName.c_str(),"@",m_strCurrentTermName.c_str(),"@","UT",S);
	
	TermsInCurrentVariant++;


	size_t Index;
	if (!(m_RuleTable.RegisterRule(TermName.c_str(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return FALSE;
	}

	LeftOP.Set(SUBROUTINE_CLASS_ID,Index,m_RuleTable.GetRuleName(Index));
	PushIntoGrammsStack(LeftOP); // ƒл€ CreateNonterminal
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,SUBROUTINE));
	IncLevel(Context);

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateNonterminal(const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(LeftOP);					// здесь лежит им€ терминала
	_ASSERT (LeftOP.GetClass()==SUBROUTINE_CLASS_ID);

	grmkrnl_exports::GRM_HANDLE hID = (pIGrmKrnl->AddNonTerminal)(m_strCurrentTermName.c_str(),LeftOP.GetSzText(),NULL,CurrentTermFlags);

	_ASSERT(hID!=NULL);
	Context.GetLangGenTable().OnGrmItem(m_strCurrentRuleName.c_str(), 
										m_strCurrentVariantName.c_str(),
										m_strCurrentTermName.c_str(),hID);
	
	ClearTermFlags(Context);

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateNonterminalWithPreSub	( const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(LeftOP);					// здесь лежит им€ терминала
	PopFromGrammsStack(RightOP);
	_ASSERT (LeftOP.GetClass()==SUBROUTINE_CLASS_ID);

	grmkrnl_exports::GRM_HANDLE hID =(pIGrmKrnl->AddNonTerminal)(m_strCurrentTermName.c_str(),LeftOP.GetSzText(),RightOP.GetSzText(),CurrentTermFlags);
	
	_ASSERT(hID!=NULL);
	Context.GetLangGenTable().OnGrmItem(m_strCurrentRuleName.c_str(), 
										m_strCurrentVariantName.c_str(),
										m_strCurrentTermName.c_str(),hID);	
	ClearTermFlags(Context);

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::SetRuleIgnoreWhiteSpaces( const ISmntAnalyzeContext & Context)	
{
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.IsWordClass());
	size_t OnIndex,OffIndex,AutoIndex;
	// можно было просто сравнить имена лексем, но такой подход безопаснее на случай, если такие зарезервированные слова будут удалены 
	OnIndex		=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"on");
	OffIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"off");
	AutoIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"auto");
	
	if (LeftOP.Index==OnIndex) 
		CurrentRuleIgnoreWhiteSpaces=VAL_ON;
	else if (LeftOP.Index==OffIndex)
		CurrentRuleIgnoreWhiteSpaces=VAL_OFF;
	else if (LeftOP.Index==AutoIndex)
		CurrentRuleIgnoreWhiteSpaces=VAL_AUTO;
	else
		_ASSERT(0);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::SetRuleSolitaryWords( const ISmntAnalyzeContext & Context)	
{
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.GetClass()==WORD_CLASS_ID);
	size_t OnIndex,OffIndex,AutoIndex;
	// можно было просто сравнить имена лексем, но такой подход безопаснее на случай, если такие зарезервированные слова будут удалены 
	OnIndex		=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"on");
	OffIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"off");
	AutoIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"auto");
	
	if (LeftOP.Index==OnIndex) 
		CurrentRuleSolitaryWords = VAL_ON;
	else if (LeftOP.Index==OffIndex)
		CurrentRuleSolitaryWords = VAL_OFF;
	else if (LeftOP.Index==AutoIndex)
		CurrentRuleSolitaryWords = VAL_AUTO;
	else
		_ASSERT(0);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::SetRuleErrorCode( const ISmntAnalyzeContext & Context)	
{
	m_strCurRuleError = Context.GetLexeme().Get_Str_Text();
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::SetTermErrorCode( const ISmntAnalyzeContext & Context)	
{
	m_strCurTermError = Context.GetLexeme().Get_Str_Text();
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSolitaryWords( const ISmntAnalyzeContext & Context)	
{
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.GetClass()==WORD_CLASS_ID);
	size_t OnIndex,OffIndex,AutoIndex;
	// можно было просто сравнить имена лексем, но такой подход безопаснее на случай, если такие зарезервированные слова будут удалены 
	OnIndex		=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"on");
	OffIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"off");
	AutoIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"auto");
	
	LeftOP.SetClass(DIRECT_NUMBER_CLASS_ID);

	if (LeftOP.Index==OnIndex) 
		LeftOP.Index = VAL_ON;
	else if (LeftOP.Index==OffIndex)
		LeftOP.Index = VAL_OFF;
	else if (LeftOP.Index==AutoIndex)
		LeftOP.Index= VAL_AUTO;
	else
		_ASSERT(0);
	
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,S_WORDS));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormIgnoreWhiteSpaces( const ISmntAnalyzeContext & Context)	
{
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.GetClass()==WORD_CLASS_ID);
	size_t OnIndex,OffIndex,AutoIndex;
	// можно было просто сравнить имена лексем, но такой подход безопаснее на случай, если такие зарезервированные слова будут удалены 
	OnIndex		=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"on");
	OffIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"off");
	AutoIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"auto");
	
	LeftOP.SetClass(DIRECT_NUMBER_CLASS_ID);

	if (LeftOP.Index==OnIndex) 
		LeftOP.Index = VAL_ON;
	else if (LeftOP.Index==OffIndex)
		LeftOP.Index = VAL_OFF;
	else if (LeftOP.Index==AutoIndex)
		LeftOP.Index= VAL_AUTO;
	else
		_ASSERT(0);
	
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,I_W_SPACES));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormIgnoreCase( const ISmntAnalyzeContext & Context)	
{
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.GetClass()==WORD_CLASS_ID);
	size_t OnIndex,OffIndex,AutoIndex;
	// можно было просто сравнить имена лексем, но такой подход безопаснее на случай, если такие зарезервированные слова будут удалены 
	OnIndex		=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"on");
	OffIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"off");
	AutoIndex	=Context.GetLexTable().GetIndex(WORD_CLASS_ID,"auto");
	
	LeftOP.SetClass(DIRECT_NUMBER_CLASS_ID);

	if (LeftOP.Index==OnIndex) 
		LeftOP.Index = VAL_ON;
	else if (LeftOP.Index==OffIndex)
		LeftOP.Index = VAL_OFF;
	else if (LeftOP.Index==AutoIndex)
		LeftOP.Index= VAL_AUTO;
	else
		_ASSERT(0);
	
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,I_CASE));
	return true;
}