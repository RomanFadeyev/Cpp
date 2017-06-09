/********************************************************************
	created:	2003/06/01
	created:	1:6:2003   11:07
	filename: 	$(Projects)\sources\grm compiler\initcompiler.cpp
	file path:	$(Projects)\sources\grm compiler
	file base:	initcompiler
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	������ ������������� ����������� ���. �������� ����������� 
				�������, ���������� � ��������� �����
*********************************************************************/

#include "StdAfx.h"
#include <AppUtils.h>
#include "InitCompiler.h"
#include "LexFunctions.h"
#include "Functions.h"
#include "EWQueue.h"


#define _LF(x)	&CLexVariant::lex_func(x)
typedef structs::sntx_error _EW;

bool init_compiler::InitStdVariables(CLexTable & LT, CSntxTable & ST,CTypesTable & TT, CLinker & Linker)
{
	Linker.Clear();

	// ��������� ���������� ����������
	structs::type	* pType;
	structs::lexeme Lex1,Lex2;	
	// ������� ����������������� ���������� �������� ������������, ��� ��������� ���������� � �������������� � ��� �� ������� ���������� ������
	//----------- ParamCount (ID=0)------------
	LT.AddID("ParamCount");
	Lex2=LT.FormLexeme(ID_CLASS_ID,"ParamCount");
	ASSERT_CHECK(TT.FindType("int",pType));
	Lex1.Clear();
	Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
	ASSERT_CHECK(ST.RegisterID(SSntxID(pType,Lex2,pType->GetTypeID()),Lex1.Index,Lex2.Index));
	Linker.AddInitCommand(structs::code_cmd(Lex1,Lex2,structs::lexeme(TYPE_CLASS_ID,pType->GetTypeID(),pType->GetTypeName()),NEW));
	Linker.AddInitCommand(structs::code_cmd(Lex1,Lex2,PRMCOUNT));
	//----------- ParamStr (ID=1)---------------
	LT.AddID("ParamStr");
	Lex2=LT.FormLexeme(ID_CLASS_ID,"ParamStr");
	ASSERT_CHECK(TT.FindType("string",pType));
	Lex1.Clear();
	Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
	ASSERT_CHECK(ST.RegisterID(SSntxID(pType,Lex2,255),Lex1.Index,Lex2.Index));
	Linker.AddInitCommand(structs::code_cmd(Lex1,Lex2,structs::lexeme(TYPE_CLASS_ID,pType->GetTypeID(),pType->GetTypeName()),structs::lexeme(DIRECT_NUMBER_CLASS_ID,255),NEWA));	
	Linker.AddInitCommand(structs::code_cmd(Lex1,Lex2,PRMSTR));
	//----------- SourceFileName (ID=2)---------------
	LT.AddID("SourceFileName");
	Lex2=LT.FormLexeme(ID_CLASS_ID,"SourceFileName");
	ASSERT_CHECK(TT.FindType("string",pType));
	Lex1.Clear();
	Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
	ASSERT_CHECK(ST.RegisterID(SSntxID(pType,Lex2,pType->GetTypeID()),Lex1.Index,Lex2.Index));
	Linker.AddInitCommand(structs::code_cmd(Lex1,Lex2,structs::lexeme(TYPE_CLASS_ID,pType->GetTypeID(),pType->GetTypeName()),NEW));
	//----------- DestFileName (ID=3)---------------
	LT.AddID("DestFileName");
	Lex2=LT.FormLexeme(ID_CLASS_ID,"DestFileName");
	ASSERT_CHECK(TT.FindType("string",pType));
	Lex1.Clear();
	Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
	ASSERT_CHECK(ST.RegisterID(SSntxID(pType,Lex2,pType->GetTypeID()),Lex1.Index,Lex2.Index));
	Linker.AddInitCommand(structs::code_cmd(Lex1,Lex2,structs::lexeme(TYPE_CLASS_ID,pType->GetTypeID(),pType->GetTypeName()),NEW));
	//----------- Delimiters(ID=4)---------------
	LT.AddID("Delimiters");
	Lex2=LT.FormLexeme(ID_CLASS_ID,"Delimiters");
	ASSERT_CHECK(TT.FindType("string",pType));
	Lex1.Clear();
	Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
	ASSERT_CHECK(ST.RegisterID(SSntxID(pType,Lex2,pType->GetTypeID()),Lex1.Index,Lex2.Index));
	Linker.AddInitCommand(structs::code_cmd(Lex1,Lex2,structs::lexeme(TYPE_CLASS_ID,pType->GetTypeID(),pType->GetTypeName()),NEW));
	
	//����������� �������� true � false - � 
	//������ LexTables.cpp, CLexTable::CLexTable(cstr szFileName) � CLexTable::AddId
	//� ������� ��� ����� ���������. 

	return true;

}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool init_compiler::InitTypesTable(CTypesTable & TT, const CLexTable & LT)
{
	//������ ��������� GetReservedWord ����� ������ ��� ����, ����� ���������
	//��� ������ ��� ����� ����������������� �����
	LT.GetReservedWord("void");		TT.CreateSimple("void",	 structs::object::vtNULL);
	LT.GetReservedWord("char");		TT.CreateSimple("char",	 structs::object::vtCHAR);	//char ������ ���� ����� bool
	LT.GetReservedWord("bool");		TT.CreateSimple("bool",	 structs::object::vtBOOL);
	LT.GetReservedWord("small");	TT.CreateSimple("small", structs::object::vtSMALL);	
	LT.GetReservedWord("short");	TT.CreateSimple("short", structs::object::vtSHORT);	
	LT.GetReservedWord("int");		TT.CreateSimple("int",	 structs::object::vtINT);	
	LT.GetReservedWord("double");	TT.CreateSimple("double",structs::object::vtDOUBLE);
	LT.GetReservedWord("string");	TT.CreateSimple("string",structs::object::vtSTRING);
	/* ��� ������ �������*/			TT.CreateSimple("variant",structs::object::vtVARIANT);
	return true;
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool init_compiler::InitLexTable(CLexTable & LT)
{
	///////////// Set LT	 //////////////////////
	LT.AddWords(
#include "DefineKeyWords.h"
					,
#include "DefineReservedFunc.h"
					,
                    NULL);
	LT.AddOperators(
					"=",		"+",		"+=",		"-",		"-=",		"*",		
					"*=",		"/",		"/=",		"%",		"%=",	
					"++",		"--",		"|",		"|=",		"^",		"^=",
					"&",		"&=",		"!",		"!=",		"==",		
					"<",		">",		"<=",		">=",		"&&",		"||",		
					",",		".",		"[",		"]",		"(",		")",		
					"{",		"}",		";",		"<<",		">>",		"<<=",		
					">>=",		"~",	":",
                    NULL);
	return true;
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool init_compiler::InitLexAnalyzer(CLexAnalyzer &	LA)
{
	////////////// LA init /////////////////
	LA.AddAutomat("StringConsts");
	LA.AddAutomat("CharConsts");
	LA.AddAutomat("Operators");
	LA.AddAutomat("DigitConsts");
	LA.AddAutomat("Identifiers");	// include Words
	LA.AddAutomat("Spaces"); // Spaces	 ������	������ ������ ���������, ������	- �������� ��� �������������

	//����� ������ �� �������� ����� ������ ����� ����, ��� ��� ��� �������
	CLexAutomat	& StringConst	= LA.GetAutomat(0);
	CLexAutomat	& CharConst		= LA.GetAutomat(1);
	CLexAutomat	& Operator		= LA.GetAutomat(2);
	CLexAutomat	& DigitConst	= LA.GetAutomat(3);
	CLexAutomat	& Identifier	= LA.GetAutomat(4);
	CLexAutomat	& Spaces		= LA.GetAutomat(5);

	//////////// End LA	init ///////////////
	#define	_L_NOEW	CLexVariant::err_no_container()
	#define	_L_EW(T)CLexVariant::err_container<T>()
// ������� ������� ���������
// ������� CLexHub ������� �� �������������	���������� CLexVariant
// ������ CLexVariant: 
//		1 -	������ ��� ����� ��������, 
//		2 -	����� �������� (����� ���� ������ ��� �������� � �������� ����)
//		3 -	����� ������ ��� ���������������, 
//		4 -	������ ����������� �������, 

// Operator	/////////////////////////////////
	//0
	typedef structs::litera litera;

	CLexHub & OH1 = Operator.AddHub();
	OH1.AddVariant(CLexVariant('=',			2,			_L_NOEW			));	// =,==
	OH1.AddVariant(CLexVariant('(',			1,			_L_NOEW			));	
	OH1.AddVariant(CLexVariant(')',			1,			_L_NOEW			));	
	OH1.AddVariant(CLexVariant('{',			1,			_L_NOEW			));	
	OH1.AddVariant(CLexVariant('}',			1,			_L_NOEW			));	
	OH1.AddVariant(CLexVariant('[',			1,			_L_NOEW			));	// [
	OH1.AddVariant(CLexVariant(']',			1,			_L_NOEW			));	
	OH1.AddVariant(CLexVariant(';',			1,			_L_NOEW			));	
	OH1.AddVariant(CLexVariant(',',			1,			_L_NOEW			));	// ,
	OH1.AddVariant(CLexVariant('+',			3,			_L_NOEW			));	// +, ++
	OH1.AddVariant(CLexVariant('-',			4,			_L_NOEW			));	// -, --
	OH1.AddVariant(CLexVariant('*',			2,			_L_NOEW			));	// *
	OH1.AddVariant(CLexVariant('/',			2,			_L_NOEW			));	// /
	OH1.AddVariant(CLexVariant('%',			2,			_L_NOEW			));	// %
	OH1.AddVariant(CLexVariant('!',			2,			_L_NOEW			));	// !,!=
	OH1.AddVariant(CLexVariant('&',			5,			_L_NOEW			));	// &, &&, &=
	OH1.AddVariant(CLexVariant('|',			6,			_L_NOEW			));	// |, ||, |=
	OH1.AddVariant(CLexVariant('^',			2,			_L_NOEW			));	// ^, ^=
	OH1.AddVariant(CLexVariant('<',			7,			_L_NOEW			));	// <,<=
	OH1.AddVariant(CLexVariant('>',			8,			_L_NOEW			));	// >,>=
	OH1.AddVariant(CLexVariant('~',			1,			_L_NOEW			));	// ,
	OH1.AddVariant(CLexVariant(':',			1,			_L_NOEW			));	// :
	OH1.AddVariant(CLexVariant('.',			1,			_L_NOEW			));	// not implemented

	//1	== (*,/,%, ^, )												
	Operator.AddHub(CLexHub(											
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))
	));																
																			
	//2	!,=,<,>																
	Operator.AddHub(CLexHub(													
		CLexVariant('=',			1,			_L_NOEW			),	// ==
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))	// 
	));																
	//3	+																	
	Operator.AddHub(CLexHub(													
		CLexVariant('+',			1,			_L_NOEW			),	// ++
		CLexVariant('=',			1,			_L_NOEW			),	// +=
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))
	));																
	//4	-																	
	Operator.AddHub(CLexHub(													
		CLexVariant('-',			1,			_L_NOEW			),	// --
		CLexVariant('=',			1,			_L_NOEW			),	// -=
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))
	));														
																	
	//5	&															
	Operator.AddHub(CLexHub(	
		CLexVariant('&',			1,			_L_NOEW			),	// &&
		CLexVariant('=',			1,			_L_NOEW			),	// &=
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))
	));														
	//6	|															
	Operator.AddHub(CLexHub(											
		CLexVariant('|',			1,			_L_NOEW			),	// ||
		CLexVariant('=',			1,			_L_NOEW			),	// |=
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))
	));														
	//7	<														
	Operator.AddHub(CLexHub(											
		CLexVariant('<',			2,			_L_NOEW			),	// <<, <<=
		CLexVariant('=',			1,			_L_NOEW			),	// <=
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))	
	));														
	//8	>														
	Operator.AddHub(CLexHub(											
		CLexVariant('>',			2,			_L_NOEW			),	// >>, >>=
		CLexVariant('=',			1,			_L_NOEW			),	// >=
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddOperator))
	));														
																	
