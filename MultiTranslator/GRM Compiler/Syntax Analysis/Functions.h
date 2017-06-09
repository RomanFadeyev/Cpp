/********************************************************************
	created:	2003/06/10
	created:	10:6:2003   9:05
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\functions.h
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	functions
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_FUNCTIONS_H__A6169E01_1564_11D4_8F94_8ABEEB38DB92__INCLUDED_)
#define AFX_FUNCTIONS_H__A6169E01_1564_11D4_8F94_8ABEEB38DB92__INCLUDED_

#pragma once
#include <list>
#include "BaseStructs.h"
#include "FunctionsTable.h"
#include "RulesTable.h"
#include "LexRuleTable.h"
//#include "SntxTables.h"

class CFunctions  
{
private:
	//Нам нужен свой переходник, потому что иногда необходимо переподставлять 
	//лексемы в функиях
	class CContextMediator:public ISmntAnalyzeContext
	{
	private:
		const ISmntAnalyzeContext & m_Context;
		const structs::lexeme & m_Lexeme;	//Новая лексема
	public:
		CContextMediator(const ISmntAnalyzeContext & Context, const structs::lexeme & Lexeme):
			m_Context(Context),m_Lexeme(Lexeme){
		}
		override const structs::lexeme&		GetLexeme() const {return m_Lexeme;}

		override const ILexList&	GetLexList()		const {return m_Context.GetLexList();}
		override const ISntxList&	GetSntxList()		const {return m_Context.GetSntxList();}

		override const ILexTable&	GetLexTable()		const {return m_Context.GetLexTable();}
		override const ILangGenTable&GetLangGenTable()	const {return m_Context.GetLangGenTable();}
		override IImportTable&		GetImportTable()	const {return m_Context.GetImportTable();}
		override ISntxTable&		GetSntxTable()		const {return m_Context.GetSntxTable();}
		override ITypeTable&		GetTypeTable()		const {return m_Context.GetTypeTable();}
		override IPCodeList&		GetPCodeList()		const {return m_Context.GetPCodeList();}
	};

public:
	CFunctions();
	virtual ~CFunctions();

	static ret_t DeleteFromLexemeStack	( const ISmntAnalyzeContext & Context);
	static ret_t DeleteFromTypeStack		( const ISmntAnalyzeContext & Context);
	static ret_t CheckStacks				( const ISmntAnalyzeContext & Context);
	static ret_t PushToLexemeStack		( const ISmntAnalyzeContext & Context);
	static ret_t CopyLexemeStackHead		( const ISmntAnalyzeContext & Context);
	static ret_t PushToTypeStack			( const ISmntAnalyzeContext & Context);	
	static ret_t CreateCodeFragment		( const ISmntAnalyzeContext & Context);	
	static ret_t AppendCodeFragment		( const ISmntAnalyzeContext & Context);	
	static ret_t DestroyCodeFragment		( const ISmntAnalyzeContext & Context);	

	static ret_t PreCreateLex			( const ISmntAnalyzeContext & Context);	
	static ret_t CreateLex				( const ISmntAnalyzeContext & Context);	
	
	static ret_t PreCreateRule			( const ISmntAnalyzeContext & Context);	
	static ret_t CreateRule				( const ISmntAnalyzeContext & Context);	
	static ret_t CreateRuleSubRoutine	( const ISmntAnalyzeContext & Context);	
	
	static ret_t PreCreateVariant		( const ISmntAnalyzeContext & Context);	
	static ret_t CreateVariant			( const ISmntAnalyzeContext & Context);	
	static ret_t CreateVariantWithPreSub	( const ISmntAnalyzeContext & Context);	
	static ret_t CreatePreVarSubroutine	( const ISmntAnalyzeContext & Context);	
	static ret_t CreateVariantSubRoutine	( const ISmntAnalyzeContext & Context);	

	static ret_t SetOptionalTermFlag		( const ISmntAnalyzeContext & Context);	
	static ret_t SetChidOptionalTermFlag	( const ISmntAnalyzeContext & Context);	
	static ret_t ClearTermFlags			( const ISmntAnalyzeContext & Context);	
	

	static ret_t CreatePreTermSubroutine	( const ISmntAnalyzeContext & Context);	
	static ret_t PreCreateTerminal		( const ISmntAnalyzeContext & Context);	
	static ret_t CreateTerminal			( const ISmntAnalyzeContext & Context);	
	static ret_t CreateTerminalWithPreSub( const ISmntAnalyzeContext & Context);	

	static ret_t CreatePreUTermSubroutine( const ISmntAnalyzeContext & Context);	
	static ret_t PreCreateNonterminal		( const ISmntAnalyzeContext & Context);	
	static ret_t CreateNonterminal		( const ISmntAnalyzeContext & Context);	
	static ret_t CreateNonterminalWithPreSub( const ISmntAnalyzeContext & Context);	

	static ret_t FormENDSUBR				( const ISmntAnalyzeContext & Context);	

	static ret_t SetTermErrorCode		( const ISmntAnalyzeContext & Context);	
	static ret_t SetRuleErrorCode		( const ISmntAnalyzeContext & Context);	
	static ret_t SetRuleIgnoreWhiteSpaces( const ISmntAnalyzeContext & Context);	
	static ret_t SetRuleSolitaryWords	(const ISmntAnalyzeContext & Context);	

	static ret_t FormSolitaryWords		( const ISmntAnalyzeContext & Context);
	static ret_t FormIgnoreWhiteSpaces	( const ISmntAnalyzeContext & Context);	
	static ret_t FormIgnoreCase			( const ISmntAnalyzeContext & Context);	

	static ret_t ResetEnter			( const ISmntAnalyzeContext & Context);	
	static ret_t RestoreEnter		( const ISmntAnalyzeContext & Context);	
	static ret_t CreateFunc			( const ISmntAnalyzeContext & Context);	
	static ret_t SearchFunc			( const ISmntAnalyzeContext & Context);	
	static ret_t AddToFuncByVal		( const ISmntAnalyzeContext & Context);
	static ret_t AddToFuncByRef		( const ISmntAnalyzeContext & Context);
	static ret_t CreateStruct		( const ISmntAnalyzeContext & Context);	
	static ret_t AddToStruct			( const ISmntAnalyzeContext & Context);
	static ret_t AddToStructArray	( const ISmntAnalyzeContext & Context);
	static ret_t EnterToStruct		( const ISmntAnalyzeContext & Context);
	static ret_t ExitFromStruct		( const ISmntAnalyzeContext & Context);
	static ret_t IncParamCount		( const ISmntAnalyzeContext & Context);
	static ret_t IncLevel			( const ISmntAnalyzeContext & Context);
	static ret_t DecLevel			( const ISmntAnalyzeContext & Context);
	
	static ret_t CreateAssignOperation	( const ISmntAnalyzeContext & Context);

	static ret_t FormPARSE			( const ISmntAnalyzeContext & Context);	
	static ret_t FormGET_TEXT		( const ISmntAnalyzeContext & Context);	
	static ret_t FormGET_FILE_POS	( const ISmntAnalyzeContext & Context);	
	static ret_t FormINS_BEFORE		( const ISmntAnalyzeContext & Context);	
	static ret_t FormINS_AFTER 		( const ISmntAnalyzeContext & Context);	
	static ret_t FormIGNORE_BLOCK	( const ISmntAnalyzeContext & Context);	
	static ret_t FormREPLACE_BLOCK	( const ISmntAnalyzeContext & Context);	
	static ret_t FormSAVE_FILES		( const ISmntAnalyzeContext & Context);	

	static ret_t FormSHR			( const ISmntAnalyzeContext & Context);	
	static ret_t FormSHL			( const ISmntAnalyzeContext & Context);	
	static ret_t FormNOT	 		( const ISmntAnalyzeContext & Context);	
	static ret_t FormNEG	 		( const ISmntAnalyzeContext & Context);	
	static ret_t FormEQ	 			( const ISmntAnalyzeContext & Context);	
	static ret_t FormINEQ			( const ISmntAnalyzeContext & Context);	
	static ret_t FormLESS			( const ISmntAnalyzeContext & Context);	
	static ret_t FormMORE			( const ISmntAnalyzeContext & Context);	
	static ret_t FormLESS_EQ		( const ISmntAnalyzeContext & Context);	
	static ret_t FormMORE_EQ		( const ISmntAnalyzeContext & Context);	
	
	static ret_t FormL_AND			( const ISmntAnalyzeContext & Context);
	static ret_t FormL_OR 			( const ISmntAnalyzeContext & Context);
	static ret_t FormOR 			( const ISmntAnalyzeContext & Context);
	static ret_t FormAND 			( const ISmntAnalyzeContext & Context);
	static ret_t FormXOR  			( const ISmntAnalyzeContext & Context);

	static ret_t FormADD			( const ISmntAnalyzeContext & Context);
	static ret_t FormSUB			( const ISmntAnalyzeContext & Context);
	static ret_t FormMUL			( const ISmntAnalyzeContext & Context);
	static ret_t FormDIV			( const ISmntAnalyzeContext & Context);
	static ret_t FormINC_POST		( const ISmntAnalyzeContext & Context);
	static ret_t FormDEC_POST		( const ISmntAnalyzeContext & Context);
	static ret_t FormRMNDR			( const ISmntAnalyzeContext & Context);
	static ret_t FormSIZEOF			( const ISmntAnalyzeContext & Context);
	static ret_t FormSIZEA			( const ISmntAnalyzeContext & Context);
	static ret_t FormPOP			( const ISmntAnalyzeContext & Context);
	static ret_t FormSTORE			( const ISmntAnalyzeContext & Context);
	static ret_t FormUNSTORE		( const ISmntAnalyzeContext & Context);
	static ret_t FormRESTORE		( const ISmntAnalyzeContext & Context);
	static ret_t FormPUSH			( const ISmntAnalyzeContext & Context);
	static ret_t FormFOR 			( const ISmntAnalyzeContext & Context);
	static ret_t FormENDFOR			( const ISmntAnalyzeContext & Context);
	static ret_t FormIF  			( const ISmntAnalyzeContext & Context);
	static ret_t FormTHEN 			( const ISmntAnalyzeContext & Context);
	static ret_t FormTHEN_ELSE		( const ISmntAnalyzeContext & Context);
	static ret_t FormELSE			( const ISmntAnalyzeContext & Context);
	static ret_t FormDeclare		( const ISmntAnalyzeContext & Context);
	static ret_t FormDeclareArray	( const ISmntAnalyzeContext & Context);
	static ret_t FormDeclareConst	( const ISmntAnalyzeContext & Context);
	static ret_t FormASSIGN			( const ISmntAnalyzeContext & Context);
	static ret_t FormRET   			( const ISmntAnalyzeContext & Context);
	static ret_t FormEMPTY_RET		( const ISmntAnalyzeContext & Context);
	static ret_t FormRECALL			( const ISmntAnalyzeContext & Context);
	static ret_t FormENDF  			( const ISmntAnalyzeContext & Context);
	static ret_t FormCALL  			( const ISmntAnalyzeContext & Context);
	static ret_t FormRETURN			( const ISmntAnalyzeContext & Context);
	static ret_t FormPRINT 			( const ISmntAnalyzeContext & Context);
	static ret_t FormPreFORMAT 		( const ISmntAnalyzeContext & Context);
	static ret_t FormFORMAT 		( const ISmntAnalyzeContext & Context);
	static ret_t FormWRITE 			( const ISmntAnalyzeContext & Context);
	static ret_t FormSTORE_IDX		( const ISmntAnalyzeContext & Context);

	static Clear();
private:
	static bool	 LogicOperationsUtilite(const ISmntAnalyzeContext & Context);
	static bool	 GetSimpleType		(const structs::lexeme & Lexeme, OUT structs::object::type & aType);
	static bool	 SimpleTypeCast		(const structs::lexeme & Lexeme1, const structs::lexeme & Lexeme2,OUT structs::object::type & aType3);
	static bool	 SimpleTypeCast		(const structs::object::type & aType1,const structs::object::type & aType2, OUT structs::object::type & aType3);
	static bool	 CheckOnGoodTypeCast(const structs::lexeme & LeftOP, const structs::lexeme & RightOP);
	
	static bool  ClearLexemeStack	(					);
	static bool  PopFromLexemeStack	( structs::lexeme &  Lexeme	);

	static bool  PushIntoTypeStack	( const structs::type *	pType);	
	static bool  PopFromTypeStack	( const structs::type *&pType);

	static bool  PushToLexemeStack	( const structs::lexeme &  Lexeme);
	static bool  PushToLexemeStack	( const ISmntAnalyzeContext & Context, structs::object::type VarType);
	static bool  PushToEnterStack	( structs::type *   pType);	
	static bool  PopFromEnterStack	( structs::type * & pType);	
	static bool  PushIntoGrammsStack( structs::lexeme &  Lexeme	);
	static bool  PopFromGrammsStack	( structs::lexeme &  Lexeme	);
	static EAssignType  GetASSIGNMode();
	
	static size_t		m_nCurrentFuncProps;
	
	static string_smart	m_strCurrentFuncName;
	static string_smart	m_strCurrentTermName;
	static string_smart	m_strCurrentVariantName;		// должен быть объект, а не ссылка, т.к. объекта не существует
	static string_smart	m_strCurrentRuleName;
	static string_std	m_strCurRuleError;	
	static string_std	m_strCurTermError;	

	static string_smart	m_strAssignOperation;// for +=, *=, /= and etc....

	static EStdTerminalClass CurrentTermType;
	static BYTE			CurrentTermFlags;
	
	static E3State		CurrentRuleIgnoreWhiteSpaces;
	static E3State		CurrentRuleSolitaryWords;
	static size_t		VariantsInCurrentHub;
	static size_t		TermsInCurrentVariant;

	static stack_ex<structs::lexeme>				m_GrammsStack;
	static stack_ex<structs::lexeme>				m_LexemeStack;
	static stack_ex<const structs::type*>	m_TypeStack;
	static stack_ex<structs::type*>			m_EnterStack;
	static stack_ex<size_t>					m_IFStack;
	static list<string_smart>				m_StringList;
	static CFunctionsTable					m_FuncTable;
	static CRulesTable						m_RuleTable;
	static CLexRuleTable					m_LexRuleTable;

	static size_t		m_nParametersCount;
	static bool			m_bInStruct;		// указывает на то что происходит разбор сложного имени (ex: A.B.C )
	static structs::type*m_pType;
	static structs::lexeme		RightOP;
	static structs::lexeme		LeftOP;
	static SSntxID		LeftID;
	static SSntxID		RightID;
	static size_t		m_nLabelCounter;// нужен для формирования меток
	static bool			m_bInFunction;
	static bool			m_bInSubRoutine;
};

#endif // !defined(AFX_FUNCTIONS_H__A6169E01_1564_11D4_8F94_8ABEEB38DB92__INCLUDED_)
