// Functions1.cpp: implementation of the CFunctions class.- язык действий
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Functions.h"
#include "EWQueue.h"

#define EXTERN_FUNC 1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

stack_ex<structs::lexeme>				CFunctions::m_LexemeStack;
stack_ex<const structs::type*>	CFunctions::m_TypeStack;
stack_ex<structs::type*>		CFunctions::m_EnterStack;
stack_ex<size_t>				CFunctions::m_IFStack;
list<string_smart>				CFunctions::m_StringList;
structs::lexeme			CFunctions::RightOP;
structs::lexeme			CFunctions::LeftOP;
SSntxID			CFunctions::LeftID;
SSntxID			CFunctions::RightID;
structs::type*	CFunctions::m_pType;
size_t			CFunctions::m_nLabelCounter=0;
size_t			CFunctions::m_nParametersCount=0;
bool			CFunctions::m_bInFunction=false;
bool			CFunctions::m_bInSubRoutine=false;
bool			CFunctions::m_bInStruct=false;
size_t			CFunctions::m_nCurrentFuncProps=0;
string_smart	CFunctions::m_strAssignOperation;
string_smart	CFunctions::m_strCurrentFuncName;
CFunctionsTable CFunctions::m_FuncTable;

EStdTerminalClass CFunctions::CurrentTermType=NO_STD_TERMINAL;

CFunctions::CFunctions()
{

}

CFunctions::~CFunctions()
{
}

CFunctions::Clear()
{
	m_FuncTable.Clear(); //Возможно, этого делать и не надо, надо посмотреть, когда дойдет дело до многомодульности
						 //m_nLabelCounter не очищаем
	m_nParametersCount=0;
	m_bInFunction=false;
	m_bInSubRoutine=false;
	m_bInStruct=false;
	m_nCurrentFuncProps=0;
	m_strAssignOperation.erase();
	m_strCurrentFuncName.erase();
	CurrentTermType=NO_STD_TERMINAL;
	
	m_LexemeStack.clear();
	m_TypeStack.clear();
	m_EnterStack.clear();
	m_IFStack.clear();
}
//////////////////////////////////////////////////////
// -------------- Type Casting -----------------------
//////////////////////////////////////////////////////

inline bool CFunctions::SimpleTypeCast( const structs::object::type & aType1,const structs::object::type & aType2, OUT structs::object::type & aType3)
{
	// если хотя бы один оператор - строка, результат тоже будет строкой
	if ((aType1 == structs::object::vtSTRING) || (aType2 == structs::object::vtSTRING)) {
		aType3=structs::object::vtSTRING;
		return true;
	}
	// если хотя бы один оператор-вещественный
	else if ((aType1 == structs::object::vtDOUBLE) || (aType2 == structs::object::vtDOUBLE)){
		aType3=structs::object::vtDOUBLE;
		return true;
	}
	//Последняя комбинация - оба типа целочисленных
	else if ((structs::object::isordinal(aType1)) && (structs::object::isordinal(aType2))){
		if (structs::object::size_of(aType2)>structs::object::size_of(aType1))	aType3=aType2;
		else													aType3=aType1;
		return true;
	}

	_ASSERT(false);
	return false;
}
//------------------------------------------------------------------------
inline bool CFunctions::SimpleTypeCast(const structs::lexeme & Lexeme1, const structs::lexeme & Lexeme2,OUT structs::object::type & aType3)
{
	_ASSERT(Lexeme1.pType);
	_ASSERT(Lexeme2.pType);

	if (Lexeme1.pType->IsStruct()) {
		errors::e_operand_must_be_simple_type(Lexeme1,0);
		return false;
	}

	if (Lexeme2.pType->IsStruct()) {
		errors::e_operand_must_be_simple_type(Lexeme2,0);
		return false;
	}

	structs::object::type aType1 =Lexeme1.pType->GetType();
	structs::object::type aType2 =Lexeme2.pType->GetType();

	return SimpleTypeCast(aType1,aType2,aType3);
}
//------------------------------------------------------------------------
bool CFunctions::GetSimpleType(const structs::lexeme & Lexeme, OUT structs::object::type & aType)
{
	_ASSERT(Lexeme.pType); // лексема должна прийти уже с типом
	if (Lexeme.pType->IsStruct()){
		errors::e_operand_must_be_simple_type(Lexeme,0);
		return false;
	}
	aType=Lexeme.pType->GetType();
	return true;
}
//------------------------------------------------------------------------
inline bool CFunctions::CheckOnGoodTypeCast(const structs::lexeme & LeftOP, const structs::lexeme & RightOP)
{
	// Сюда лексемы должны прийти уже с типом
	// LeftOP - должна быть переменной
	_ASSERT(LeftOP.pType);
	_ASSERT(RightOP.pType);

	if ((LeftOP.pType->GetMemSize()==0)	|| 
		(RightOP.pType->GetMemSize()==0))
	{
		errors::e_bad_type_cast(LeftOP,RightOP);
		return false;
	}

	//Правый операнд должен быть одного из следующих типов
	_ASSERT (oneof(RightOP.GetClass(),
				ID_CLASS_ID,
				TEMPORARY_ID_CLASS_ID,
				TEMPORARY_CONST_CLASS_ID,
				CONST_CLASS_ID));

	// непростые типы можно присваивать только при их совпадении
	if ((LeftOP.pType->IsStruct()) && (LeftOP.pType!=RightOP.pType)) 
	{
		errors::e_bad_type_cast(LeftOP,RightOP);
		return false;
	}

	//Если это были структуры, то выходим
	if (LeftOP.pType->IsStruct())
		return true;

	if (LeftOP.pType->IsOrdinal())	//Целочисленный
	{
		//Преобразование из вещественного в целочисленный
		if (RightOP.pType->IsReal())
			errors::w_truncated_assignment(RightOP,LeftOP);
		//Преобразование из целочисленного в челочисленный меньшего размера
		else if (RightOP.pType->IsOrdinal())
		{
			if (RightOP.pType->GetMemSize()>LeftOP.pType->GetMemSize())
				errors::w_truncated_assignment(RightOP,LeftOP);
		}
		else if (RightOP.pType->IsString()) // идет автоматическое приведение из типа string
		{
			errors::w_strange_type_cast(RightOP,LeftOP);
		}
		else 
			_ASSERT(0);
	}
	else if (LeftOP.pType->IsReal()) // к типу float приводятся все типы, в т.ч. string
	{
		if (RightOP.pType->IsString()) // идет автоматическое приведение из типа string
			errors::w_strange_type_cast(RightOP,LeftOP);
	}	
	else if (LeftOP.pType->IsString()) // при различных типах идет автоматическое приведение с помощью itoa, gcvt ...
	{
		if (!(RightOP.pType->IsString())) 
			errors::w_strange_type_cast(RightOP,LeftOP);
	}
	else if (LeftOP.pType->IsVariant()) 
	{
	}
	else
		_ASSERT(0);// Структура, массив 

	return true;
}

