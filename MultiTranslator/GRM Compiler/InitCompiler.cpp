/********************************************************************
	created:	2003/06/01
	created:	1:6:2003   11:07
	filename: 	$(Projects)\sources\grm compiler\initcompiler.cpp
	file path:	$(Projects)\sources\grm compiler
	file base:	initcompiler
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Модуль инициализации компилятора ЯОП. Задается лексический 
				автомат, грамматика и семантика языка
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

	// Добавляем внутренние переменные
	structs::type	* pType;
	structs::lexeme Lex1,Lex2;	
	// Индексы нижеперечисленных переменных являются статическими, что позволяет обращаться в интерпретаторе к ним по заранее известному номеру
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
	
	//Регистрация значений true и false - в 
	//модуле LexTables.cpp, CLexTable::CLexTable(cstr szFileName) и CLexTable::AddId
	//в будущем это нужно исправить. 

	return true;

}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool init_compiler::InitTypesTable(CTypesTable & TT, const CLexTable & LT)
{
	//Первое выражение GetReservedWord нужно только для того, чтобы убедиться
	//что данный тип имеет зарезервированное слово
	LT.GetReservedWord("void");		TT.CreateSimple("void",	 structs::object::vtNULL);
	LT.GetReservedWord("char");		TT.CreateSimple("char",	 structs::object::vtCHAR);	//char должен идти перед bool
	LT.GetReservedWord("bool");		TT.CreateSimple("bool",	 structs::object::vtBOOL);
	LT.GetReservedWord("small");	TT.CreateSimple("small", structs::object::vtSMALL);	
	LT.GetReservedWord("short");	TT.CreateSimple("short", structs::object::vtSHORT);	
	LT.GetReservedWord("int");		TT.CreateSimple("int",	 structs::object::vtINT);	
	LT.GetReservedWord("double");	TT.CreateSimple("double",structs::object::vtDOUBLE);
	LT.GetReservedWord("string");	TT.CreateSimple("string",structs::object::vtSTRING);
	/* нет явного аналога*/			TT.CreateSimple("variant",structs::object::vtVARIANT);
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
	LA.AddAutomat("Spaces"); // Spaces	 всегда	должен стоять последним, почему	- смотрите его инициализацию

	//Брать ссылки на автоматы можно только после того, как все они внесены
	CLexAutomat	& StringConst	= LA.GetAutomat(0);
	CLexAutomat	& CharConst		= LA.GetAutomat(1);
	CLexAutomat	& Operator		= LA.GetAutomat(2);
	CLexAutomat	& DigitConst	= LA.GetAutomat(3);
	CLexAutomat	& Identifier	= LA.GetAutomat(4);
	CLexAutomat	& Spaces		= LA.GetAutomat(5);

	//////////// End LA	init ///////////////
	#define	_L_NOEW	CLexVariant::err_no_container()
	#define	_L_EW(T)CLexVariant::err_container<T>()
// Правила задания автоматов
// Задание CLexHub состоит из произвольного	количества CLexVariant
// Внутри CLexVariant: 
//		1 -	символ или класс символов, 
//		2 -	точка перехода (может быть выхода или перехода в соседний граф)
//		3 -	Класс ошибки или предупрежедения, 
//		4 -	Список выполняемых функций, 

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
 //	В выражении	сохраняется	только открывающаяся кавычка, ее достаточно	чтобы отличить строковую константу
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
		CLexVariant(litera::TAG_DLAS,	1,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)), //	оптимизация. Может и не	сработать
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
		CLexVariant(litera::TAG_END,	EXIT_TRUE,	_L_NOEW	)			 //	На всякий случай
	));
	Spaces.AddHub(CLexHub(
		CLexVariant(litera::TAG_SPACE,	1,			_L_NOEW,			_LF(CLexFunctions::GetAllSimilary)),
		CLexVariant(litera::TAG_ANY,	EXIT_TRUE,	_L_NOEW				)
	));
	
	// Обратите	внимание на	способы	задания	ANOTHER_HUB	- как список узлов для одного ANOTHER_HUB (см Delimiter),
	// так и в виде	списка ANOTHER_HUB c узлами	(см	Spaces.). 
	// Первый вариант является предпочтительнее, так как он	быстрее	и не предполагает двусмысленностей (приоритетный по	поиску)
	
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
	SA.AddRule(CSntxRule("Программа",							NULL,
		CSntxVariant(	GE("Компонента трансляции"),	
						_EW(NULL),	
						CFunctions::CheckStacks),// стеки должны быть пустыми
	NULL));
	////--------------- 1 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Компонента трансляции",				NULL,
		CSntxVariant(	GE("Внешняя декларация"),
/*OPTIONAL*/			GE("Компонента трансляции",NULL,R_OPTIONAL),
						_EW(NULL)), 	
	NULL));
	
	////--------------- 2
	SA.AddRule(CSntxRule("Внешняя декларация",					NULL,
		CSntxVariant(	GE("Определение лексического правила"),
						_EW(NULL)), 
		CSntxVariant(	GE("Определение правила"),
						_EW(NULL)), 
		CSntxVariant(	GE("Декларация"),
						_EW(NULL)),													
		CSntxVariant(	GE("Определение функции"),									
						_EW(NULL)),
	NULL));

	////--------------- 3
	SA.AddRule(CSntxRule("Определение лексического правила", E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("lex"),E_MISSING_RULE_WORD,NULL),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL ("=")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateLex),
						GE(FL(";"),E_MISSING_SEMICOLON),
						_EW(NULL),			
						CFunctions::CreateLex), 
	NULL));

	////--------------- 3
	SA.AddRule(CSntxRule("Определение правила", E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("rule"),E_MISSING_RULE_WORD,NULL),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateRule),
						GE(FL(">")),