// Identifier /////////////////////////////////						
	//0																
	Identifier.AddHub(CLexHub(											
		CLexVariant(litera::TAG_LETTER,1,			_L_NOEW			),	
		CLexVariant('_',			1,			_L_NOEW			)
	));														
	//1																
	Identifier.AddHub(CLexHub(											
		CLexVariant('_',			1,			_L_NOEW			),
		CLexVariant(litera::TAG_DL,	1,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)),
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddID))
	));

// CharConst /////////////////////////////////	
	//0
	CharConst.AddHub(CLexHub(	
		CLexVariant('\'',			1,			_L_NOEW,			_LF(CLexFunctions::SetChar),
																	_LF(CLexFunctions::DisableAdding))
	));	
	//1
	CharConst.AddHub(CLexHub(	
		CLexVariant('\\',			2,			_L_NOEW			),	
		CLexVariant('\'',			4,			_L_NOEW,			_LF(CLexFunctions::AddConst),
																	_LF(CLexFunctions::DisableAdding)),	
		CLexVariant('\n',			EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant(litera::TAG_END,	EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),
		CLexVariant(litera::TAG_ANY,	3,			_L_NOEW			)
	));	
	//2
	CharConst.AddHub(CLexHub(	
		CLexVariant('\n',			EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant(litera::TAG_END,	EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant(litera::TAG_ANY,	3,			_L_NOEW,			_LF(CLexFunctions::TryReplaceEscSeq))
	));	
	//3
	CharConst.AddHub(CLexHub(	
		CLexVariant('\'',			4,			_L_NOEW,			_LF(CLexFunctions::AddConst),
																	_LF(CLexFunctions::DisableAdding)),	
		CLexVariant('\n',			EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant(litera::TAG_END,	EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),
		CLexVariant(litera::TAG_ANY,	EXIT_FALSE,	_L_EW(errors::e_too_many_chars))
	));	
	//4
	CharConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW				)
	));	

// StringConst /////////////////////////////////	
 //	� ���������	�����������	������ ������������� �������, �� ����������	����� �������� ��������� ���������
	//0
	StringConst.AddHub(CLexHub(	
		CLexVariant('"',			1,			_L_NOEW,			_LF(CLexFunctions::SetString))
	));	
	//1
	StringConst.AddHub(CLexHub(	
		CLexVariant('\\',			2,			_L_NOEW			),	
		CLexVariant('"',			4,			_L_NOEW,			_LF(CLexFunctions::AddConst),
																	_LF(CLexFunctions::DisableAdding)),
		CLexVariant('\n',			EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant(litera::TAG_END,	EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant(litera::TAG_DLAS,	1,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)), //	�����������. ����� � ��	���������
		CLexVariant(litera::TAG_ANY,	1,			_L_NOEW			)
	));	
	//2
	StringConst.AddHub(CLexHub(	
		CLexVariant('\n',			1,			_L_NOEW,			_LF(CLexFunctions::DisableAdding),
																	_LF(CLexFunctions::DeleteLast)),
		CLexVariant(litera::TAG_END,	EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant(litera::TAG_SPACE,3,			_L_NOEW,			_LF(CLexFunctions::TryReplaceEscSeq)),
		CLexVariant(litera::TAG_ANY,	1,			_L_NOEW,			_LF(CLexFunctions::TryReplaceEscSeq))
	));	
	//3
	StringConst.AddHub(CLexHub(	
		CLexVariant('\n',			1,			_L_NOEW,			_LF(CLexFunctions::DisableAdding),
																	_LF(CLexFunctions::DeleteLast)),
		CLexVariant('"',			4,			_L_NOEW,			_LF(CLexFunctions::AddConst),
																	_LF(CLexFunctions::DisableAdding)),	

		CLexVariant(litera::TAG_END,	EXIT_FALSE,	_L_EW(errors::e_unterminated_string)),	
		CLexVariant('\\',			2,			_L_NOEW			),	
		CLexVariant(litera::TAG_SPACE,3,			_L_NOEW,			_LF(CLexFunctions::TryReplaceEscSeq)),
		CLexVariant(litera::TAG_ANY,	1,			_L_NOEW,			_LF(CLexFunctions::TryReplaceEscSeq))
	));	
	//4
	StringConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW			)
	));	

