// Functions3.cpp: implementation of the CFunctions class. - ”правление грамматиками
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Functions.h"
#include "EWQueue.h"

//------------------------------------------------------------------------
ret_t CFunctions::FormPARSE(const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(LeftOP);

	if (m_bInSubRoutine)
	{
		errors::e_bad_operation_in_subroutine(LeftOP,0);
		return FALSE;
	}

	_ASSERT(LeftOP.GetClass()==CONST_CLASS_ID);
	_ASSERT(LeftOP.pType);
	
	if (!LeftOP.pType->IsString())
	{
		errors::e_need_string_const(LeftOP,0);
		return false;
	}

	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,PARSE));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormGET_TEXT(const ISmntAnalyzeContext & Context)
{
	if (!m_bInSubRoutine)
	{
		errors::e_operation_out_of_subroutine(Context.GetLexeme(),0);
		return FALSE;
	}
	
	Context.GetPCodeList().Add(structs::code_cmd(GET_TEXT));
	PushToLexemeStack(Context,structs::object::vtSTRING);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormGET_FILE_POS(const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(GET_FILE_POS));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormINS_BEFORE(const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);
	PopFromLexemeStack(LeftOP);
	
	structs::object::type aType;
	if (!GetSimpleType(LeftOP, aType))
		return false;
	
	if ((aType!=structs::object::vtINT) && (aType!=structs::object::vtSTRING))
		errors::w_strange_use_of_type(LeftOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,RightOP,INS_BEFORE));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormINS_AFTER(const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);
	PopFromLexemeStack(LeftOP);

	structs::object::type aType;
	if (!GetSimpleType(LeftOP, aType))
		return false;
	
	if ((aType!=structs::object::vtINT) && (aType!=structs::object::vtSTRING))
		errors::w_strange_use_of_type(LeftOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,RightOP,INS_AFTER));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormIGNORE_BLOCK(const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);
	PopFromLexemeStack(LeftOP);
	
	structs::object::type aType;
	if (!GetSimpleType(LeftOP, aType))
		return false;
	if (!GetSimpleType(RightOP, aType))
		return false;

	Context.GetPCodeList().Add(structs::code_cmd(I_BLOCK));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormREPLACE_BLOCK(const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);
	PopFromLexemeStack(LeftOP);

	structs::object::type aType;
	if (!GetSimpleType(LeftOP, aType))
		return false;
	if (!GetSimpleType(RightOP, aType))
		return false;

	Context.GetPCodeList().Add(structs::code_cmd(REPLACE_BLOCK));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSAVE_FILES(const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(SAVE_FILES));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