/*OPTIONAL*/			GE(FL(":"),NULL,R_OPTIONAL),
/**/					GE("Список параметров правила",NULL,R_CHILD_OPTIONAL),
						GE(FL("{")),
						GE("Тело правила"),
						GE(FL("}")),												
						_EW(NULL),			
						CFunctions::CreateRule), 
	NULL));

	////--------------- 4 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список параметров правила", E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("Параметр правила"),
/*OPTIONAL*/			GE(FL (","),NULL,R_OPTIONAL),
/**/					GE("Список параметров правила",NULL,R_CHILD_OPTIONAL),			
						_EW(NULL)),
	NULL));
	
	////--------------- 5
	SA.AddRule(CSntxRule("Параметр правила", E_DECLARATION_INCORRECTLY,
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
	SA.AddRule(CSntxRule("Тело правила",				E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("Список вариантов с предиструкциями",CFunctions::CreateRuleSubRoutine),
/*OPTIONAL*/ 	//		GE("Список инструкций",NULL,R_OPTIONAL),					
/*OPTIONAL*/			GE("Список декл-инстр",NULL,R_OPTIONAL),					
						GE(EMPTY_ELEMENT, CFunctions::FormRET),
						_EW(NULL),					
						CFunctions::FormENDSUBR), 	
	NULL));
	
	////--------------- 8 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список вариантов с предиструкциями",			E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("Определение варианта с предиструкциями"),
/*OPTIONAL*/			GE("Список вариантов с предиструкциями",NULL,R_OPTIONAL),						
						_EW(NULL)), 
	NULL));
	
	////--------------- 9
	SA.AddRule(CSntxRule("Определение варианта с предиструкциями",		E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("variant"),E_MISSING_VARIANT_WORD, CFunctions::PreCreateVariant),
						GE(FL("{"),E_SYNTAX_ERROR, NULL),
						GE("Тело варианта"),
						GE(FL("}")),													
						_EW(NULL),						
						CFunctions::CreateVariant), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreVarSubroutine),
						GE(FL(":")),
//						GE("Список инструкций",					CFunctions::FormRET),
						GE("Список декл-инстр",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),
						GE(FL("variant"),E_MISSING_VARIANT_WORD,CFunctions::PreCreateVariant),
						GE(FL("{")),
						GE("Тело варианта"),
						GE(FL("}")),													
						_EW(NULL),						
						CFunctions::CreateVariantWithPreSub), 
	NULL));

	////--------------- 10 
	SA.AddRule(CSntxRule("Тело варианта",				E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("Список термов с предиструкциями",CFunctions::CreateVariantSubRoutine),
						GE("Список декл-инстр"),										
//						GE("Список инструкций"),										
						GE(EMPTY_ELEMENT,		CFunctions::FormRET),
						_EW(NULL),			
						CFunctions::FormENDSUBR), 	
		CSntxVariant(	GE("Список термов с предиструкциями",CFunctions::CreateVariantSubRoutine),
						GE(EMPTY_ELEMENT,		CFunctions::FormRET),
						_EW(NULL),				
						CFunctions::FormENDSUBR), 	
	NULL));

	////--------------- 11 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список термов с предиструкциями",	E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("Определение терма с предиструкциями"),
/*OPTIONAL*/			GE("Список термов с предиструкциями",NULL,R_OPTIONAL),
						_EW(NULL)), 
	NULL));
		
	////--------------- 12
	SA.AddRule(CSntxRule("Определение терма с предиструкциями",E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("Определение терминала"),									
						_EW(NULL)), 
		CSntxVariant(	GE("Определение нетерминала"),									
						_EW(NULL)), 
	NULL));
	
	////--------------- 13
	int a=0;
	SA.AddRule(CSntxRule("Определение терминала",		E_DECLARATION_INCORRECTLY,
		// Не ставить R_FORBID_BACK - это может быть либо терминал, либо нетерминал
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE("Имя терминала"), // в нем же  CreateSubRoutine
/*OPTIONAL*/			GE(FL(":"),		NULL,	R_OPTIONAL),
/**/					GE(FL("error"),	NULL,									R_CHILD_OPTIONAL),
/**/					GE(FL("="),		NULL,									R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,		R_CHILD_OPTIONAL),
						GE("Блок действий для symbol"),	
						_EW(NULL),					
						CFunctions::CreateTerminal), 
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("["),CFunctions::SetOptionalTermFlag),
						GE("Имя терминала"), // в нем же  CreateSubRoutine
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
/*OPTIONAL*/			GE(FL(":"),		NULL,	R_OPTIONAL),
/**/					GE(FL("error"),	NULL,									R_CHILD_OPTIONAL),
/**/					GE(FL("="),		NULL,									R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,		R_CHILD_OPTIONAL),
						GE("Блок действий для symbol"),	
						_EW(NULL),					
						CFunctions::CreateTerminal), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreTermSubroutine),
						GE(FL(":")),
//						GE("Список инструкций",					CFunctions::FormRET),
						GE("Список декл-инстр",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE("Имя терминала"), // в нем же  CreateSubRoutine
/*OPTIONAL*/			GE(FL(":"),NULL,R_OPTIONAL),
/**/					GE(FL("error"),NULL,	R_CHILD_OPTIONAL),
/**/					GE(FL("="),NULL,		R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,R_CHILD_OPTIONAL),
						GE("Блок действий для symbol"),	
						_EW(NULL),				
						CFunctions::CreateTerminalWithPreSub), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreTermSubroutine),
						GE(FL(":")),