// DigitConst /////////////////////////////////
	//0
	DigitConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_DIGIT,2,			_L_NOEW,			_LF(CLexFunctions::SetInteger)),	
		CLexVariant('-',	1,					_L_NOEW			)
	));	
	//1
	DigitConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_DIGIT,2,			_L_NOEW,			_LF(CLexFunctions::SetInteger))
	));	
	//2
	CLexHub & DCH = DigitConst.AddHub();
	DCH.AddVariant(CLexVariant(litera::TAG_DIGIT,2,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)));
	DCH.AddVariant(CLexVariant('.',			3,				_L_NOEW,			_LF(CLexFunctions::SetDouble)));
	DCH.AddVariant(CLexVariant('E',			4,				_L_NOEW,			_LF(CLexFunctions::SetDouble)));			
	DCH.AddVariant(CLexVariant('e',			4,				_L_NOEW,			_LF(CLexFunctions::SetDouble)));
	DCH.AddVariant(CLexVariant('.',			EXIT_FALSE,		_L_EW(errors::e_bad_num_format)));
	DCH.AddVariant(CLexVariant(litera::TAG_SYMBOL,EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddConst)));
	DCH.AddVariant(CLexVariant(litera::TAG_ANY,	EXIT_FALSE,	_L_EW(errors::e_bad_num_format)));

	//3
	DigitConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_DIGIT,3,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)),
		CLexVariant('E',			4,			_L_NOEW,			_LF(CLexFunctions::SetDouble)),
		CLexVariant('e',			4,			_L_NOEW,			_LF(CLexFunctions::SetDouble)),
		CLexVariant('.',			EXIT_FALSE,	_L_EW(errors::e_bad_num_format)),	
		CLexVariant(litera::TAG_SYMBOL,EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddConst)),	
		CLexVariant(litera::TAG_ANY,	EXIT_FALSE,	_L_EW(errors::e_bad_num_format))
));
	//4
	DigitConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_DIGIT,5,			_L_NOEW				),
		CLexVariant('+',			6,			_L_NOEW				),
		CLexVariant('-',			6,			_L_NOEW				),
		CLexVariant(litera::TAG_ANY,	EXIT_FALSE,	_L_EW(errors::e_bad_num_format))
));
	//5
	DigitConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_DIGIT,5,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)),
		CLexVariant('.',			EXIT_FALSE,	_L_EW(errors::e_bad_num_format)),	
		CLexVariant(litera::TAG_SYMBOL,EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddConst)),	
		CLexVariant(litera::TAG_ANY,	EXIT_FALSE,	_L_EW(errors::e_bad_num_format))
));
	//6
	DigitConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_DIGIT,7,			_L_NOEW				),
		CLexVariant(litera::TAG_ANY,	EXIT_FALSE,	_L_EW(errors::e_bad_num_format))
));
	//7
	DigitConst.AddHub(CLexHub(	
		CLexVariant(litera::TAG_DIGIT,7,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)),
		CLexVariant('.',			EXIT_FALSE,	_L_EW(errors::e_bad_num_format)),	
		CLexVariant(litera::TAG_SYMBOL,EXIT_TRUE,	_L_NOEW,			_LF(CLexFunctions::AddConst)),	
		CLexVariant(litera::TAG_ANY,	EXIT_FALSE,	_L_EW(errors::e_bad_num_format))
));

// Spaces /////////////////////////////////////
	//0
	Spaces.AddHub(CLexHub(
		CLexVariant(litera::TAG_SPACE,	1,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)),
		CLexVariant(litera::TAG_END,	EXIT_TRUE,	_L_NOEW	)			 //	�� ������ ������
	));
	Spaces.AddHub(CLexHub(
		CLexVariant(litera::TAG_SPACE,	1,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)),
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW				)
	));
	
	// ��������	�������� ��	�������	�������	ANOTHER_HUB	- ��� ������ ����� ��� ������ ANOTHER_HUB (�� Delimiter),
	// ��� � � ����	������ ANOTHER_HUB c ������	(��	Spaces.). 
	// ������ ������� �������� ����������������, ��� ��� ��	�������	� �� ������������ ���������������� (������������ ��	������)
	
	return true;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool init_compiler::InitSntxAnalyzer(CSntxAnalyzer & SA,	const CLexTable & LT)
{
	#define	GE		structs::grm_term
	#define	FL		LT.FormLexeme
	#define	FL_		LT.FormLexeme
	#define	LG		structs::lexeme_group

	#define R_OPTIONAL			structs::grm_term::flagOptional
	#define R_CHILD_OPTIONAL	structs::grm_term::flagChildOptional

	// SA /////////////////////////////////
	////---------------	0
	SA.AddRule(CSntxRule("���������",							NULL,
		CSntxVariant(	GE("���������� ����������"),	
						_EW(NULL),	
						CFunctions::CheckStacks),// ����� ������ ���� �������
	NULL));
	////--------------- 1 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("���������� ����������",				NULL,
		CSntxVariant(	GE("������� ����������"),
/*OPTIONAL*/			GE("���������� ����������",NULL,R_OPTIONAL),
						_EW(NULL)), 	
	NULL));
	
	////--------------- 2
	SA.AddRule(CSntxRule("������� ����������",					NULL,
		CSntxVariant(	GE("����������� ������������ �������"),
						_EW(NULL)), 
		CSntxVariant(	GE("����������� �������"),
						_EW(NULL)), 
		CSntxVariant(	GE("����������"),
						_EW(NULL)),													
		CSntxVariant(	GE("����������� �������"),									
						_EW(NULL)),
	NULL));

	////--------------- 3
	SA.AddRule(CSntxRule("����������� ������������ �������", E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("lex"),E_MISSING_RULE_WORD,NULL),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL ("=")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateLex),
						GE(FL(";"),E_MISSING_SEMICOLON),
						_EW(NULL),			
						CFunctions::CreateLex), 
	NULL));

	////--------------- 3
	SA.AddRule(CSntxRule("����������� �������", E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("rule"),E_MISSING_RULE_WORD,NULL),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateRule),
						GE(FL(">")),
/*OPTIONAL*/			GE(FL(":"),NULL,R_OPTIONAL),
/**/					GE("������ ���������� �������",NULL,R_CHILD_OPTIONAL),
						GE(FL("{")),
						GE("���� �������"),
						GE(FL("}")),												
						_EW(NULL),			
						CFunctions::CreateRule), 
	NULL));

	////--------------- 4 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ���������� �������", E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("�������� �������"),
