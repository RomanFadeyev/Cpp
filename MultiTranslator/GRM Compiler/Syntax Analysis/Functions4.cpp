// Functions4.cpp: implementation of the CFunctions class. - ���������� ������������
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
	//������� ����� ���������� ���������
	RightOP=Context.GetLexeme();

	_ASSERT(RightOP.IsConstClass());
	_ASSERT(RightOP.pType);

	if (!RightOP.pType->IsString()) {
		errors::e_need_string_const(RightOP,0);
		return false;
	}

	//������ ����������� ��� �������
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.IsIDClass());
	size_t Index;
	if (!(m_LexRuleTable.RegisterRule(LeftOP.GetSzPureText(),Index)))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return false;
	}

	//��������� ��������� �� ������������
	try{
		boost::regex expression(RightOP.GetSzPureText());
	}
	catch (boost::bad_pattern & ) {
		errors::e_bad_regular_expression(LeftOP,0);
		return false;
	}
	

	//��������� ������� ��� �������
	PushIntoGrammsStack(LeftOP); // ��� CreateLex

	//.. � ���������
	PushIntoGrammsStack(RightOP); // ��� CreateLex

	_ASSERT(!m_bInSubRoutine);	
	_ASSERT(!m_bInFunction);	

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateLex( const ISmntAnalyzeContext & Context)	
{
	PopFromGrammsStack(RightOP);				// ����� ����� ���������
	PopFromGrammsStack(LeftOP);					// ����� ����� ���. �������� PreCreateLex
	
	pIGrmKrnl->AddRegExp(LeftOP.GetSzPureText(), RightOP.GetSzPureText());

	return true;
}