//						GE("Список инструкций",					CFunctions::FormRET),
						GE("Список декл-инстр",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("[")),
						GE("Имя терминала"), // в нем же  CreateSubRoutine
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
/*OPTIONAL*/			GE(FL(":"),NULL,R_OPTIONAL),
/**/					GE(FL("error"),NULL,	R_CHILD_OPTIONAL),
/**/					GE(FL("="),NULL,		R_CHILD_OPTIONAL),
/**/					GE(CONST_CLASS_ID, CFunctions::SetTermErrorCode,R_CHILD_OPTIONAL),
						GE("Блок действий для symbol"),	
						_EW(NULL),		
						CFunctions::CreateTerminalWithPreSub), 
	NULL));

	////--------------- 14
	SA.AddRule(CSntxRule("Имя терминала",		E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("STD_ID"),CFunctions::PreCreateTerminal),		// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_LINE"),CFunctions::PreCreateTerminal),	// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_INT_CONST"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_NUM_CONST"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_SQMSTR_CONST"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_DQMSTR_CONST"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_STR_CONST"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_COMMON_CONST"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_ALPHA"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_NUM"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_ALNUM"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(FL("STD_CNTRL"),CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(CONST_CLASS_ID,CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PreCreateTerminal),// кладет имя в стек лексем, 
						_EW(NULL)),
	NULL));

	////--------------- 15 
	SA.AddRule(CSntxRule("Определение нетерминала",		E_DECLARATION_INCORRECTLY,
	// R_FORBID_BACK можно поставить, но необходимо учесть, что в определении терма первым должен стоять "Определение терминала"
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// кладет имя в стек лексем, 
																			// в нем же  CreateSubRoutine
						GE(FL(">")),
						GE("Блок действий для symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminal), 		
		CSntxVariant(	GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("["),CFunctions::SetOptionalTermFlag),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// кладет имя в стек лексем, 
																			// в нем же  CreateSubRoutine
						GE(FL(">")),
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
						GE("Блок действий для symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminal),
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreUTermSubroutine),
						GE(FL(":")),
//						GE("Список инструкций",					CFunctions::FormRET),
						GE("Список декл-инстр",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),							
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// кладет имя в стек лексем, 
																				// в нем же  CreateSubRoutine
						GE(FL(">")),
						GE("Блок действий для symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminalWithPreSub), 
		CSntxVariant(	GE(FL("before"),						CFunctions::CreatePreUTermSubroutine),
						GE(FL(":")),
//						GE("Список инструкций",					CFunctions::FormRET),
						GE("Список декл-инстр",					CFunctions::FormRET),
						GE(EMPTY_ELEMENT,						CFunctions::FormENDSUBR),							
						GE(LG("term","symbol",NULL),E_MISSING_TERM_WORD,NULL),
						GE(FL("["),CFunctions::SetOptionalTermFlag),
						GE(FL("<")),
						GE(CONST_CLASS_ID,CFunctions::PreCreateNonterminal),	// кладет имя в стек лексем, 
																				// в нем же  CreateSubRoutine
						GE(FL(">")),
						GE(FL("]")),
/*OPTIONAL*/			GE(FL("*"),CFunctions::SetChidOptionalTermFlag,R_OPTIONAL),
						GE("Блок действий для symbol"),	
						_EW(NULL),				
						CFunctions::CreateNonterminalWithPreSub), 
	NULL));

	////--------------- ??
	SA.AddRule(CSntxRule("Блок действий для symbol",	E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE("Составная инструкция"),										
						GE(EMPTY_ELEMENT,	CFunctions::FormRET),
						GE(EMPTY_ELEMENT,	CFunctions::FormENDSUBR),
/*OPTIONAL*/			GE(FL(";"),NULL,R_OPTIONAL),	//Пользы никакой, просто иногда люди ставят по привычке
						_EW(NULL)),
		CSntxVariant(	GE(FL(";"),E_MISSING_SEMICOLON),
						GE(EMPTY_ELEMENT,	CFunctions::FormRET),
						GE(EMPTY_ELEMENT,	CFunctions::FormENDSUBR),
						_EW(NULL)), 
	NULL));

	////--------------- 16
	SA.AddRule(CSntxRule("Определение функции",				E_DECLARATION_INCORRECTLY,
		CSntxVariant(	GE(FL("extern")),
						GE("Спецификатор типа"),
						GE("Декларатор функции",CFunctions::DestroyCodeFragment),
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
		CSntxVariant(	GE("Спецификатор типа"),
						GE("Декларатор функции",CFunctions::AppendCodeFragment),
						GE("Составная инструкция"),										
						GE(EMPTY_ELEMENT,	CFunctions::FormEMPTY_RET),
						GE(EMPTY_ELEMENT,	CFunctions::DecLevel),
						_EW(NULL),
						CFunctions::FormENDF), 
	NULL));
	////--------------- -17 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список деклараций",					NULL,
		CSntxVariant(	GE("Декларация"),
/*OPTIONAL*/			GE("Список деклараций",NULL,R_OPTIONAL),
						_EW(NULL)),
	NULL));
	////--------------- 18
	SA.AddRule(CSntxRule("Декларация",							NULL,
		CSntxVariant(	GE("Const спецификатор"),
						GE(FL(";"),E_MISSING_SEMICOLON,CFunctions::FormRESTORE),
						_EW(NULL)), 
		CSntxVariant(	GE("Спецификатор типа"),
						GE("Список иниц деклараторов"),
						GE(FL(";"),E_MISSING_SEMICOLON,CFunctions::FormRESTORE),
						_EW(NULL),
						CFunctions::DeleteFromTypeStack), //Тип помещается Спецификатором типа
		CSntxVariant(	GE("Структ спецификатор"),
						GE(FL(";"),E_MISSING_SEMICOLON,CFunctions::FormRESTORE), 
						_EW(NULL)), 
	NULL));
	////--------------- 19
	SA.AddRule(CSntxRule("Const спецификатор",			NULL,
		CSntxVariant(	GE(FL("const")),
						GE("Список const деклараций"),
						_EW(NULL)), 
	NULL));
	////--------------- 20 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список const деклараций",			NULL,
		CSntxVariant(	GE("Const декларация"),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("Список const деклараций",NULL,R_CHILD_OPTIONAL),			
						_EW(NULL)), 
	NULL));
	////--------------- 21
	SA.AddRule(CSntxRule("Const декларация",					NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL ("="),NULL,CFunctions::CreateAssignOperation),
						GE("Инициализатор"), 
						_EW(NULL),
						CFunctions::FormDeclareConst), 
	NULL));	
	////--------------- 22
	SA.AddRule(CSntxRule("Спецификатор типа",					E_DECLARATION_INCORRECTLY,
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
	SA.AddRule(CSntxRule("Структ спецификатор",				NULL,
		CSntxVariant(	GE(FL ("struct"),NULL),
						GE(ID_CLASS_ID,CFunctions::CreateStruct),
						GE(FL("{")),
						GE("Список структ деклараций"),
						GE(FL("}")),
						_EW(NULL)), 
	NULL));
	////--------------- 24 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список структ деклараций",			NULL,
		CSntxVariant(	GE("Структ декларация"),
/*OPTIONAL*/			GE("Список структ деклараций",NULL,R_OPTIONAL),
						_EW(NULL)), 
	NULL));
	////--------------- 25 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список иниц деклараторов",			NULL,
		CSntxVariant(	GE("Иниц декларатор"),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("Список иниц деклараторов",NULL,R_CHILD_OPTIONAL), 
						_EW(NULL)), 
	NULL));
	////--------------- 26
	SA.AddRule(CSntxRule("Иниц декларатор",					NULL,
		CSntxVariant(	GE("Собственно декларатор"),
						GE(FL ("="),NULL,CFunctions::CreateAssignOperation),
						GE("Инициализатор"), 
						_EW(NULL),							
						CFunctions::FormASSIGN), 
		CSntxVariant(	GE("Собственно декларатор"),									
						_EW(NULL)), 
	NULL));	
	////--------------- 27
	SA.AddRule(CSntxRule("Структ декларация",				NULL,
		CSntxVariant(	GE("Спецификатор типа"),
						GE("Список структ деклараторов"), 
						GE(FL (";"),CFunctions::FormRESTORE),							
						_EW(NULL),
						CFunctions::DeleteFromTypeStack), //Тип помещается Спецификатором типа													
	NULL));	
	////--------------- 28 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список структ деклараторов",			NULL,
		CSntxVariant(	GE("Структ декларатор"),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("Список структ деклараторов",NULL,R_CHILD_OPTIONAL), 
						_EW(NULL)), 
	NULL));
	////--------------- 29
	SA.AddRule(CSntxRule("Структ декларатор",					NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL("[")),												
						GE("Выражение присваивания"),												
						GE(FL("]")),							
						_EW(NULL),								
						CFunctions::AddToStructArray),// FormSTORE выполняется там же
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						_EW(NULL),								
						CFunctions::AddToStruct),
	NULL));	

	////--------------- 30
	SA.AddRule(CSntxRule("Декларатор функции",					NULL,
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
						GE("Список параметров"),
						GE(FL (")")),													
/*OPTIONAL*/			GE(FL ("["),NULL,R_OPTIONAL),													
/**/					GE(FL ("]"),NULL,R_CHILD_OPTIONAL),								
						_EW(NULL)), 
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::CreateFunc),							
						GE(EMPTY_ELEMENT,CFunctions::IncLevel),
						GE(FL ("("),CFunctions::CreateCodeFragment), 
						GE("Список параметров"),
						GE(FL (")")),
						_EW(NULL)), 
	NULL));	
	////--------------- 31 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список параметров",			NULL,
		CSntxVariant(	GE("Параметр"),	
/*OPTIONAL*/			GE(FL (","),NULL,R_OPTIONAL),
/**/					GE("Список параметров",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)), 
	NULL));
	////--------------- 32
	SA.AddRule(CSntxRule("Параметр",					NULL,
		CSntxVariant(	GE("Спецификатор типа"),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::DeleteFromLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::FormDeclare),//Form Declare не изменяяет состояние TypeStack; FormDeclare вызывает FormSTORE
						_EW(NULL),									
						CFunctions::AddToFuncByVal),	 //AddToFunc удаляет тип из стека типов (DeleteFromStack) и формирует POP
		CSntxVariant(	GE("Спецификатор типа"),
						GE(FL("&")),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::DeleteFromLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::FormDeclare),//Form Declare не изменяяет состояние TypeStack; FormDeclare вызывает FormSTORE
						_EW(NULL),									
						CFunctions::AddToFuncByRef),	 //AddToFunc удаляет тип из стека типов (DeleteFromStack) и формирует POP
	NULL));

	////--------------- 33
	SA.AddRule(CSntxRule("Собственно декларатор",				NULL,
	// Функции FormDeclare... передают тип далее, не выкидывая его из стека
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL("["),NULL),												
						GE("Выражение присваивания"),												
						GE(FL("]")),													
						_EW(NULL),								
						CFunctions::FormDeclareArray),// FormSTORE выполняется там же
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),					
						GE(EMPTY_ELEMENT,CFunctions::DeleteFromLexemeStack),
						GE(EMPTY_ELEMENT,CFunctions::FormDeclare),//Form Declare не изменяяет состояние TypeStack; FormDeclare вызывает FormSTORE
						_EW(NULL),								
						CFunctions::FormSTORE),// FormSTORE выполняется там же
	NULL));	
	////--------------- 34
	SA.AddRule(CSntxRule("Инициализатор",						NULL,
		CSntxVariant(	GE("Выражение присваивания"),
						_EW(NULL)), 
	NULL));

	////--------------- 35
	SA.AddRule(CSntxRule("Инструкция",							NULL,
		CSntxVariant(	GE("SolitaryWords"),
						_EW(NULL)), 
		CSntxVariant(	GE("IgnoreWhiteSpaces"),
						_EW(NULL)), 
		CSntxVariant(	GE("IgnoreCase"),
						_EW(NULL)), 
		CSntxVariant(	GE("Составная инструкция"),
						_EW(NULL)), 
		CSntxVariant(	GE("Инструкция выбора"),
						_EW(NULL)), 
		CSntxVariant(	GE("Инструкция возврата"),
						_EW(NULL)), 
		CSntxVariant(	GE("Циклическая инструкция"),
						_EW(NULL)), 
		CSntxVariant(	GE("Циклическая инструкция while"),
						_EW(NULL)), 
		CSntxVariant(	GE("Инструкция-выражение"),
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
	SA.AddRule(CSntxRule("Инструкция возврата",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL("return"),NULL),
						GE(FL (";")),
						_EW(NULL),					
						CFunctions::FormEMPTY_RET),
		CSntxVariant(	GE(FL("return"),NULL),
						GE("Выражение"),
						GE(FL (";"),CFunctions::FormRETURN),						
						_EW(NULL)),
	NULL));

	////--------------- 40
	SA.AddRule(CSntxRule("Циклическая инструкция",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL ("for"),NULL,CFunctions::IncLevel),
						GE(FL ("(")),
						GE("Иниц циклической инструкции"),
						GE(FL (";"),CFunctions::FormFOR),
						GE("Выражение",CFunctions::FormIF), 
						GE(FL (")"),CFunctions::FormRESTORE),								
						GE("Инструкция"),												
						GE(FL("next")),
						GE(FL ("(")),
						GE("Выражение"),
						GE(FL (")"),CFunctions::FormRESTORE),														
						GE(FL (";"),CFunctions::FormENDFOR),
						_EW(NULL),	
						CFunctions::DecLevel), 
	NULL));
	////--------------- 40
	SA.AddRule(CSntxRule("Иниц циклической инструкции",	E_MISSING_SEMICOLON,
		CSntxVariant(	GE("Спецификатор типа"),
						GE("Список иниц деклараторов"),
						_EW(NULL),CFunctions::DeleteFromTypeStack), //Тип помещается Спецификатором типа 
		CSntxVariant(	GE("Выражение"),
						_EW(NULL),												NULL), 
	NULL));

	////--------------- 40
	SA.AddRule(CSntxRule("Циклическая инструкция while",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL ("while")),
						GE(FL ("("),CFunctions::FormFOR),
						GE("Выражение",CFunctions::FormIF), 
						GE(FL (")"),CFunctions::FormRESTORE),								
						GE("Инструкция",CFunctions::FormRESTORE),
						_EW(NULL),												
						CFunctions::FormENDFOR), 
	NULL));
	
	////--------------- 41
	SA.AddRule(CSntxRule("Инструкция-выражение",				E_MISSING_SEMICOLON,
		CSntxVariant(	GE(FL (";")),
						_EW(NULL),												NULL), 
		CSntxVariant(	GE("Выражение"),
						GE(FL (";"),CFunctions::FormRESTORE),
						_EW(NULL),												NULL), 
	NULL));
	////--------------- 42 
	SA.AddRule(CSntxRule("Составная инструкция",				NULL,
		CSntxVariant(	GE(FL ("{"),_EW(E_MISSING_LEFT_BRACE),CFunctions::IncLevel),
/*OPTIONAL*/			GE("Блок составной инструкции",NULL,R_OPTIONAL),
						GE(FL ("}"),E_MISSING_RIGHT_BRACE,CFunctions::DecLevel),		
						_EW(NULL)), 
	NULL));
	////--------------- 43 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Блок составной инструкции",			NULL,
		CSntxVariant(	GE("Список декл-инстр"),
/*OPTIONAL*/			GE("Блок составной инструкции",NULL,R_OPTIONAL),
						_EW(NULL)),
	NULL));
	////--------------- 44
	SA.AddRule(CSntxRule("Список декл-инстр",					NULL,
		CSntxVariant(	GE("Список деклараций"),
						GE("Список инструкций"),
						_EW(NULL)),
		CSntxVariant(	GE("Список деклараций"),
						_EW(NULL)),
		CSntxVariant(	GE("Список инструкций"),
						_EW(NULL)),
	NULL));

	////--------------- 45 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список инструкций",					NULL,
		CSntxVariant(	GE("Инструкция"),
/*OPTIONAL*/			GE("Список инструкций",NULL,R_OPTIONAL),
						_EW(NULL)),
	NULL));
	////--------------- 46
	SA.AddRule(CSntxRule("Инструкция выбора",					NULL,
		CSntxVariant(	GE(FL ("if"),NULL),
						GE(FL ("("),_EW(E_MISSING_LEFT_BRACKET)),
						GE("Выражение"),
						GE(FL (")"),_EW(E_MISSING_RIGHT_BRACKET),CFunctions::FormIF), 
						GE("Инструкция"),
						GE(FL ("else"),CFunctions::FormTHEN_ELSE),
						GE("Инструкция",CFunctions::FormELSE),							
						_EW(NULL)),
		CSntxVariant(	GE(FL ("if"),NULL),
						GE(FL ("("),_EW(E_MISSING_LEFT_BRACKET)),
						GE("Выражение"),
						GE(FL (")"),_EW(E_MISSING_RIGHT_BRACKET),CFunctions::FormIF), 
						GE("Инструкция"/*	CFunctions::FormTHEN*/),					
						_EW(NULL),						
						CFunctions::FormTHEN), 
	NULL));

	////--------------- 47 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Выражение",							NULL,				
		CSntxVariant(	GE("Выражение присваивания"),
/*OPTIONAL*/			GE(FL(","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("Выражение",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)), 
	NULL));
	
	////--------------- 48
	SA.AddRule(CSntxRule("Выражение присваивания",				NULL,
		CSntxVariant(	GE("Унарное выражение"),
						GE("Оператор присваивания"),
						GE("Выражение присваивания"),
						_EW(NULL),					
						CFunctions::FormASSIGN), 
		CSntxVariant(	GE("Условное выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 49
	SA.AddRule(CSntxRule("Оператор присваивания",				NULL,
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
	SA.AddRule(CSntxRule("Условное выражение",					NULL,
//		CSntxVariant(	GE("Логич ИЛИ выражение"),GE(FL ("?")),GE("Выражение").GE(FL (":")),GE("Условное выражение"),NULL), 
//						_EW(NULL)), 
		CSntxVariant(	GE("Логич ИЛИ выражение"),
						_EW(NULL)), 

	NULL));
	
	////--------------- 51
	SA.AddRule(CSntxRule("Логич ИЛИ выражение",				NULL,
		CSntxVariant(	GE("Логич И выражение"),
						GE(FL ("||"),NULL),
						GE("Логич ИЛИ выражение"),
						_EW(NULL),				
						CFunctions::FormL_OR), 
		CSntxVariant(	GE("Логич И выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 52
	SA.AddRule(CSntxRule("Логич И выражение",					NULL,
		CSntxVariant(	GE("ИЛИ выражение"),
						GE(FL ("&&"),NULL),
						GE("Логич И выражение"),
						_EW(NULL),				
						CFunctions::FormL_AND), 
		CSntxVariant(	GE("ИЛИ выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 53
	SA.AddRule(CSntxRule("ИЛИ выражение",						NULL,
		CSntxVariant(	GE("Искл ИЛИ выражение"),
						GE(FL ("|"),NULL),
						GE("ИЛИ выражение"),
						_EW(NULL),					
						CFunctions::FormOR), 
		CSntxVariant(	GE("Искл ИЛИ выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 54
	SA.AddRule(CSntxRule("Искл ИЛИ выражение",				NULL,
		CSntxVariant(	GE("И выражение"),
						GE(FL ("^"),NULL),
						GE("Искл ИЛИ выражение"),
						_EW(NULL),					
						CFunctions::FormXOR), 
		CSntxVariant(	GE("И выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 55
	SA.AddRule(CSntxRule("И выражение",						NULL,
		CSntxVariant(	GE("Выражение равентсва"),
						GE(FL ("&"),NULL),
						GE("И выражение"),		
						_EW(NULL),					
						CFunctions::FormAND), 
		CSntxVariant(	GE("Выражение равентсва"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 56
	SA.AddRule(CSntxRule("Выражение равентсва",				NULL,
		CSntxVariant(	GE("Выражение отношения"),
						GE(FL ("=="),NULL),
						GE("Выражение равентсва"),
						_EW(NULL),							
						CFunctions::FormEQ), 
		CSntxVariant(	GE("Выражение отношения"),
						GE(FL ("!="),NULL),
						GE("Выражение равентсва"),
						_EW(NULL),							
						CFunctions::FormINEQ), 
		CSntxVariant(	GE("Выражение отношения"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 57
	SA.AddRule(CSntxRule("Выражение отношения",				NULL,
		CSntxVariant(	GE("Сдвиговое выражение"),
						GE(FL ("<"),NULL),
						GE("Выражение отношения"),
						_EW(NULL),							
						CFunctions::FormLESS), 
		CSntxVariant(	GE("Сдвиговое выражение"),
						GE(FL (">"),NULL),
						GE("Выражение отношения"),
						_EW(NULL),							
						CFunctions::FormMORE), 
		CSntxVariant(	GE("Сдвиговое выражение"),
						GE(FL ("<="),NULL),
						GE("Выражение отношения"),
						_EW(NULL),							
						CFunctions::FormLESS_EQ), 
		CSntxVariant(	GE("Сдвиговое выражение"),
						GE(FL (">="),NULL),
						GE("Выражение отношения"),
						_EW(NULL),
						CFunctions::FormMORE_EQ), 
		CSntxVariant(	GE("Сдвиговое выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 58
	SA.AddRule(CSntxRule("Сдвиговое выражение",				NULL,
		CSntxVariant(	GE("Аддитивное выражение"),
						GE(FL ("<<"),NULL),
						GE("Сдвиговое выражение"),
						_EW(NULL),							
						CFunctions::FormSHL), 
		CSntxVariant(	GE("Аддитивное выражение"),
						GE(FL (">>"),NULL),
						GE("Сдвиговое выражение"),	
						_EW(NULL),							
						CFunctions::FormSHR), 
		CSntxVariant(	GE("Аддитивное выражение"),	
						_EW(NULL)), 
	NULL));
	
	////--------------- 59
	SA.AddRule(CSntxRule("Аддитивное выражение",				NULL,
		CSntxVariant(	GE("Мультипликативное выражение"),
							GE(FL ("+" ),NULL),
							GE("Аддитивное выражение"),	
						_EW(NULL),							
						CFunctions::FormADD), 
		CSntxVariant(	GE("Мультипликативное выражение"),
							GE(FL ("-" ),NULL),
							GE("Аддитивное выражение"),	
						_EW(NULL),							
						CFunctions::FormSUB), 
		CSntxVariant(	GE("Мультипликативное выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 60
	SA.AddRule(CSntxRule("Мультипликативное выражение",		NULL,
		CSntxVariant(	GE("Приведение к типу"),
						GE(FL ("*" ),NULL),
						GE("Мультипликативное выражение"),
						_EW(NULL),							
						CFunctions::FormMUL), 
		CSntxVariant(	GE("Приведение к типу"),
						GE(FL ("/"),NULL),
						GE("Мультипликативное выражение"),
						_EW(NULL),							
						CFunctions::FormDIV), 
		CSntxVariant(	GE("Приведение к типу"),
						GE(FL ("%"),NULL),
						GE("Мультипликативное выражение"),
						_EW(NULL),							
						CFunctions::FormRMNDR), 
		CSntxVariant(	GE("Приведение к типу"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 61
	SA.AddRule(CSntxRule("Приведение к типу",					NULL,
//		CSntxVariant(	GE(FL ("(" )),GE("Имя типа"),GE(FL (")" )),GE("Приведение к типу"),	NULL), 
//						_EW(NULL)), 
		CSntxVariant(	GE("Унарное выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 62
	SA.AddRule(CSntxRule("Унарное выражение",					NULL,
//		CSntxVariant(	GE(FL ("++" )),GE("Унарное выражение"),
//						_EW(NULL)), 
//		CSntxVariant(	GE(FL ("--" )),GE("Унарное выражение"),
//						_EW(NULL)), 
		CSntxVariant(	GE(FL ("+" )),
						GE("Приведение к типу"),	
						_EW(NULL)), 
		CSntxVariant(	GE(FL ("-" )),														
						GE("Приведение к типу"),	
						_EW(NULL), 
						CFunctions::FormNEG),
		CSntxVariant(	GE(FL ("!"),NULL),														
						GE("Приведение к типу"),	
						_EW(NULL),				
						CFunctions::FormNOT),
		CSntxVariant(	GE(FL ("~" ),NULL),
						GE("Приведение к типу"),
						_EW(NULL)), 
		CSntxVariant(	GE("Постфиксное выражение"),
						_EW(NULL)), 
	NULL));
	
	////--------------- 63
	SA.AddRule(CSntxRule("Постфиксное выражение",				NULL,
// Вызов функции должен стоять первым, иначе если поставить первым идекс массива, то его вариант "Имя переменной" вводит в неоднозначность - нет разделяющего терминала
		CSntxVariant(	GE(FL("sizeof"),NULL),
						GE(FL ("(")),
						GE("Спецификатор типа или идентификатор"),
						GE(FL (")")),
						_EW(NULL)), 
		CSntxVariant(	GE(FL("size"),NULL),												
						GE(FL ("(")),
						GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL (")")),					
						_EW(NULL),	
						CFunctions::FormSIZEA),
		CSntxVariant(	GE("Вызов функции"),						
						GE(FL ("."),CFunctions::EnterToStruct),
						GE("Постфиксное выражение"),	
						_EW(NULL),
						CFunctions::ExitFromStruct), 
		CSntxVariant(	GE("Вызов функции"),			
						_EW(NULL)), 
		CSntxVariant(	GE("Индекс массива"),
						GE(FL ("."),CFunctions::EnterToStruct),
						GE("Постфиксное выражение"),	
						_EW(NULL),
						CFunctions::ExitFromStruct), 
		CSntxVariant(	GE("Индекс массива"),
						GE(FL ("++" ),NULL),
						_EW(NULL),						
						CFunctions::FormINC_POST), 
		CSntxVariant(	GE("Индекс массива"),
						GE(FL ("--" ),NULL),
						_EW(NULL),						
						CFunctions::FormDEC_POST), 
		CSntxVariant(	GE("Индекс массива"),	
						_EW(NULL)), 
		CSntxVariant(	GE("Первичное выражение"),
						_EW(NULL)), 		
	
	NULL));
	
	////--------------- 64
	SA.AddRule(CSntxRule("Спецификатор типа или идентификатор",NULL,
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
	SA.AddRule(CSntxRule("идентификатор",NULL,
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
	SA.AddRule(CSntxRule("Индекс массива",							NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::PushToLexemeStack),
						GE(FL ("[" ),CFunctions::ResetEnter),
						GE("Выражение"),
						GE(FL ("]" ),CFunctions::RestoreEnter),
						_EW(NULL),					
						CFunctions::FormSTORE_IDX), 
		CSntxVariant(	GE("Имя переменной"),
						_EW(NULL)), 

	NULL));
	
	////--------------- 67
	SA.AddRule(CSntxRule("Имя переменной",							NULL,
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::FormSTORE),
						_EW(NULL)), 
	NULL));
	
	////--------------- 68
	SA.AddRule(CSntxRule("Вызов функции",				NULL,
		CSntxVariant(	GE(FL("Parse"),NULL),
						GE(FL("(")),
						GE(CONST_CLASS_ID, CFunctions::PushToLexemeStack),
						GE(FL(")")),
						_EW(NULL),
						CFunctions::FormPARSE),// FormPARSE кладет в стек переменных	
		CSntxVariant(	GE(FL("SaveAllFiles"),NULL),
						GE(FL("(")),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormSAVE_FILES),// FormPARSE кладет в стек переменных	
		CSntxVariant(	GE(FL("GetText"),NULL),
						GE(FL("(")),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormGET_TEXT),// FormPARSE кладет в стек переменных	
		CSntxVariant(	GE(FL("GetFilePos"),NULL),
						GE(FL("(")),
						GE(FL(")")),														
						_EW(NULL),	
						CFunctions::FormGET_FILE_POS),// FormPARSE кладет в стек переменных	
		CSntxVariant(	GE(FL("InsertBefore"),NULL),
						GE(FL("(")),
						GE("Выражение присваивания"),
						GE(FL(",")),
						GE("Выражение присваивания"), 
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormINS_BEFORE),
		CSntxVariant(	GE(FL("InsertAfter"),NULL),
						GE(FL("(")),
						GE("Выражение присваивания"),
						GE(FL(",")),
						GE("Выражение присваивания"),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormINS_AFTER), 
		CSntxVariant(	GE(FL("IgnoreBlock"),NULL),
						GE(FL("(")),
						GE("Выражение присваивания"),
						GE(FL(",")),
						GE("Выражение присваивания"),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormIGNORE_BLOCK),
		CSntxVariant(	GE(FL("ReplaceBlock"),NULL),
						GE(FL("(")),
						GE("Выражение присваивания"),
						GE(FL(",")),
						GE("Выражение присваивания"),
						GE(FL(")")),
						_EW(NULL),		
						CFunctions::FormREPLACE_BLOCK),
		CSntxVariant(	GE(FL("print"),NULL),
						GE(FL("(")),
						GE("Список аргументов print"),
						GE(FL(")")),
						_EW(NULL)),
		// Не забывать, что перед R_FORBID_BACK PERMIT_BACK идет указатель на функцию 
		// Если она не нужна, ставить NULL
		CSntxVariant(	GE(FL("write"),NULL),
						GE(FL("(")),
						GE("Список аргументов write"),
						GE(FL(")")),
						_EW(NULL)),			
		CSntxVariant(	GE(FL("format"),NULL),
						GE(FL("(")),
						GE("Список аргументов"),
						GE(FL(")"),CFunctions::FormFORMAT),
						_EW(NULL)),
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::SearchFunc),
						GE(FL ("(" )),
						GE(FL (")" )),
						_EW(NULL),
						CFunctions::FormCALL),	// FormCALL извлекает параметры из стека переменных и имя вызываемой функции, записанной туда SearchFunc и формирует PUSH
		CSntxVariant(	GE(ID_CLASS_ID,CFunctions::SearchFunc),
						GE(FL ("(" )),
						GE("Список аргументов"),
						GE(FL (")" )),
						_EW(NULL),
						CFunctions::FormCALL),	// FormCALL извлекает параметры из стека переменных и имя вызываемой функции, записанной туда SearchFunc и формирует PUSH
	NULL));
	////--------------- 69 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список аргументов",			NULL,
		CSntxVariant(	GE("Выражение присваивания"),
/*OPTIONAL*/			GE(FL (","),NULL,R_OPTIONAL),
/**/					GE("Список аргументов",NULL,R_CHILD_OPTIONAL),
						_EW(NULL),	
						CFunctions::IncParamCount), //Наличие функции не даст оптимизатору разложить рекурсию в цикл
	NULL));

	////--------------- 70 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список аргументов print",			NULL,
		CSntxVariant(	GE("Выражение присваивания",CFunctions::FormPRINT),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("Список аргументов print",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)), 
	NULL));	
	
	////--------------- 71 (Может быть разложен оптимизатором из рекурсии в список)
	SA.AddRule(CSntxRule("Список аргументов write",			NULL,
		CSntxVariant(	GE("Выражение присваивания",CFunctions::FormWRITE),
/*OPTIONAL*/			GE(FL (","),CFunctions::FormUNSTORE,R_OPTIONAL),
/**/					GE("Список аргументов write",NULL,R_CHILD_OPTIONAL),
						_EW(NULL)),
	NULL));	
	
	////--------------- 72
	SA.AddRule(CSntxRule("Первичное выражение",	E_BAD_EXPRESSION,
		CSntxVariant(	GE(CONST_CLASS_ID),
						_EW(NULL),								
						CFunctions::FormSTORE), 
		CSntxVariant(	GE(ID_CLASS_ID),
						_EW(NULL),		
						CFunctions::FormSTORE), 
		CSntxVariant(	GE(FL ("(")),
						GE("Выражение"),
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