/*OPTIONAL*/			GE(FL (","),NULL,R_OPTIONAL),
/**/					GE("������ ���������� �������",NULL,R_CHILD_OPTIONAL),			
						_EW(NULL)),
	NULL));
	
	////--------------- 5
	SA.AddRule(CSntxRule("�������� �������", E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("error"),NULL),
						GE(FL("=")),
						GE(CONST_CLASS_ID),			
						_EW(NULL),			
						CFunctions::SetRuleErrorCode), 
		CSntxVariant(	GE(FL("SolitaryWords")),
						GE(FL("=")),
						GE("On,Off,Auto"),				
						_EW(NULL),			
						CFunctions::SetRuleSolitaryWords), 
		CSntxVariant(	GE(FL("IgnoreWhiteSpaces")),
						GE(FL("=")),
						GE("On,Off,Auto"),					
						_EW(NULL),			
						CFunctions::SetRuleIgnoreWhiteSpaces), 
	NULL));	
	
	////--------------- 6
	SA.AddRule(CSntxRule("On,Off,Auto",				E_INCORRECT_ON_OFF_A_EXPRESSION,
		CSntxVariant(	GE(FL("on"), CFunctions::PushToLexemeStack),					
						_EW(NULL)),													
		CSntxVariant(	GE(FL("off"),CFunctions::PushToLexemeStack),					
						_EW(NULL)),
		CSntxVariant(	GE(FL("auto"), CFunctions::PushToLexemeStack),					
						_EW(NULL)),
	NULL));

	////--------------- 7 
	SA.AddRule(CSntxRule("���� �������",				E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("������ ��������� � ���������������",CFunctions::CreateRuleSubRoutine),
/*OPTIONAL*/ 	//		GE("������ ����������",NULL,R_OPTIONAL),					
/*OPTIONAL*/			GE("������ ����-�����",NULL,R_OPTIONAL),					
						GE(EMPTY_ELEMENT, CFunctions::FormRET),
						_EW(NULL),					
						CFunctions::FormENDSUBR), 	
	NULL));
	
	////--------------- 8 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ��������� � ���������������",			E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("����������� �������� � ���������������"),
/*OPTIONAL*/			GE("������ ��������� � ���������������",NULL,R_OPTIONAL),						
						_EW(NULL)), 
	NULL));
	
	////--------------- 9
	SA.AddRule(CSntxRule("����������� �������� � ���������������",		E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("variant"),E_MISSING_VARIANT_WORD, CFunctions::PreCreateVariant),
						GE(FL("{"),E_SYNTAX_ERROR, NULL),
						GE("���� ��������"),
						GE(FL("}")),													
						_EW(NULL),						
						CFunctions::CreateVariant), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreVarSubroutine),
						GE(FL(":")),
//						GE("������ ����������",					CFunctions::FormRET),
						GE("������ ����-�����",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),
						GE(FL("variant"),E_MISSING_VARIANT_WORD,CFunctions::PreCreateVariant),
						GE(FL("{")),
						GE("���� ��������"),
						GE(FL("}")),													
						_EW(NULL),						
						CFunctions::CreateVariantWithPreSub), 
	NULL));

	////--------------- 10 
	SA.AddRule(CSntxRule("���� ��������",				E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("������ ������ � ���������������",CFunctions::CreateVariantSubRoutine),
						GE("������ ����-�����"),										
//						GE("������ ����������"),										
						GE(EMPTY_ELEMENT,		CFunctions::FormRET),
						_EW(NULL),			
						CFunctions::FormENDSUBR), 	
		CSntxVariant(	GE("������ ������ � ���������������",CFunctions::CreateVariantSubRoutine),
						GE(EMPTY_ELEMENT,		CFunctions::FormRET),
						_EW(NULL),				
						CFunctions::FormENDSUBR), 	
	NULL));

	////--------------- 11 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ������ � ���������������",	E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("����������� ����� � ���������������"),
/*OPTIONAL*/			GE("������ ������ � ���������������",NULL,R_OPTIONAL),
						_EW(NULL)), 
	NULL));
		
	////--------------- 12
	SA.AddRule(CSntxRule("����������� ����� � ���������������",E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("����������� ���������"),									
						_EW(NULL)), 
		CSntxVariant(	GE("����������� �����������"),									
						_EW(NULL)), 
	NULL));
	
	////--------------- 13
	int a=0;
	SA.AddRule(CSntxRule("����������� ���������",		E_DECLARATION_INCORRECTLY,
		// �� ������� R_FORBID_BACK - ��� ����� ���� ���� ��������, ���� ����������
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE("��� ���������"), // � ��� ��  CreateSubRoutine
/*OPTIONAL*/			GE(FL(":"),		NULL,	R_OPTIONAL),
/**/					GE(FL("error"),	NULL,									R_CHILD_OPTIONAL),
/**/					GE(FL("="),		NULL,									R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,		R_CHILD_OPTIONAL),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),					
						CFunctions::CreateTerminal), 
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("["),CFunctions::SetOptionalTermFlag),
						GE("��� ���������"), // � ��� ��  CreateSubRoutine
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
/*OPTIONAL*/			GE(FL(":"),		NULL,	R_OPTIONAL),
/**/					GE(FL("error"),	NULL,									R_CHILD_OPTIONAL),
/**/					GE(FL("="),		NULL,									R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,		R_CHILD_OPTIONAL),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),					
						CFunctions::CreateTerminal), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreTermSubroutine),
						GE(FL(":")),
//						GE("������ ����������",					CFunctions::FormRET),
						GE("������ ����-�����",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE("��� ���������"), // � ��� ��  CreateSubRoutine
/*OPTIONAL*/			GE(FL(":"),NULL,R_OPTIONAL),
/**/					GE(FL("error"),NULL,	R_CHILD_OPTIONAL),
/**/					GE(FL("="),NULL,		R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,R_CHILD_OPTIONAL),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),				
						CFunctions::CreateTerminalWithPreSub), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreTermSubroutine),
						GE(FL(":")),
//						GE("������ ����������",					CFunctions::FormRET),
						GE("������ ����-�����",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("[")),
						GE("��� ���������"), // � ��� ��  CreateSubRoutine
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
/*OPTIONAL*/			GE(FL(":"),NULL,R_OPTIONAL),
/**/					GE(FL("error"),NULL,	R_CHILD_OPTIONAL),
/**/					GE(FL("="),NULL,		R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,R_CHILD_OPTIONAL),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),		
						CFunctions::CreateTerminalWithPreSub), 
	NULL));

	////--------------- 14
	SA.AddRule(CSntxRule("��� ���������",		E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("STD_ID"),CFunctions::PreCreateTerminal),		// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_LINE"),CFunctions::PreCreateTerminal),	// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_INT_CONST"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_NUM_CONST"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_SQMSTR_CONST"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_DQMSTR_CONST"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_STR_CONST"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_COMMON_CONST"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_ALPHA"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_NUM"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_ALNUM"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_CNTRL"),CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(CONST_CLASS_ID,CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PreCreateTerminal),// ������ ��� � ���� ������, 
						_EW(NULL)),
	NULL));

	////--------------- 15 
	SA.AddRule(CSntxRule("����������� �����������",		E_DECLARATION_INCORRECTLY,
	// R_FORBID_BACK ����� ���������, �� ���������� ������, ��� � ����������� ����� ������ ������ ������ "����������� ���������"
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// ������ ��� � ���� ������, 
																			// � ��� ��  CreateSubRoutine
						GE(FL(">")),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminal), 		
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("["),CFunctions::SetOptionalTermFlag),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// ������ ��� � ���� ������, 
																			// � ��� ��  CreateSubRoutine
						GE(FL(">")),
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminal),
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreUTermSubroutine),
						GE(FL(":")),
//						GE("������ ����������",					CFunctions::FormRET),
						GE("������ ����-�����",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),							
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// ������ ��� � ���� ������, 
																				// � ��� ��  CreateSubRoutine
						GE(FL(">")),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminalWithPreSub), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreUTermSubroutine),
						GE(FL(":")),