//////////////////////////////////////////////////////
// -------------- Stack Operations -------------------
//////////////////////////////////////////////////////

ret_t CFunctions::CheckStacks( const ISmntAnalyzeContext & Context)
{
	_ASSERT(m_LexemeStack.empty());
	_ASSERT(m_TypeStack.empty());
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::PushIntoGrammsStack( structs::lexeme  &Lexeme)
{
	m_GrammsStack.push(Lexeme);		
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::PopFromGrammsStack( structs::lexeme & Lexeme)
{
	_ASSERT(!m_GrammsStack.empty());
	Lexeme=m_GrammsStack.pop_val();
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::PushToLexemeStack( const ISmntAnalyzeContext & Context)
{
	return PushToLexemeStack(Context.GetLexeme());
}
//------------------------------------------------------------------------
bool CFunctions::PushToLexemeStack(const ISmntAnalyzeContext & Context, structs::object::type VarType)
{
	structs::lexeme Lexeme(TEMPORARY_CONST_CLASS_ID,0,STR_NULL);	
	if (!Context.GetTypeTable().FindStdType(VarType,Lexeme.pType))
		_ASSERT(false);

	Lexeme.SetLocation(Context.GetLexeme().GetLocation());
	return PushToLexemeStack(Lexeme);
}
//------------------------------------------------------------------------
bool CFunctions::PushToLexemeStack( const structs::lexeme  &Lexeme)
{
	if (oneof(Lexeme.GetClass(),CONST_CLASS_ID,TEMPORARY_CONST_CLASS_ID))
		_ASSERT(Lexeme.pType);
	m_LexemeStack.push(Lexeme);		
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CopyLexemeStackHead(const ISmntAnalyzeContext & Context)
{
	m_LexemeStack.push(m_LexemeStack.top());		
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::ClearLexemeStack()
{
	while (!m_LexemeStack.empty())
		m_LexemeStack.pop();
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::PopFromLexemeStack( structs::lexeme & Lexeme)
{
	_ASSERT(!m_LexemeStack.empty());
	Lexeme=m_LexemeStack.pop_val(); 
	if (oneof(Lexeme.GetClass(),CONST_CLASS_ID,TEMPORARY_CONST_CLASS_ID))
		_ASSERT(Lexeme.pType);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::DeleteFromLexemeStack( const ISmntAnalyzeContext & Context)
{
	_ASSERT(!m_LexemeStack.empty());
	m_LexemeStack.pop(); 
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::PushToTypeStack( const ISmntAnalyzeContext & Context)
{
 	_ASSERT(Context.GetLexeme().GetSzText());
	structs::type * pType;
	if (Context.GetTypeTable().FindType(Context.GetLexeme().GetSzText(),pType)==false)	
	{
 		errors::e_symbol_undefined(Context.GetLexeme(),0);
		return false;
	}
	m_TypeStack.push(pType);		
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::PushIntoTypeStack(const structs::type *pType)
{
	m_TypeStack.push(pType);		
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::PopFromTypeStack(const structs::type *& pType)
{
	_ASSERT(!m_TypeStack.empty());
	pType=m_TypeStack.pop_val(); 
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::DeleteFromTypeStack( const ISmntAnalyzeContext & Context)
{
	_ASSERT(!m_TypeStack.empty());
	m_TypeStack.pop(); 
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::PushToEnterStack( structs::type * pType)
{
	m_EnterStack.push(pType);		
	return true;
}
//------------------------------------------------------------------------
bool CFunctions::PopFromEnterStack( structs::type * & pType)
{
	pType=m_EnterStack.pop_val();		
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::ResetEnter( const ISmntAnalyzeContext & Context)
{
	PushToEnterStack(Context.GetSntxTable().GetStructCurrent());
	Context.GetSntxTable().ClearStructCurrent();
	m_bInStruct=false;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateCodeFragment( const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().CreateNewFragment();
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::DestroyCodeFragment( const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().DestroyFragment(false);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::AppendCodeFragment( const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().DestroyFragment(true);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::RestoreEnter( const ISmntAnalyzeContext & Context)
{
	structs::type * pType; PopFromEnterStack(pType);
	Context.GetSntxTable().SetStructCurrent(pType);
	if (Context.GetSntxTable().GetStructCurrent())
		m_bInStruct=true;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateAssignOperation( const ISmntAnalyzeContext & Context)
{
	m_strAssignOperation=Context.GetLexeme().GetSzText();
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::SearchFunc ( const ISmntAnalyzeContext & Context)
{
	_ASSERT(m_nParametersCount==0);	// Если это не так, кто-то их там забыл
	LeftOP=Context.GetLexeme();
	RightOP=LeftOP; //Здесь для нас интересна позиция в тексте
	if (!m_FuncTable.FindFunc(LeftOP.GetSzText(),LeftOP.Index))
	{
		//Попытаемся найти в импортируемых библиотеках
		structs::func_data fdata;
		size_t nFuncID;
		switch (Context.GetImportTable().FindFunction(LeftOP.GetSzText(),fdata,nFuncID))
		{
		case IImportTable::ffrHasNoEntryPoint:
			errors::e_function_has_no_entry_point(LeftOP,0);
			return false;
		case IImportTable::ffrHasNoFunction:
			errors::e_symbol_undefined(LeftOP,0);
			return false;
		case IImportTable::ffrOK: 
			break;
		default:
			_ASSERT(false);
		}
		
		//Возвращаемый тип
 		structs::object::type t= fdata.m_RetValue.m_VarType;
		structs::type * pType;
		Context.GetTypeTable().FindStdType(t,pType);
		_ASSERT(pType);
		m_FuncTable.CreateFunction(LeftOP.GetSzText(),pType,LeftOP.Index);

		//Аргументы
		for (size_t i=0; i<fdata.m_nArgCount;i++) {
			t= fdata.m_Args[i].m_VarType;
			Context.GetTypeTable().FindStdType(t,pType);
			_ASSERT(pType);

			RightOP.Set(Format("arg%d",i).c_str());
			if (!m_FuncTable.AddParamToLastFunction(SSntxID(pType,RightOP,1,
				(fdata.m_Args[i].m_PassType==grm_structs::pt_byref) ? BY_REF : NO_PROP))) 
				_ASSERT(false);
		}
		//Добавляем функцию в код, задом наперед
		Context.GetPCodeList().InsertFront(structs::code_cmd(ENDF));
		Context.GetPCodeList().InsertFront(structs::code_cmd(RET));
		
		//Перевызов
		structs::lexeme FuncID(DIRECT_NUMBER_CLASS_ID,nFuncID);
		Context.GetPCodeList().InsertFront(structs::code_cmd(FuncID,RECALL_EXTERN));

		LeftOP.SetClass(FUNC_CLASS_ID);
		Context.GetPCodeList().InsertFront(structs::code_cmd(LeftOP,FUNC));
	}
	PushIntoTypeStack(m_FuncTable.GetRetType(LeftOP.Index));	//Тип возвращаемого значения
	PushToLexemeStack(LeftOP); // Кладем это для CALL
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateFunc ( const ISmntAnalyzeContext & Context)
{
	_ASSERT(Context.GetLexeme().GetClass()==ID_CLASS_ID);
	PopFromTypeStack(m_pType);
	LeftOP=Context.GetLexeme();
	//											
	if (m_FuncTable.CreateFunction(Context.GetLexeme().GetSzText(),m_pType,LeftOP.Index)!=true)
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),Context.GetLexeme());
		return false;
	}
	m_nCurrentFuncProps=0;
	m_strCurrentFuncName=Context.GetLexeme().GetSzText();

	PushIntoTypeStack(m_pType);
	LeftOP.SetClass(FUNC_CLASS_ID);
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,FUNC));
	_ASSERT(!(m_bInFunction||m_bInSubRoutine));
	m_bInFunction=true;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::AddToFuncByVal ( const ISmntAnalyzeContext & Context)
{
	_ASSERT(Context.GetLexeme().GetClass()==ID_CLASS_ID);
	PopFromTypeStack(m_pType);
	LeftOP=Context.GetLexeme();				
	if (m_FuncTable.AddParamToLastFunction(SSntxID(m_pType,LeftOP))!=true)
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),Context.GetLexeme());
		return false;
	}
	
	RightOP.SetClass(DIRECT_NUMBER_CLASS_ID);
	if (Context.GetSntxTable().Find(LeftOP.GetSzText(),LeftID,RightOP.Index,LeftOP.Index)!=true) 
	{
		errors::e_symbol_undefined(LeftOP,0);
		return false;
	}

	Context.GetPCodeList().Add(structs::code_cmd(RightOP,LeftOP,POP));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::AddToFuncByRef ( const ISmntAnalyzeContext & Context)
{
	_ASSERT(Context.GetLexeme().GetClass()==ID_CLASS_ID);
	PopFromTypeStack(m_pType);
	LeftOP=Context.GetLexeme();
														
	if (m_FuncTable.AddParamToLastFunction(SSntxID(m_pType,LeftOP,1,BY_REF))!=true)
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),Context.GetLexeme());
		return false;
	}
	
	RightOP.SetClass(DIRECT_NUMBER_CLASS_ID);
	if (Context.GetSntxTable().Find(LeftOP.GetSzText(),LeftID,RightOP.Index,LeftOP.Index)!=true) 
	{
		errors::e_symbol_undefined(LeftOP,0);
		return false;
	}

	Context.GetPCodeList().Add(structs::code_cmd(RightOP,LeftOP,POP_ALIAS));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::CreateStruct (const ISmntAnalyzeContext & Context)
{
	_ASSERT(Context.GetLexeme().GetClass()==ID_CLASS_ID);

	if (Context.GetTypeTable().CreateStructure(Context.GetLexeme().GetSzText())!=true)
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),Context.GetLexeme());
		return false;
	}

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::AddToStruct (const ISmntAnalyzeContext & Context)
{
	PopFromTypeStack(m_pType);
	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.GetClass()==ID_CLASS_ID);

	if (Context.GetTypeTable().AddFieldToLastStructure(SSntxID(m_pType,LeftOP))!=true)
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return false;
	}
	PushIntoTypeStack(m_pType);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::AddToStructArray (const ISmntAnalyzeContext & Context)
{
	size_t nSize;
	PopFromLexemeStack(LeftOP);					// здесь лежит размерность массива. 
	
	if (LeftOP.GetClass()!=CONST_CLASS_ID){
		errors::general_syntax_error((E_NEED_CONST_EXP)); 
		return false;
	}
	
	if (!LeftOP.pType->IsOrdinal())
		errors::w_strange_use_of_type(LeftOP,0); 
	
	nSize=atoi(LeftOP.GetSzText());

	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.GetClass()==ID_CLASS_ID);
	if (nSize==0){
		errors::general_syntax_error((E_NULL_SIZE_OF_ARRAY), LeftOP); 
		return false;
	}

	PopFromTypeStack(m_pType);
	if (Context.GetTypeTable().AddFieldToLastStructure(SSntxID(m_pType,LeftOP,nSize,NO_PROP))!=true){
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return false;
	}

	PushIntoTypeStack(m_pType);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::EnterToStruct( const ISmntAnalyzeContext & Context)
{

	_ASSERT(!m_LexemeStack.empty()); 
	PopFromLexemeStack(LeftOP);	// Имя переменной, куда мы заходим
	size_t Level;

	switch (LeftOP.GetClass())
	{
	case ID_CLASS_ID:
		if (!Context.GetSntxTable().Find(LeftOP.GetSzText(),LeftID,Level,LeftOP.Index))
		{
			errors::e_symbol_undefined(LeftOP,0);
			return false;
		}	

		if (LeftID.pType->IsSimple())
		{
			errors::general_syntax_error((E_UNSTRUCTURE_VARIABLE),LeftOP);
			return false;
		}
		Context.GetSntxTable().SetStructCurrent(LeftID.pType);
		//if (m_bInStruct)
			//Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(NO_CLASS,Level),LeftOP,ENTER));				
		//else
		//	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(DIRECT_NUMBER_CLASS_ID,Level),LeftOP,ENTER));	

		break;
	case TEMPORARY_ID_CLASS_ID: // ENTER Y0; - rtv-nj создается временная переменная y0, и мы в нее входим
		Context.GetSntxTable().SetStructCurrent(LeftOP.pType);
		break;
	default:
		_ASSERT(0);

	}
	m_bInStruct=true;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::ExitFromStruct( const ISmntAnalyzeContext & Context)
{
	m_bInStruct=false;
	return Context.GetSntxTable().ClearStructCurrent();
}
//------------------------------------------------------------------------
ret_t CFunctions::IncLevel	( const ISmntAnalyzeContext & Context)
{
	Context.GetSntxTable().IncLevel();	
	Context.GetPCodeList().Add(structs::code_cmd(NEW_LEVEL));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::DecLevel	( const ISmntAnalyzeContext & Context)
{
	if (!(m_bInFunction && (m_nCurrentFuncProps & EXTERN_FUNC)))
		if (Context.GetSntxTable().GetUnreferencedFirst(LeftID))
		{
			errors::w_no_references(LeftID,0);
			while (Context.GetSntxTable().GetUnreferencedNext(LeftID))
				errors::w_no_references(LeftID,0);
		}

	Context.GetSntxTable().DecLevel();	
	Context.GetPCodeList().Add(structs::code_cmd(EXIT_LEVEL));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::IncParamCount( const ISmntAnalyzeContext & Context)
{
	m_nParametersCount++;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSTORE(const ISmntAnalyzeContext & Context)
{
	static size_t Level;
	static structs::lexeme Lex1,Lex2;

	Lex2=Context.GetLexeme();
	Lex1.Clear();
	switch (Lex2.GetClass())
	{
		case ID_CLASS_ID:

			if (Context.GetSntxTable().Find(Context.GetLexeme().GetSzText(),LeftID,Level,Lex2.Index)!=true) 
			{
				errors::e_symbol_undefined(Lex2,0);
				return false;
			}
		
			if (m_bInStruct)				// в случае структуры уровень вложенности нам не нежен
				Lex1.SetClass(NO_CLASS);
			else
			{
				Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
				Lex1.Index=Level;
			}
			Lex2.pType=LeftID.pType;
			break;
		case CONST_CLASS_ID:
			_ASSERT(m_bInStruct==false);
			Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
			break;
		default:
			_ASSERT(0);
	}
	Context.GetPCodeList().Add(structs::code_cmd(Lex1,Lex2,STORE));
	PushToLexemeStack(Lex2);

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormUNSTORE(const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(UNSTORE));
	_ASSERT(!m_LexemeStack.empty());// В стеке может быть максимум 1 элемент к этому моменту (невостребованный операнд)
	DeleteFromLexemeStack(Context);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormRESTORE(const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(RESTORE));
	_ASSERT(m_LexemeStack.size()<2);// В стеке может быть максимум 1 элемент к этому моменту (невостребованный операнд)
	ClearLexemeStack();
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormPUSH	( const ISmntAnalyzeContext & Context)
{
	_ASSERT(0); // функция не работает
	static size_t Level;
	static structs::lexeme Lex1,Lex2;

	Lex2=Context.GetLexeme();
	Lex1.Clear();
	
	switch (Lex2.GetClass())
	{
	
		case ID_CLASS_ID:
			if (Context.GetSntxTable().Find(Context.GetLexeme().GetSzText(),LeftID,Level,Lex2.Index)!=true) 
			{
				errors::e_symbol_undefined(Context.GetLexeme(),0);
				return false;
			}
			if (m_bInStruct)				// в случае структуры уровень вложенности нам не нежен
				Lex1.SetClass(NO_CLASS);
			else
			{
				Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
				Lex1.Index=Level;
			}
			Lex2.pType=LeftID.pType;
			break;
		case CONST_CLASS_ID:
			_ASSERT(m_bInStruct==false);
			Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
			break;
		default:
			_ASSERT(0);
	}
	Context.GetPCodeList().Add(structs::code_cmd(Lex1,Lex2,PUSH));
//	PushToLexemeStack(Lex2);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormPOP	( const ISmntAnalyzeContext & Context)
{
	_ASSERT(0); // функция не работает
	static size_t Level;
	static structs::lexeme Lex1,Lex2;

	Lex2=Context.GetLexeme();
	Lex1.Clear();
	
	switch (Lex2.GetClass())
	{
	
		case ID_CLASS_ID:
			if (Context.GetSntxTable().Find(Context.GetLexeme().GetSzText(),LeftID,Level,Lex2.Index)!=true) 
			{
				errors::e_symbol_undefined(Context.GetLexeme(),0);
				return false;
			}
			if (m_bInStruct)				// в случае структуры уровень вложенности нам не нежен
				Lex1.SetClass(NO_CLASS);
			else
			{
				Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
				Lex1.Index=Level;
			}
			Lex2.pType=LeftID.pType;
			break;
		case CONST_CLASS_ID:
			_ASSERT(m_bInStruct==false);
			Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
			break;
		default:
			_ASSERT(0);
	}
	Context.GetPCodeList().Add(structs::code_cmd(Lex1,Lex2,POP));
	DeleteFromLexemeStack(Context);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormFORMAT( const ISmntAnalyzeContext & Context)
{
	static structs::lexeme Lex; // нужна для формирования предупреждений

	for (int i=0;i<m_nParametersCount;i++)
		PopFromLexemeStack(LeftOP);

	if (m_nParametersCount==0)
	{
		errors::e_bad_param_count(LeftOP,0);
		return false;
	}
	Lex.SetClass(DIRECT_NUMBER_CLASS_ID);
	Lex.Index=m_nParametersCount;

	Context.GetPCodeList().Add(structs::code_cmd(Lex,FORMAT));
	PushToLexemeStack(Context,structs::object::vtSTRING);	// функция должна возвращать значение
	m_nParametersCount=0;
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormPRINT( const ISmntAnalyzeContext & Context)
{
	DeleteFromLexemeStack(Context);	// мы якобы его применили
	Context.GetPCodeList().Add(structs::code_cmd(PRINT));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormWRITE( const ISmntAnalyzeContext & Context)
{
	DeleteFromLexemeStack(Context);	// мы якобы его применили
	Context.GetPCodeList().Add(structs::code_cmd(WRITE));
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSTORE_IDX( const ISmntAnalyzeContext & Context)
{
	DeleteFromLexemeStack(Context);	// мы якобы его применили
	PopFromLexemeStack(LeftOP);
	static size_t Level;
	static structs::lexeme Lex1,Lex2;

	Lex2=LeftOP;
	Lex1.Clear();
	switch (Lex2.GetClass())
	{
		case ID_CLASS_ID:
			if (Context.GetSntxTable().Find(Lex2.GetSzText(),LeftID,Level,Lex2.Index)!=true) 
			{
				errors::e_symbol_undefined(Lex2,0);
				return false;
			}
			if (!LeftID.IsArray())
			{
				errors::e_bad_operation_with_non_array(Lex2,0);
				return false;
			}
			if (m_bInStruct)				// в случае структуры уровень вложенности нам не нежен
				Lex1.SetClass(NO_CLASS);
			else
			{
				Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
				Lex1.Index=Level;
			}
			Lex2.pType=LeftID.pType;
			break;
		default:
			_ASSERT(0);
	}

//	if (Lex1.Index==0)
//		Context.GetPCodeList().Add(structs::code_cmd(Lex1,Lex2,STORE_GLBL_IDX));
//	else
		Context.GetPCodeList().Add(structs::code_cmd(Lex1,Lex2,STORE_IDX));
	PushToLexemeStack(Lex2);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormFOR ( const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,m_nLabelCounter),LABEL));
	m_IFStack.push(m_nLabelCounter++);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormENDFOR ( const ISmntAnalyzeContext & Context)
{
	size_t Label=m_IFStack.pop_val();

	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,m_IFStack.pop_val()),JMP));
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,Label),LABEL));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormIF	( const ISmntAnalyzeContext & Context)
{
	DeleteFromLexemeStack(Context); // Здесь лежит выражение if 
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,m_nLabelCounter),GET_JZ));
	m_IFStack.push(m_nLabelCounter++);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormTHEN( const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,m_IFStack.pop_val()),LABEL));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormTHEN_ELSE( const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,m_nLabelCounter),JMP));
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,m_IFStack.pop_val()),LABEL));
	m_IFStack.push(m_nLabelCounter++);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormELSE( const ISmntAnalyzeContext & Context)
{
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(LABEL_CLASS_ID,m_IFStack.pop_val()),LABEL));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSIZEOF	( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(LeftOP);
	if (Context.GetSntxTable().Find(LeftOP.GetSzText(),LeftID,LeftOP.Index,RightOP.Index))
	{
		Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(DIRECT_NUMBER_CLASS_ID,LeftOP.Index),structs::lexeme(ID_CLASS_ID,RightOP.Index),SIZEOF));
	}
	else if (Context.GetTypeTable().FindType(LeftOP.GetSzText(),m_pType))
	{
		Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(NO_CLASS,0),structs::lexeme(TYPE_CLASS_ID,m_pType->GetTypeID()),SIZEOF));
	}
	else
	{
		errors::e_symbol_undefined(LeftOP,0);
		return false;
	}
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSIZEA	( const ISmntAnalyzeContext & Context )
{
	PopFromLexemeStack(LeftOP);
	if (Context.GetSntxTable().Find(LeftOP.GetSzText(),LeftID,LeftOP.Index,RightOP.Index))
	{
		Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(DIRECT_NUMBER_CLASS_ID,LeftOP.Index),structs::lexeme(ID_CLASS_ID,RightOP.Index),SIZEA));
	}
	else
	{
		errors::e_symbol_undefined(LeftOP,0);
		return false;
	}
	PushToLexemeStack(Context,structs::object::vtINT);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormDeclare	( const ISmntAnalyzeContext & Context)
{
	static structs::lexeme Lex1,Lex2;	
	Lex1.Clear();
	Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);
	LeftOP=Context.GetLexeme();;
	Lex2=LeftOP;

	_ASSERT(LeftOP.GetClass()==ID_CLASS_ID);
	PopFromTypeStack(m_pType);

	if (Context.GetSntxTable().RegisterID(SSntxID(m_pType,LeftOP),Lex1.Index,Lex2.Index)!=true) 
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return false;
	}
	PushIntoTypeStack(m_pType);

	Context.GetPCodeList().Add(structs::code_cmd(Lex1,Lex2,structs::lexeme(TYPE_CLASS_ID,m_pType->GetTypeID(),m_pType->GetTypeName()),NEW));

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormDeclareArray	( const ISmntAnalyzeContext & Context)
{
	static structs::lexeme Lex1,Lex2;	
	Lex1.Clear();
	Lex1.SetClass(DIRECT_NUMBER_CLASS_ID);

	PopFromLexemeStack(LeftOP);					// здесь лежит размерность массива. 
	
	_ASSERT(LeftOP.pType);

	if (!(LeftOP.pType->IsOrdinal()))
		errors::w_strange_use_of_type(LeftOP,0); 

	PopFromLexemeStack(LeftOP);
	_ASSERT(LeftOP.GetClass()==ID_CLASS_ID);
	Lex2=LeftOP;
	
	PopFromTypeStack(m_pType);
	
	if (Context.GetSntxTable().RegisterID(SSntxID(m_pType,LeftOP,0xFFFFFFFF),Lex1.Index,Lex2.Index)!=true) 
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return false;
	}
	PushIntoTypeStack(m_pType);
	Context.GetPCodeList().Add(structs::code_cmd(Lex1,Lex2,structs::lexeme(TYPE_CLASS_ID,m_pType->GetTypeID(),m_pType->GetTypeName()),NEWA));
	
	CContextMediator cm(Context,LeftOP); //Не заносить в вызов FormSTORE - пропускает проверку на abstract
	FormSTORE(cm);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormDeclareConst	( const ISmntAnalyzeContext & Context)
{
	_ASSERT(!m_LexemeStack.empty()); PopFromLexemeStack(RightOP);	
	_ASSERT(!m_LexemeStack.empty()); PopFromLexemeStack(LeftOP);
	
	if (LeftOP.GetClass()!=ID_CLASS_ID)
	{
		errors::e_l_value_required(Context.GetLexeme(),0);
		return false;
	}
	
	if (RightOP.GetClass()!=CONST_CLASS_ID)
	{
		//errors::e_l_value_required(Context.GetLexeme(),0);
		return false;
	}
	
	// Сюда лексемы должны прийти уже с типом
	_ASSERT(RightOP.pType);
	m_pType = LeftOP.pType = RightOP.pType;

	//Не доделано ReadOnly!!!!
	if (!Context.GetSntxTable().RegisterID(SSntxID(LeftOP.pType,LeftOP),RightOP.Index,LeftOP.Index))
	{
		errors::general_syntax_error((E_DUPLICATED_IDENTIFIER),LeftOP);
		return false;
	}

	//Создаем переменную
	structs::lexeme tmp(RightOP);
	tmp.SetClass(DIRECT_NUMBER_CLASS_ID);
	Context.GetPCodeList().Add(structs::code_cmd(tmp,LeftOP,structs::lexeme(TYPE_CLASS_ID,m_pType->GetTypeID(),m_pType->GetTypeName()),NEW));

	//10.04.2005. Переменная создавалась, но инициализации не было. Добавил инициализацию
	//Теперь присваиваем ей значение

	//Второй операнд
	CContextMediator cm2(Context,LeftOP);
	FormSTORE(cm2);

	//Первый операнд
	CContextMediator cm(Context,RightOP);
	FormSTORE(cm);

	//Операция присваивания
	FormASSIGN(Context);

	return true;
}
//------------------------------------------------------------------------
EAssignType CFunctions::GetASSIGNMode()
{
	if (!m_strAssignOperation.isempty())
	{
		// О наличии второго операнда уже позаботились, см. грамматики "Оператор присваивания"
		if (m_strAssignOperation.cmp("="))
			return SIMPLE_ASSIGN;
		else if	(m_strAssignOperation.cmp("+="))				
			return ADD_ASSIGN;
		else if (m_strAssignOperation.cmp("*="))
			return MUL_ASSIGN;
		else if (m_strAssignOperation.cmp("-="))
			return SUB_ASSIGN;
		else if (m_strAssignOperation.cmp("/="))
			return DIV_ASSIGN;
		else if (m_strAssignOperation.cmp("%="))
			return RMNDR_ASSIGN;
		else if (m_strAssignOperation.cmp("<<="))
			return SHL_ASSIGN;
		else if (m_strAssignOperation.cmp(">>="))
			return SHR_ASSIGN;
		else if (m_strAssignOperation.cmp("&="))
			return AND_ASSIGN;
		else if (m_strAssignOperation.cmp("^="))
			return XOR_ASSIGN;
		else if (m_strAssignOperation.cmp("|="))
			return OR_ASSIGN;
		else _ASSERT(0);
	}
	return SIMPLE_ASSIGN;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormASSIGN	( const ISmntAnalyzeContext & Context)
{
	_ASSERT(!m_strAssignOperation.isempty());
	_ASSERT(!m_LexemeStack.empty()); PopFromLexemeStack(RightOP);	
	_ASSERT(!m_LexemeStack.empty()); PopFromLexemeStack(LeftOP);
	
	if (LeftOP.GetClass()!=ID_CLASS_ID)
	{
		errors::e_l_value_required(Context.GetLexeme(),0);
		return false;
	}
	
	// Сюда лексемы должны прийти уже с типом
	_ASSERT(LeftOP.pType);
	
	CheckOnGoodTypeCast(LeftOP,RightOP);

	LeftOP.SetClass(TEMPORARY_ID_CLASS_ID);
	PushToLexemeStack(LeftOP);

	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(DIRECT_NUMBER_CLASS_ID,GetASSIGNMode(),STR_NULL),ASSIGN));
	m_strAssignOperation.clear();

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormRET( const ISmntAnalyzeContext & Context)
{
	if (m_bInSubRoutine)
	{
		Context.GetPCodeList().Add(structs::code_cmd(RET_S));
	}
	else if (m_bInFunction)
	{
		Context.GetPCodeList().Add(structs::code_cmd(RET));
	}
	else
		_ASSERT(0);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormEMPTY_RET( const ISmntAnalyzeContext & Context)
{
	if (m_bInSubRoutine)
	{
		Context.GetPCodeList().Add(structs::code_cmd(RET_S));
	}
	else if  (m_bInFunction)
	{
		PopFromTypeStack(m_pType);
		if (m_pType->GetMemSize()!=0)
		{
			Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(TYPE_CLASS_ID,m_pType->GetTypeID(),m_pType->GetTypeName()),PUSH_EMPTY));
		}
		else
			Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme(TYPE_CLASS_ID,m_pType->GetTypeID(),m_pType->GetTypeName()),PUSH_EMPTY));
		Context.GetPCodeList().Add(structs::code_cmd(RET));
		PushIntoTypeStack(m_pType);	// не мы клали - не нам и доставать
	}
	else
		_ASSERT(0);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormRECALL	( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);	// Имя внешней функции
	PopFromLexemeStack(LeftOP);		// Имя библиотеки
	
	_ASSERT (LeftOP.GetClass()==CONST_CLASS_ID);
	_ASSERT (RightOP.GetClass()==CONST_CLASS_ID);
	_ASSERT (LeftOP.pType);
	_ASSERT (RightOP.pType);

	if (!LeftOP.pType->IsString()) {
		errors::e_need_string_const(LeftOP,0);
		return false;
	}
	if (!RightOP.pType->IsString()) {
		errors::e_need_string_const(RightOP,0);
		return false;
	}
	m_nCurrentFuncProps |= EXTERN_FUNC;

	cstr szLibName = LeftOP.GetSzText();
	cstr szFuncName = RightOP.GetSzText();

	//Находим информацию о интересующей нас функции 
	CFunctionsTable::func_data fdata;
	structs::func_data imp_fdata; 
	ASSERT_CHECK(m_FuncTable.FindFunc(m_strCurrentFuncName.c_str(),fdata));

	//Экспортируем параметры в другой вид

	//Пока поддерживаются только простые параметры
	if (!fdata.m_pRetType->IsSimple()){
		errors::e_operand_must_be_simple_type(RightOP,0);
		return false;
	}
	
	imp_fdata.m_RetValue.m_PassType = grm_structs::pt_byval;
	imp_fdata.m_RetValue.m_VarType = fdata.m_pRetType->GetType();
	imp_fdata.m_nArgCount = fdata.m_lstParams.size();

	for (size_t i=0; i<fdata.m_lstParams.size();++i){
		const SSntxID & ID = fdata.m_lstParams[i];
		const structs::type * pType = ID.pType;
		_ASSERT(pType);
		//Пока поддерживаются только простые параметры
		if (!pType->IsSimple()){
			errors::e_operand_must_be_simple_type(ID,0);
			return false;
		}

		imp_fdata.m_Args[i].m_VarType = pType->GetType();
		imp_fdata.m_Args[i].m_PassType = (ID.IsByRef()) ? grm_structs::pt_byref : grm_structs::pt_byval;
	}

	size_t nFuncID;
	Context.GetImportTable().AddFunction(szLibName,szFuncName,imp_fdata,nFuncID);
	Context.GetPCodeList().Add(structs::code_cmd(structs::lexeme (DIRECT_NUMBER_CLASS_ID,nFuncID),RECALL_EXTERN));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormENDF( const ISmntAnalyzeContext & Context)
{
	m_nCurrentFuncProps=0;
	_ASSERT(m_bInFunction);
	_ASSERT(!m_bInSubRoutine);
	m_bInFunction=false;

	//Этот блок ищет в теле функции вставленные return, процедура пока недописана
	//она не учитывает ветвления, циклы и пр
	PopFromTypeStack(m_pType);// должно быть, благодаря спецификатору типа
	if (m_pType->GetMemSize()>0)
	{
		bool bRetExist=false;long i;
		for (i=Context.GetPCodeList().GetCount()-1; i>0;--i){
			if (Context.GetPCodeList().GetAt(i).Operation==FUNC)
				break;
			else if ((Context.GetPCodeList().GetAt(i).Operation==RET) && 
					(Context.GetPCodeList().GetAt(i-1).Operation!=PUSH_EMPTY))
			{
				bRetExist=true;
				break;
			}
		}
		
		if ((!bRetExist) && (i>=0) && (Context.GetPCodeList().GetAt(i).Operation==FUNC)){
			errors::w_undefined_return_value(Context.GetPCodeList().GetAt(i).Op1,0);
		}
	}
	Context.GetPCodeList().Add(structs::code_cmd(ENDF));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormRETURN(const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP); 
	if (m_bInSubRoutine)
	{
		errors::e_subroutine_has_no_ret_val(RightOP,0);
		return false;
	}
	_ASSERT(m_bInFunction);
	PopFromTypeStack(m_pType);// должно быть, благодаря спецификатору типа
	if (m_pType->GetMemSize()==0)
	{
		errors::e_function_has_no_ret_val(RightOP,0);
		return false;
	}

	LeftOP=RightOP;
	LeftOP.SetClass(ID_CLASS_ID);
	LeftOP.pType=m_pType;
	LeftOP.Set(STR_NULL);
	
	PushIntoTypeStack(m_pType);	// не мы клали - не нам и доставать
	if (!CheckOnGoodTypeCast(LeftOP,RightOP))
		return false;
	Context.GetPCodeList().Add(structs::code_cmd(PUSH));
	Context.GetPCodeList().Add(structs::code_cmd(RET));
	return true;	
}
//------------------------------------------------------------------------
ret_t CFunctions::FormCALL( const ISmntAnalyzeContext & Context)
{
	static structs::type * pType;
	static size_t PrmsCount;
	vector <structs::lexeme> TempParamList;

	// список параметров хранится задом наперед
	for (int i=0;i<m_nParametersCount;i++)
	{
		Context.GetPCodeList().Add(structs::code_cmd(PUSH));
		PopFromLexemeStack(LeftOP);

		TempParamList.push_back(LeftOP);
	}
	
	PopFromTypeStack(m_pType);	//Возвращаемыйы тип функции. эта информация любезно предоставлена функиией SearchFunc	
	PopFromLexemeStack(LeftOP); // Имя функции. эта информация любезно предоставлена функиией SearchFunc

	if (m_FuncTable.GetParamCount(LeftOP.Index)!=m_nParametersCount)
	{
		errors::e_bad_param_count(LeftOP,0);
		return false;
	}

	for (int i=0;m_nParametersCount>0;m_nParametersCount--,i++)
	{
				// LeftOP.Index уже содержит фактический индекс в таблице
		RightID = m_FuncTable.GetParam(LeftOP.Index,i);
		RightOP = TempParamList[m_nParametersCount-1];

		if ((RightID.ExtraProperties==BY_REF) && (RightOP.GetClass()!=ID_CLASS_ID))
		{
			errors::e_l_value_required(RightOP,0);
			return false;
		}

		// нужна для формирования предупрежденийLex=RightOP; 
		structs::lexeme Lex=RightOP; //Здесь мы копируем основные свойства, такие как позиция в тексте
		Lex.SetClass(ID_CLASS_ID);
		Lex.pType=RightID.pType;

		if (!CheckOnGoodTypeCast(Lex,RightOP))
			return false;
	}
	
	RightOP=LeftOP;
	RightOP.SetClass(TEMPORARY_ID_CLASS_ID);
	RightOP.pType=m_pType;

	PushToLexemeStack(RightOP); // кладем результат вызова функции
	
	LeftOP.SetClass(FUNC_CLASS_ID);
	Context.GetPCodeList().Add(structs::code_cmd(LeftOP,CALL));
//	if (pType->MemSize!=0)
		Context.GetPCodeList().Add(structs::code_cmd(POP));	
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSHR			( const ISmntAnalyzeContext & Context)	
{
	Context.GetPCodeList().Add(structs::code_cmd(SHR));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSHL			( const ISmntAnalyzeContext & Context)	
{
	Context.GetPCodeList().Add(structs::code_cmd(SHL));
	return true;
}
//------------------------------------------------------------------------
inline bool CFunctions::LogicOperationsUtilite(const ISmntAnalyzeContext & Context)
{
	_ASSERT(!m_LexemeStack.empty()); PopFromLexemeStack(RightOP);	
	_ASSERT(!m_LexemeStack.empty()); PopFromLexemeStack(LeftOP);
	
	// Типы устанавливаются FormSTORE
	_ASSERT(LeftOP.pType);
	_ASSERT(RightOP.pType);

	structs::object::type aType;
	if (GetSimpleType(LeftOP,aType)!=true)
		return false;

	PushToLexemeStack(Context,structs::object::vtBOOL);
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormEQ(const ISmntAnalyzeContext & Context)
{
	if (!LogicOperationsUtilite(Context))
		return false;
	Context.GetPCodeList().Add(structs::code_cmd(EQ));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormINEQ ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;
	Context.GetPCodeList().Add(structs::code_cmd(INEQ));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormLESS ( const ISmntAnalyzeContext & Context)	
{

	if (!LogicOperationsUtilite(Context))
		return false;
	Context.GetPCodeList().Add(structs::code_cmd(LESS));

	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormMORE ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;
	Context.GetPCodeList().Add(structs::code_cmd(MORE));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormLESS_EQ ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;
	Context.GetPCodeList().Add(structs::code_cmd(LESS_EQ));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormMORE_EQ ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;
	Context.GetPCodeList().Add(structs::code_cmd(MORE_EQ));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormL_AND ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;

	structs::object::type aType;
	ASSERT_RETURN_FALSE(GetSimpleType(LeftOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(LeftOP,0);

	ASSERT_RETURN_FALSE(GetSimpleType(RightOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(RightOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(L_AND));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormL_OR ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;

	structs::object::type aType;
	ASSERT_RETURN_FALSE(GetSimpleType(LeftOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(LeftOP,0);

	ASSERT_RETURN_FALSE(GetSimpleType(RightOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(RightOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(L_OR));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormAND ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;

	structs::object::type aType;
	ASSERT_RETURN_FALSE(GetSimpleType(LeftOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(LeftOP,0);

	ASSERT_RETURN_FALSE(GetSimpleType(RightOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(RightOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(AND));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormOR ( const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;

	structs::object::type aType;
	ASSERT_RETURN_FALSE(GetSimpleType(LeftOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(LeftOP,0);

	ASSERT_RETURN_FALSE(GetSimpleType(RightOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(RightOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(OR));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormXOR (const ISmntAnalyzeContext & Context)	
{
	if (!LogicOperationsUtilite(Context))
		return false;

	structs::object::type aType;
	ASSERT_RETURN_FALSE(GetSimpleType(LeftOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(LeftOP,0);

	ASSERT_RETURN_FALSE(GetSimpleType(RightOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(RightOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(XOR));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormADD ( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);	
	PopFromLexemeStack(LeftOP);

	structs::object::type aType;
	SimpleTypeCast(LeftOP,RightOP,aType);
	PushToLexemeStack(Context,aType);
	Context.GetPCodeList().Add(structs::code_cmd(ADD));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormSUB ( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);	
	PopFromLexemeStack(LeftOP);

	structs::object::type aType;
	SimpleTypeCast(LeftOP,RightOP,aType);
	PushToLexemeStack(Context,aType);

	Context.GetPCodeList().Add(structs::code_cmd(SUB));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormMUL ( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);	
	PopFromLexemeStack(LeftOP);

	structs::object::type aType;
	SimpleTypeCast(LeftOP,RightOP,aType);
	PushToLexemeStack(Context,aType);

	Context.GetPCodeList().Add(structs::code_cmd(MUL));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormDIV ( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);	
	PopFromLexemeStack(LeftOP);

	structs::object::type aType;
	SimpleTypeCast(LeftOP,RightOP,aType);
	PushToLexemeStack(Context,aType);

	Context.GetPCodeList().Add(structs::code_cmd(DIV));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormRMNDR( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(RightOP);	
	PopFromLexemeStack(LeftOP);

	structs::object::type aType;
	SimpleTypeCast(LeftOP,RightOP,aType);
	PushToLexemeStack(Context,aType);

	Context.GetPCodeList().Add(structs::code_cmd(RMNDR));
	return true;
}
//------------------------------------------------------------------------
ret_t CFunctions::FormNOT( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(LeftOP);				// Это то, что имеем 

	structs::object::type aType;	
	ASSERT_RETURN_FALSE(GetSimpleType(LeftOP,aType));
	if (oneof(aType,structs::object::vtSTRING,structs::object::vtDOUBLE))
		errors::w_strange_use_of_type(LeftOP,0);
	
	PushToLexemeStack(Context,structs::object::vtINT);
	Context.GetPCodeList().Add(structs::code_cmd(NOT));
	return true;
}	
//------------------------------------------------------------------------
ret_t CFunctions::FormNEG( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(LeftOP);				// Это то, что имеем 

	structs::object::type aType;	
	ASSERT_RETURN_FALSE(GetSimpleType(LeftOP,aType));
	
	if (aType == structs::object::vtSTRING)
		errors::w_strange_use_of_type(LeftOP,0);
	
	PushToLexemeStack(Context,aType);
	Context.GetPCodeList().Add(structs::code_cmd(NEG));
	return true;
}	
//------------------------------------------------------------------------
ret_t CFunctions::FormINC_POST( const ISmntAnalyzeContext & Context)
{
	CopyLexemeStackHead(Context);			// Эмулируем процесс "достал-изменил-положил обратно"
	PopFromLexemeStack(LeftOP);				// Это то, что имеем 
	if (LeftOP.GetClass()!=ID_CLASS_ID){
		errors::e_l_value_required(Context.GetLexeme(),0);
		return false;
	}
							
	if (Context.GetSntxTable().Find(LeftOP.GetSzText(),LeftID)!=true) {
		errors::e_symbol_undefined(LeftOP,0);
		return false;
	}

	structs::object::type aType;	
	if (GetSimpleType(LeftOP,aType)!=true)
		return false;
	
	if (aType==structs::object::vtSTRING)
		errors::w_strange_use_of_type(LeftOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(INC_POST));
	return true;
}	
//------------------------------------------------------------------------
ret_t CFunctions::FormDEC_POST( const ISmntAnalyzeContext & Context)
{
	PopFromLexemeStack(LeftOP);				// Это то, что имеем 
	if (LeftOP.GetClass()!=ID_CLASS_ID){
		errors::e_l_value_required(Context.GetLexeme(),0);
		return false;
	}
							
	if (Context.GetSntxTable().Find(LeftOP.GetSzText(),LeftID)!=true) {
		errors::e_symbol_undefined(LeftOP,0);
		return false;
	}

	structs::object::type aType;	
	if (GetSimpleType(LeftOP,aType)!=true)
		return false;
	
	if (aType==structs::object::vtSTRING)
		errors::w_strange_use_of_type(LeftOP,0);

	Context.GetPCodeList().Add(structs::code_cmd(DEC_POST));
	return true;
}	
