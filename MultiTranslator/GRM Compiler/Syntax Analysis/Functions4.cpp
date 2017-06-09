// Functions4.cpp: implementation of the CFunctions class. - Управление грамматиками
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <boost/regex.hpp>

#include "Functions.h"
#include "DLLSupport.h"
#include "EWQueue.h"

//------------------------------------------------------------------------
ret_t CFunctions::PreCreateLex	( const ISmntAnalyzeContext & Context)	
{
	//Сначала берем регулярное выражение
	RightOP=Context.GetLexeme();

	_ASSERT(RightOP.IsConstClass());
	_ASSERT(RightOP.pType);

	if (!RightOP.pType->IsString()) {
		errors::e_need_string_const(RightOP,0);
		return false;
	}

	//теперь вытаскиваем имя правила
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.IsIDClass());
	size_t Index;
	if (!(m_LexRuleTable.RegisterRule(LeftOP.GetSzPureText(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return false;
	}

	//проверяем выражение на допустимость
	try{
		boost::regex expression(RightOP.GetSzPureText());
	}
	catch (boost::bad_pattern & ) {
		errors::e_bad_regular_expression(LeftOP,0);
		return false;
	}
	

	//загружаем обратно имя правила
	PushIntoGrammsStack(LeftOP); // Для CreateLex

	//.. и выражение
	PushIntoGrammsStack(RightOP); // Для CreateLex

	_ASSERT(!m_bInSubRoutine);	
	_ASSERT(!m_bInFunction);	

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateLex( const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(RightOP);				// здесь лежит выражение
	PopFromGrammsStack(LeftOP);					// здесь лежит имя. Положено PreCreateLex
	
	pIGrmKrnl->AddRegExp(LeftOP.GetSzPureText(), RightOP.GetSzPureText());

	return true;
}