//						GE("������ ����������",					CFunctions::FormRET),
						GE("������ ����-�����",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),							
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("["),CFunctions::SetOptionalTermFlag),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// ������ ��� � ���� ������, 
																				// � ��� ��  CreateSubRoutine
						GE(FL(">")),
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
						GE("���� �������� ��� symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminalWithPreSub), 
	NULL));

	////--------------- ??
	SA.AddRule(CSntxRule("���� �������� ��� symbol",	E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("��������� ����������"),										
						GE(EMPTY_ELEMENT,	CFunctions::FormRET),
						GE(EMPTY_ELEMENT,	CFunctions::FormENDSUBR),
/*OPTIONAL*/			GE(FL(";"),NULL,R_OPTIONAL),	//������ �������, ������ ������ ���� ������ �� ��������
						_EW(NULL)),
		CSntxVariant(	GE(FL(";"),E_MISSING_SEMICOLON),
						GE(EMPTY_ELEMENT,	CFunctions::FormRET),
						GE(EMPTY_ELEMENT,	CFunctions::FormENDSUBR),
						_EW(NULL)), 
	NULL));

	////--------------- 16
	SA.AddRule(CSntxRule("����������� �������",				E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("extern")),
						GE("������������ ����"),
						GE("���������� �������",CFunctions::DestroyCodeFragment),
						GE(FL("library"),E_MISSING_LIBRARY_WORD),
						GE(CONST_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL(","),E_MISSING_COMA),								
						GE(CONST_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL(";"),E_MISSING_SEMICOLON),								
						GE(EMPTY_ELEMENT,	CFunctions::FormRECALL),
						GE(EMPTY_ELEMENT,	CFunctions::FormRET),
						GE(EMPTY_ELEMENT,	CFunctions::DecLevel),
						_EW(NULL),
						CFunctions::FormENDF),
		CSntxVariant(	GE("������������ ����"),
						GE("���������� �������",CFunctions::AppendCodeFragment),
						GE("��������� ����������"),										
						GE(EMPTY_ELEMENT,	CFunctions::FormEMPTY_RET),
						GE(EMPTY_ELEMENT,	CFunctions::DecLevel),
						_EW(NULL),
						CFunctions::FormENDF), 
	NULL));
	////--------------- -17 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ����������",					NULL,
		CSntxVariant(	GE("����������"),
/*OPTIONAL*/			GE("������ ����������",NULL,R_OPTIONAL),
						_EW(NULL)),
	NULL));
	////--------------- 18
	SA.AddRule(CSntxRule("����������",							NULL,
		CSntxVariant(	GE("Const ������������"),
						GE(FL(";"),E_MISSING_SEMICOLON,CFunctions::FormRESTORE),
						_EW(NULL)), 
		CSntxVariant(	GE("������������ ����"),
						GE("������ ���� ������������"),
						GE(FL(";"),E_MISSING_SEMICOLON,CFunctions::FormRESTORE),
						_EW(NULL),
						CFunctions::DeleteFromTypeStack), //��� ���������� �������������� ����
		CSntxVariant(	GE("������ ������������"),
						GE(FL(";"),E_MISSING_SEMICOLON,CFunctions::FormRESTORE), 
						_EW(NULL)), 
	NULL));
	////--------------- 19
	SA.AddRule(CSntxRule("Const ������������",			NULL,
		CSntxVariant(	GE(FL("const")),
						GE("������ const ����������"),
						_EW(NULL)), 
	NULL));
	////--------------- 20 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ const ����������",			NULL,
		CSntxVariant(	GE("Const ����������"),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("������ const ����������",NULL,R_CHILD_OPTIONAL),			
						_EW(NULL)), 
	NULL));
	////--------------- 21
	SA.AddRule(CSntxRule("Const ����������",					NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL ("="),NULL,CFunctions::CreateAssignOperation),
						GE("�������������"), 
						_EW(NULL),
						CFunctions::FormDeclareConst), 
	NULL));	
	////--------------- 22
	SA.AddRule(CSntxRule("������������ ����",					E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(LG(	"void",
								"bool",
								"int",
								"char",
								"short",
								"double",
								"string",
								NULL)),
						_EW(NULL),	
						CFunctions::PushToTypeStack), 

		CSntxVariant(	GE(ID_CLASS_ID),
						_EW(NULL),
						CFunctions::PushToTypeStack),
	NULL));
	
	////--------------- 23
	SA.AddRule(CSntxRule("������ ������������",				NULL,
		CSntxVariant(	GE(FL ("struct"),NULL),
						GE(ID_CLASS_ID,CFunctions::CreateStruct),
						GE(FL("{")),
						GE("������ ������ ����������"),
						GE(FL("}")),
						_EW(NULL)), 
	NULL));
	////--------------- 24 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ������ ����������",			NULL,
		CSntxVariant(	GE("������ ����������"),
/*OPTIONAL*/			GE("������ ������ ����������",NULL,R_OPTIONAL),
						_EW(NULL)), 
	NULL));
	////--------------- 25 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ���� ������������",			NULL,
		CSntxVariant(	GE("���� ����������"),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("������ ���� ������������",NULL,R_CHILD_OPTIONAL), 
						_EW(NULL)), 
	NULL));
	////--------------- 26
	SA.AddRule(CSntxRule("���� ����������",					NULL,
		CSntxVariant(	GE("���������� ����������"),
						GE(FL ("="),NULL,CFunctions::CreateAssignOperation),
						GE("�������������"), 
						_EW(NULL),							
						CFunctions::FormASSIGN), 
		CSntxVariant(	GE("���������� ����������"),									
						_EW(NULL)), 
	NULL));	
	////--------------- 27
	SA.AddRule(CSntxRule("������ ����������",				NULL,
		CSntxVariant(	GE("������������ ����"),
						GE("������ ������ ������������"), 
						GE(FL (";"),CFunctions::FormRESTORE),							
						_EW(NULL),
						CFunctions::DeleteFromTypeStack), //��� ���������� �������������� ����													
	NULL));	
	////--------------- 28 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ������ ������������",			NULL,
		CSntxVariant(	GE("������ ����������"),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("������ ������ ������������",NULL,R_CHILD_OPTIONAL), 
						_EW(NULL)), 
	NULL));
	////--------------- 29
	SA.AddRule(CSntxRule("������ ����������",					NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL("[")),												
						GE("��������� ������������"),												
						GE(FL("]")),							
						_EW(NULL),								
						CFunctions::AddToStructArray),// FormSTORE ����������� ��� ��
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						_EW(NULL),								
						CFunctions::AddToStruct),
	NULL));	

	////--------------- 30
	SA.AddRule(CSntxRule("���������� �������",					NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::CreateFunc),							
						GE(EMPTY_ELEMENT,CFunctions::IncLevel),
						GE(FL ("("),CFunctions::CreateCodeFragment), 
						GE(FL (")")),													
/*OPTIONAL*/			GE(FL ("["),NULL,R_OPTIONAL),													
/**/					GE(FL ("]"),NULL,R_CHILD_OPTIONAL),								
						_EW(NULL)), 
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::CreateFunc),
						GE(EMPTY_ELEMENT,CFunctions::IncLevel),
						GE(FL ("("),CFunctions::CreateCodeFragment), 
						GE("������ ����������"),
						GE(FL (")")),													
/*OPTIONAL*/			GE(FL ("["),NULL,R_OPTIONAL),													
/**/					GE(FL ("]"),NULL,R_CHILD_OPTIONAL),								
						_EW(NULL)), 
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::CreateFunc),							
						GE(EMPTY_ELEMENT,CFunctions::IncLevel),
						GE(FL ("("),CFunctions::CreateCodeFragment), 
						GE("������ ����������"),
						GE(FL (")")),
						_EW(NULL)), 
	NULL));	
	////--------------- 31 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ����������",			NULL,
		CSntxVariant(	GE("��������"),	
/*OPTIONAL*/			GE(FL (","),NULL,R_OPTIONAL),
/**/					GE("������ ����������",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)), 
	NULL));
	////--------------- 32
	SA.AddRule(CSntxRule("��������",					NULL,
		CSntxVariant(	GE("������������ ����"),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::DeleteFromLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::FormDeclare),//Form Declare �� ��������� ��������� TypeStack; FormDeclare �������� FormSTORE
						_EW(NULL),									
						CFunctions::AddToFuncByVal),	 //AddToFunc ������� ��� �� ����� ����� (DeleteFromStack) � ��������� POP
		CSntxVariant(	GE("������������ ����"),
						GE(FL("&")),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::DeleteFromLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::FormDeclare),//Form Declare �� ��������� ��������� TypeStack; FormDeclare �������� FormSTORE
						_EW(NULL),									
						CFunctions::AddToFuncByRef),	 //AddToFunc ������� ��� �� ����� ����� (DeleteFromStack) � ��������� POP
	NULL));

	////--------------- 33
	SA.AddRule(CSntxRule("���������� ����������",				NULL,
	// ������� FormDeclare... �������� ��� �����, �� ��������� ��� �� �����
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL("["),NULL),												
						GE("��������� ������������"),												
						GE(FL("]")),													
						_EW(NULL),								
						CFunctions::FormDeclareArray),// FormSTORE ����������� ��� ��
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),					
						GE(EMPTY_ELEMENT,CFunctions::DeleteFromLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::FormDeclare),//Form Declare �� ��������� ��������� TypeStack; FormDeclare �������� FormSTORE
						_EW(NULL),								
						CFunctions::FormSTORE),// FormSTORE ����������� ��� ��
	NULL));	
	////--------------- 34
	SA.AddRule(CSntxRule("�������������",						NULL,
		CSntxVariant(	GE("��������� ������������"),
						_EW(NULL)), 
	NULL));

	////--------------- 35
	SA.AddRule(CSntxRule("����������",							NULL,
		CSntxVariant(	GE("SolitaryWords"),
						_EW(NULL)), 
		CSntxVariant(	GE("IgnoreWhiteSpaces"),
						_EW(NULL)), 
		CSntxVariant(	GE("IgnoreCase"),
						_EW(NULL)), 
		CSntxVariant(	GE("��������� ����������"),
						_EW(NULL)), 
		CSntxVariant(	GE("���������� ������"),
						_EW(NULL)), 
		CSntxVariant(	GE("���������� ��������"),
						_EW(NULL)), 
		CSntxVariant(	GE("����������� ����������"),
						_EW(NULL)), 
		CSntxVariant(	GE("����������� ���������� while"),
						_EW(NULL)), 
		CSntxVariant(	GE("����������-���������"),
						_EW(NULL)), 
	NULL));

	////--------------- 36
	SA.AddRule(CSntxRule("SolitaryWords",						E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL("SolitaryWords"),NULL),
						GE(FL("=")),
						GE("On,Off,Auto"),												
						GE(FL (";")),
						_EW(NULL),		
						CFunctions::FormSolitaryWords), 
	NULL));

	////--------------- 37
	SA.AddRule(CSntxRule("IgnoreWhiteSpaces",					E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL("IgnoreWhiteSpaces"),NULL),
						GE(FL("=")),
						GE("On,Off,Auto"),
						GE(FL (";")),
						_EW(NULL),		
						CFunctions::FormIgnoreWhiteSpaces), 
	NULL));

	////--------------- 38
	SA.AddRule(CSntxRule("IgnoreCase",							E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL("IgnoreCase"),NULL),
						GE(FL("=")),
						GE("On,Off,Auto"),
						GE(FL (";")),
						_EW(NULL),		
						CFunctions::FormIgnoreCase),
	NULL));

	////--------------- 39
	SA.AddRule(CSntxRule("���������� ��������",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL("return"),NULL),
						GE(FL (";")),
						_EW(NULL),					
						CFunctions::FormEMPTY_RET),
		CSntxVariant(	GE(FL("return"),NULL),
						GE("���������"),
						GE(FL (";"),CFunctions::FormRETURN),						
						_EW(NULL)),
	NULL));

	////--------------- 40
	SA.AddRule(CSntxRule("����������� ����������",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL ("for"),NULL,CFunctions::IncLevel),
						GE(FL ("(")),
						GE("���� ����������� ����������"),
						GE(FL (";"),CFunctions::FormFOR),
						GE("���������",CFunctions::FormIF), 
						GE(FL (")"),CFunctions::FormRESTORE),								
						GE("����������"),												
						GE(FL("next")),
						GE(FL ("(")),
						GE("���������"),
						GE(FL (")"),CFunctions::FormRESTORE),														
						GE(FL (";"),CFunctions::FormENDFOR),
						_EW(NULL),	
						CFunctions::DecLevel), 
	NULL));
	////--------------- 40
	SA.AddRule(CSntxRule("���� ����������� ����������",	E_MISSING_SEMICOLON,
		CSntxVariant(	GE("������������ ����"),
						GE("������ ���� ������������"),
						_EW(NULL),CFunctions::DeleteFromTypeStack), //��� ���������� �������������� ���� 
		CSntxVariant(	GE("���������"),
						_EW(NULL),												NULL), 
	NULL));

	////--------------- 40
	SA.AddRule(CSntxRule("����������� ���������� while",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL ("while")),
						GE(FL ("("),CFunctions::FormFOR),
						GE("���������",CFunctions::FormIF), 
						GE(FL (")"),CFunctions::FormRESTORE),								
						GE("����������",CFunctions::FormRESTORE),
						_EW(NULL),												
						CFunctions::FormENDFOR), 
	NULL));
	
	////--------------- 41
	SA.AddRule(CSntxRule("����������-���������",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL (";")),
						_EW(NULL),												NULL), 
		CSntxVariant(	GE("���������"),
						GE(FL (";"),CFunctions::FormRESTORE),
						_EW(NULL),												NULL), 
	NULL));
	////--------------- 42 
	SA.AddRule(CSntxRule("��������� ����������",				NULL,
		CSntxVariant(	GE(FL ("{"),_EW(E_MISSING_LEFT_BRACE),CFunctions::IncLevel),
/*OPTIONAL*/			GE("���� ��������� ����������",NULL,R_OPTIONAL),
						GE(FL ("}"),E_MISSING_RIGHT_BRACE,CFunctions::DecLevel),		
						_EW(NULL)), 
	NULL));
	////--------------- 43 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("���� ��������� ����������",			NULL,
		CSntxVariant(	GE("������ ����-�����"),
/*OPTIONAL*/			GE("���� ��������� ����������",NULL,R_OPTIONAL),
						_EW(NULL)),
	NULL));
	////--------------- 44
	SA.AddRule(CSntxRule("������ ����-�����",					NULL,
		CSntxVariant(	GE("������ ����������"),
						GE("������ ����������"),
						_EW(NULL)),
		CSntxVariant(	GE("������ ����������"),
						_EW(NULL)),
		CSntxVariant(	GE("������ ����������"),
						_EW(NULL)),
	NULL));

	////--------------- 45 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ����������",					NULL,
		CSntxVariant(	GE("����������"),
/*OPTIONAL*/			GE("������ ����������",NULL,R_OPTIONAL),
						_EW(NULL)),
	NULL));
	////--------------- 46
	SA.AddRule(CSntxRule("���������� ������",					NULL,
		CSntxVariant(	GE(FL ("if"),NULL),
						GE(FL ("("),_EW(E_MISSING_LEFT_BRACKET)),
						GE("���������"),
						GE(FL (")"),_EW(E_MISSING_RIGHT_BRACKET),CFunctions::FormIF), 
						GE("����������"),
						GE(FL ("else"),CFunctions::FormTHEN_ELSE),
						GE("����������",CFunctions::FormELSE),							
						_EW(NULL)),
		CSntxVariant(	GE(FL ("if"),NULL),
						GE(FL ("("),_EW(E_MISSING_LEFT_BRACKET)),
						GE("���������"),
						GE(FL (")"),_EW(E_MISSING_RIGHT_BRACKET),CFunctions::FormIF), 
						GE("����������"/*	CFunctions::FormTHEN*/),					
						_EW(NULL),						
						CFunctions::FormTHEN), 
	NULL));

	////--------------- 47 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("���������",							NULL,				
		CSntxVariant(	GE("��������� ������������"),
/*OPTIONAL*/			GE(FL(","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("���������",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)), 
	NULL));
	
	////--------------- 48
	SA.AddRule(CSntxRule("��������� ������������",				NULL,
		CSntxVariant(	GE("������� ���������"),
						GE("�������� ������������"),
						GE("��������� ������������"),
						_EW(NULL),					
						CFunctions::FormASSIGN), 
		CSntxVariant(	GE("�������� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 49
	SA.AddRule(CSntxRule("�������� ������������",				NULL,
		CSntxVariant(	GE(LG(	"=",
								"+=",
								"-=",
								"/=",
								"*=",
								"%=",
								">>=",
								"<<=",
								"&=",
								"^=",
								"|=",
								NULL)),
						_EW(NULL),	
						CFunctions::CreateAssignOperation),
	NULL));

	////--------------- 50
	SA.AddRule(CSntxRule("�������� ���������",					NULL,
//		CSntxVariant(	GE("����� ��� ���������"),GE(FL ("?")),GE("���������").GE(FL (":")),GE("�������� ���������"),NULL), 
//						_EW(NULL)), 
		CSntxVariant(	GE("����� ��� ���������"),
						_EW(NULL)), 

	NULL));
	
	////--------------- 51
	SA.AddRule(CSntxRule("����� ��� ���������",				NULL,
		CSntxVariant(	GE("����� � ���������"),
						GE(FL ("||"),NULL),
						GE("����� ��� ���������"),
						_EW(NULL),				
						CFunctions::FormL_OR), 
		CSntxVariant(	GE("����� � ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 52
	SA.AddRule(CSntxRule("����� � ���������",					NULL,
		CSntxVariant(	GE("��� ���������"),
						GE(FL ("&&"),NULL),
						GE("����� � ���������"),
						_EW(NULL),				
						CFunctions::FormL_AND), 
		CSntxVariant(	GE("��� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 53
	SA.AddRule(CSntxRule("��� ���������",						NULL,
		CSntxVariant(	GE("���� ��� ���������"),
						GE(FL ("|"),NULL),
						GE("��� ���������"),
						_EW(NULL),					
						CFunctions::FormOR), 
		CSntxVariant(	GE("���� ��� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 54
	SA.AddRule(CSntxRule("���� ��� ���������",				NULL,
		CSntxVariant(	GE("� ���������"),
						GE(FL ("^"),NULL),
						GE("���� ��� ���������"),
						_EW(NULL),					
						CFunctions::FormXOR), 
		CSntxVariant(	GE("� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 55
	SA.AddRule(CSntxRule("� ���������",						NULL,
		CSntxVariant(	GE("��������� ���������"),
						GE(FL ("&"),NULL),
						GE("� ���������"),		
						_EW(NULL),					
						CFunctions::FormAND), 
		CSntxVariant(	GE("��������� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 56
	SA.AddRule(CSntxRule("��������� ���������",				NULL,
		CSntxVariant(	GE("��������� ���������"),
						GE(FL ("=="),NULL),
						GE("��������� ���������"),
						_EW(NULL),							
						CFunctions::FormEQ), 
		CSntxVariant(	GE("��������� ���������"),
						GE(FL ("!="),NULL),
						GE("��������� ���������"),
						_EW(NULL),							
						CFunctions::FormINEQ), 
		CSntxVariant(	GE("��������� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 57
	SA.AddRule(CSntxRule("��������� ���������",				NULL,
		CSntxVariant(	GE("��������� ���������"),
						GE(FL ("<"),NULL),
						GE("��������� ���������"),
						_EW(NULL),							
						CFunctions::FormLESS), 
		CSntxVariant(	GE("��������� ���������"),
						GE(FL (">"),NULL),
						GE("��������� ���������"),
						_EW(NULL),							
						CFunctions::FormMORE), 
		CSntxVariant(	GE("��������� ���������"),
						GE(FL ("<="),NULL),
						GE("��������� ���������"),
						_EW(NULL),							
						CFunctions::FormLESS_EQ), 
		CSntxVariant(	GE("��������� ���������"),
						GE(FL (">="),NULL),
						GE("��������� ���������"),
						_EW(NULL),
						CFunctions::FormMORE_EQ), 
		CSntxVariant(	GE("��������� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 58
	SA.AddRule(CSntxRule("��������� ���������",				NULL,
		CSntxVariant(	GE("���������� ���������"),
						GE(FL ("<<"),NULL),
						GE("��������� ���������"),
						_EW(NULL),							
						CFunctions::FormSHL), 
		CSntxVariant(	GE("���������� ���������"),
						GE(FL (">>"),NULL),
						GE("��������� ���������"),	
						_EW(NULL),							
						CFunctions::FormSHR), 
		CSntxVariant(	GE("���������� ���������"),	
						_EW(NULL)), 
	NULL));
	
	////--------------- 59
	SA.AddRule(CSntxRule("���������� ���������",				NULL,
		CSntxVariant(	GE("����������������� ���������"),
							GE(FL ("+" ),NULL),
							GE("���������� ���������"),	
						_EW(NULL),							
						CFunctions::FormADD), 
		CSntxVariant(	GE("����������������� ���������"),
							GE(FL ("-" ),NULL),
							GE("���������� ���������"),	
						_EW(NULL),							
						CFunctions::FormSUB), 
		CSntxVariant(	GE("����������������� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 60
	SA.AddRule(CSntxRule("����������������� ���������",		NULL,
		CSntxVariant(	GE("���������� � ����"),
						GE(FL ("*" ),NULL),
						GE("����������������� ���������"),
						_EW(NULL),							
						CFunctions::FormMUL), 
		CSntxVariant(	GE("���������� � ����"),
						GE(FL ("/"),NULL),
						GE("����������������� ���������"),
						_EW(NULL),							
						CFunctions::FormDIV), 
		CSntxVariant(	GE("���������� � ����"),
						GE(FL ("%"),NULL),
						GE("����������������� ���������"),
						_EW(NULL),							
						CFunctions::FormRMNDR), 
		CSntxVariant(	GE("���������� � ����"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 61
	SA.AddRule(CSntxRule("���������� � ����",					NULL,
//		CSntxVariant(	GE(FL ("(" )),GE("��� ����"),GE(FL (")" )),GE("���������� � ����"),	NULL), 
//						_EW(NULL)), 
		CSntxVariant(	GE("������� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 62
	SA.AddRule(CSntxRule("������� ���������",					NULL,
//		CSntxVariant(	GE(FL ("++" )),GE("������� ���������"),
//						_EW(NULL)), 
//		CSntxVariant(	GE(FL ("--" )),GE("������� ���������"),
//						_EW(NULL)), 
		CSntxVariant(	GE(FL ("+" )),
						GE("���������� � ����"),	
						_EW(NULL)), 
		CSntxVariant(	GE(FL ("-" )),														
						GE("���������� � ����"),	
						_EW(NULL), 
						CFunctions::FormNEG),
		CSntxVariant(	GE(FL ("!"),NULL),														
						GE("���������� � ����"),	
						_EW(NULL),				
						CFunctions::FormNOT),
		CSntxVariant(	GE(FL ("~" ),NULL),
						GE("���������� � ����"),
						_EW(NULL)), 
		CSntxVariant(	GE("����������� ���������"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 63
	SA.AddRule(CSntxRule("����������� ���������",				NULL,
// ����� ������� ������ ������ ������, ����� ���� ��������� ������ ����� �������, �� ��� ������� "��� ����������" ������ � ��������������� - ��� ������������ ���������
		CSntxVariant(	GE(FL("sizeof"),NULL),
						GE(FL ("(")),
						GE("������������ ���� ��� �������������"),
						GE(FL (")")),
						_EW(NULL)), 
		CSntxVariant(	GE(FL("size"),NULL),												
						GE(FL ("(")),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL (")")),					
						_EW(NULL),	
						CFunctions::FormSIZEA),
		CSntxVariant(	GE("����� �������"),						
						GE(FL ("."),CFunctions::EnterToStruct),
						GE("����������� ���������"),	
						_EW(NULL),
						CFunctions::ExitFromStruct), 
		CSntxVariant(	GE("����� �������"),			
						_EW(NULL)), 
		CSntxVariant(	GE("������ �������"),
						GE(FL ("."),CFunctions::EnterToStruct),
						GE("����������� ���������"),	
						_EW(NULL),
						CFunctions::ExitFromStruct), 
		CSntxVariant(	GE("������ �������"),
						GE(FL ("++" ),NULL),
						_EW(NULL),						
						CFunctions::FormINC_POST), 
		CSntxVariant(	GE("������ �������"),
						GE(FL ("--" ),NULL),
						_EW(NULL),						
						CFunctions::FormDEC_POST), 
		CSntxVariant(	GE("������ �������"),	
						_EW(NULL)), 
		CSntxVariant(	GE("��������� ���������"),
						_EW(NULL)), 		
	
	NULL));
	
	////--------------- 64
	SA.AddRule(CSntxRule("������������ ���� ��� �������������",NULL,
		CSntxVariant(	GE(LG(	"void",
								"bool",
								"int",
								"char",
								"short",
								"double",
								"string",
								NULL)),
						GE(EMPTY_ELEMENT, CFunctions::PushToLexemeStack),
						_EW(NULL),	
						CFunctions::FormSIZEOF), 
		CSntxVariant(	GE(ID_CLASS_ID),
						GE(EMPTY_ELEMENT, CFunctions::PushToLexemeStack),
						_EW(NULL),	
						CFunctions::FormSIZEOF),
	NULL));
	
	////--------------- 65
	SA.AddRule(CSntxRule("�������������",NULL,
		CSntxVariant(	GE(LG(	"void",
								"bool",
								"int",
								"char",
								"short",
								"double",
								"string",
								NULL)
							),
						GE(EMPTY_ELEMENT, CFunctions::PushToLexemeStack),
						_EW(NULL),	
						CFunctions::FormSIZEA), 
	NULL));

	////--------------- 66
	SA.AddRule(CSntxRule("������ �������",							NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL ("[" ),CFunctions::ResetEnter),
						GE("���������"),
						GE(FL ("]" ),CFunctions::RestoreEnter),
						_EW(NULL),					
						CFunctions::FormSTORE_IDX), 
		CSntxVariant(	GE("��� ����������"),
						_EW(NULL)), 

	NULL));
	
	////--------------- 67
	SA.AddRule(CSntxRule("��� ����������",							NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::FormSTORE),
						_EW(NULL)), 
	NULL));
	
	////--------------- 68
	SA.AddRule(CSntxRule("����� �������",				NULL,
		CSntxVariant(	GE(FL("Parse"),NULL),
						GE(FL("(")),
						GE(CONST_CLASS_ID, CFunctions::PushToLexemeStack),
						GE(FL(")")),
						_EW(NULL),
						CFunctions::FormPARSE),// FormPARSE ������ � ���� ����������	
		CSntxVariant(	GE(FL("SaveAllFiles"),NULL),
						GE(FL("(")),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormSAVE_FILES),// FormPARSE ������ � ���� ����������	
		CSntxVariant(	GE(FL("GetText"),NULL),
						GE(FL("(")),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormGET_TEXT),// FormPARSE ������ � ���� ����������	
		CSntxVariant(	GE(FL("GetFilePos"),NULL),
						GE(FL("(")),
						GE(FL(")")),														
						_EW(NULL),	
						CFunctions::FormGET_FILE_POS),// FormPARSE ������ � ���� ����������	
		CSntxVariant(	GE(FL("InsertBefore"),NULL),
						GE(FL("(")),
						GE("��������� ������������"),
						GE(FL(",")),
						GE("��������� ������������"), 
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormINS_BEFORE),
		CSntxVariant(	GE(FL("InsertAfter"),NULL),
						GE(FL("(")),
						GE("��������� ������������"),
						GE(FL(",")),
						GE("��������� ������������"),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormINS_AFTER), 
		CSntxVariant(	GE(FL("IgnoreBlock"),NULL),
						GE(FL("(")),
						GE("��������� ������������"),
						GE(FL(",")),
						GE("��������� ������������"),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormIGNORE_BLOCK),
		CSntxVariant(	GE(FL("ReplaceBlock"),NULL),
						GE(FL("(")),
						GE("��������� ������������"),
						GE(FL(",")),
						GE("��������� ������������"),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormREPLACE_BLOCK),
		CSntxVariant(	GE(FL("print"),NULL),
						GE(FL("(")),
						GE("������ ���������� print"),
						GE(FL(")")),
						_EW(NULL)),
		// �� ��������, ��� ����� R_FORBID_BACK PERMIT_BACK ���� ��������� �� ������� 
		// ���� ��� �� �����, ������� NULL
		CSntxVariant(	GE(FL("write"),NULL),
						GE(FL("(")),
						GE("������ ���������� write"),
						GE(FL(")")),
						_EW(NULL)),			
		CSntxVariant(	GE(FL("format"),NULL),
						GE(FL("(")),
						GE("������ ����������"),
						GE(FL(")"),CFunctions::FormFORMAT),
						_EW(NULL)),
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::SearchFunc),
						GE(FL ("(" )),
						GE(FL (")" )),
						_EW(NULL),
						CFunctions::FormCALL),	// FormCALL ��������� ��������� �� ����� ���������� � ��� ���������� �������, ���������� ���� SearchFunc � ��������� PUSH
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::SearchFunc),
						GE(FL ("(" )),
						GE("������ ����������"),
						GE(FL (")" )),
						_EW(NULL),
						CFunctions::FormCALL),	// FormCALL ��������� ��������� �� ����� ���������� � ��� ���������� �������, ���������� ���� SearchFunc � ��������� PUSH
	NULL));
	////--------------- 69 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ����������",			NULL,
		CSntxVariant(	GE("��������� ������������"),
/*OPTIONAL*/			GE(FL (","),NULL,R_OPTIONAL),
/**/					GE("������ ����������",NULL,R_CHILD_OPTIONAL),
						_EW(NULL),	
						CFunctions::IncParamCount), //������� ������� �� ���� ������������ ��������� �������� � ����
	NULL));

	////--------------- 70 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ���������� print",			NULL,
		CSntxVariant(	GE("��������� ������������",CFunctions::FormPRINT),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("������ ���������� print",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)), 
	NULL));	
	
	////--------------- 71 (����� ���� �������� ������������� �� �������� � ������)
	SA.AddRule(CSntxRule("������ ���������� write",			NULL,
		CSntxVariant(	GE("��������� ������������",CFunctions::FormWRITE),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("������ ���������� write",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)),
	NULL));	
	
	////--------------- 72
	SA.AddRule(CSntxRule("��������� ���������",	E_BAD_EXPRESSION,
		CSntxVariant(	GE(CONST_CLASS_ID),
						_EW(NULL),								
						CFunctions::FormSTORE), 
		CSntxVariant(	GE(ID_CLASS_ID),
						_EW(NULL),		
						CFunctions::FormSTORE), 
		CSntxVariant(	GE(FL ("(")),
						GE("���������"),
						GE(FL (")")), 
						_EW(NULL)), 
	NULL));
	////---------------	-1+

#ifdef __DIAGNOSTIC__
	base_utils::save_file(string_smart(ApplicationData::APP_DIR,"grammars.txt").c_str(),SA.GetXBNF().c_str());
#endif

	bool b = SA.Rebuild(); //	very IMPORTANT
#ifdef __DIAGNOSTIC__
	base_utils::save_file(string_smart(ApplicationData::APP_DIR,"tree_dump.txt").c_str(),SA.GetTreeDump().c_str());
#endif
	return b;
}
