// PCode.cpp: implementation of the CPCodeList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <algorithm>

#include "PCode.h"
#include "EWQueue.h"
#include "DLLSupport.h"

using namespace out_streams;

#ifdef __DIAGNOSTIC__ 
	#define REPORT(x,y) WriteToDiagnosticFile (x,y)
#else
	#define REPORT(x,y)
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cstr CPCodeList::CommandNames[]=
{
    "NOP         ",  "PUSH        ",  "POP         ",  "EQ          ",  
    "INEQ        ",  "LESS        ",  "MORE        ",  "LESS_EQ     ", 
    "MORE_EQ     ",  "SHL         ",  "SHR         ",  "ADD         ",  
    "SUB         ",  "MUL         ",  "DIV         ",  "RMNDR       ", 
    "GET_JZ      ",  "JMP         ",  "NEW         ",  "ASSIGN      ",  
    "NEW_LEVEL   " , "EXIT_LEVEL  ",  "RET         ",
    "STORE       ",  "RESTORE     ",  "UNSTORE     ",  "ENDF        ",  
    "PUSH_EMPTY  ",  "CALL        ",  "FUNC        ",  "PRINT       ",
    "NEWA        ",  "POP_NEW     ",  "SAVE        ",  "LOAD        ",
    "STORE_IDX   ",  "HALT        ",  "LABEL       ",  "END_ISEGM   ",
    "INC         ",  "DEC         ",  "POP_ALIAS   ",  "SIZEOF      ",
    "SIZEA       ",  "SUBROUTINE  ",  "S_WORDS     ",  "I_W_SPACES  ",
    "PRMCOUNT    ",  "PRMSTR      ",  "ENDSUBR     ",  "PARSE       ",
    "RET_S       ",  "NOT         ",  "INVERT      ",  "NEG	        ",
    "L_AND       ",  "L_OR        ",  "AND         ",  "OR          ",
    "XOR         ",  "WRITE       ",  "GET_TEXT    ",  "GET_FILE_POS",
    "INS_BEFORE  ",  "INS_AFTER   ",  "I_BLOCK",  "I_CASE      ",  
	"FORMAT      ",  "REPLACE_BLOCK", "RECALL_EXTRN",  "SAVE_FILES  ",
    "RESERVED    "                                                  

};

cstr CPCodeList::CommandComments[]=
{
	/*NOP*/			"",
	/*PUSH*/		"[<влож>][<индекс>]. Для структуры 1-ый оп отсутствует. Если нет ни 1-го, ни 2-го,знач-е берется из стека переменных",
	/*POP*/			"Выталкивает элемент из стека в операнд. При отсутствии операнда кладет значение в стек переменных",
	/*EQ*/			"Берет операнды из стека. 1-ый -правый, 2-ой -левый",
	/*INEQ*/		"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*LESS*/		"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*MORE*/		"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*LESS_EQ*/		"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*MORE_EQ*/		"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*SHL*/			"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*SHR*/			"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*ADD*/			"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*SUB*/			"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*MUL*/			"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*DIV*/			"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*RMNDR*/		"Берет операнды из стека переменных. 1-ый -правый, 2-ой -левый",
	/*GET_JZ*/		"Признак перехода по элементу из стека (по 0)",
	/*JMP*/			"",
	/*NEW*/			"Выделяет новую переменную",
	/*ASSIGN*/		"Первым извлекает из стека переменных присваиваемое, затем присваиватель",
	/*NEW_LEVEL*/	"Создает новый блок и новый стек переменных",
	/*EXIT_LEVE*/	"Разрушает текущий блок и текущий стек переменных",
	/*RET*/			"Выход из функции.",
	/*STORE*/		"[<Влож>]<Индекс>Кладет в стек переменных (не PUSH!!!)Для структуры 1-ый оп отсутствует.",
	/*RESTORE*/		"Очищает стек переменных",
	/*UNSTORE*/		"Выталкивает из стека переменных",
	/*ENDF*/		"Отмечает конец функции. Нужна для компоновщика",
	/*PUSH_EMPTY*/	"Помещает в стек пустую переменную указанного типа",
	/*CALL*/		"Вызов функции",
	/*FUNC*/		"Отмечает вхождение в функцию, нужна для компоновщика",
	/*PRINT*/		"Вывод на экран. Аргументов не имеет. Работает исключительно через стек переменных",
	/*NEWA*/		"Выделение массива. Размерность массива берется из стека переменных",
	/*POP_MEW*/		"Cоздает временную переменную и скидывает в нее значение из стека, если переменная уже существует, она уничтожается",
	/*SAVE*/		"Запоминает состояние стека переменных",
	/*LOAD*/		"Восстанавливает стек переменных",
	/*STORE_IDX*/	"Индексирует массив. Размерность массива берется из стека переменных. Туда же кладется элемент вместе с индексом",
	/*HALT*/		"Завершение программы",
	/*LABEL*/		"Отмечает метку. Нужна для компоновщика",
	/*END_ISEGM*/	"Отмечает конец инициализации глобальных переменных. Устанавливается компоновщитком",
	/*INC*/			"[Уровень][Индекс] Инкрементирует переменную", 
	/*DEC*/			"[Уровень][Индекс] Декрементирует переменную",
	/*POP_ALIAS*/	"необходим при параметрах по ссылке. Адрес переменной берется из стека",
	/*SIZE_OF*/		"Кладет в стек переменнных размер типа, переменнной, массива",
	/*SIZEA*/		"Кладет в стек переменнных размер массива",
	/*SUBROUTINE*/	"Отмечает процедуру обработки правила/варианта правила/терма",
	/*S_WORDS*/		"Задает SolitaryWords",
	/*I_W_SPACES*/	"Задает IgnoreWhiteSpaces",
	/*PRMCOUNT*/	"Зaписывает в переменную количество аргументов КС",
	/*PRMSTR*/		"Зaписывает в переменную массива аргументы КС",
	/*ENDSUBR*/		"Отмечает конец процедуры обработки правила",
	/*PARSE*/		"Запускает разбор грамматик",
	/*RET_S*/		"Выход из процедуры правила.",
	/*NOT*/			"Операция НЕ",
	/*INVERT*/		"Побитовое инвертирование операнда",
	/*NEG*/			"Унарный минус",
	/*L_AND*/		"Логическое И",
	/*L_OR*/		"Логичексое ИЛИ",
	/*AND*/			"Побитовое И",
	/*OR*/			"Побитовое ИЛИ",
	/*XOR*/			"Побитовое ИСКЛ ИЛИ",
	/*WRITE*/		"Запись аргумента в файл",
	/*GET_TEXT*/	"Взять распознанный текст в текущем правиле",
	/*GET_FILE_POS*/"Взять текущий указатель в файле",
	/*INS_AFTER*/	"Вставляет оп2 в позицию в файле после оп1. Оп берутся из стека",
	/*INS_BEFORE*/	"Вставляет оп2 в позицию в файле до оп1. Оп берутся из стека",
	/*I_BLOCK*/"Указывает грамматическому модулю, какие блоки текста игнорировать",
	/*I_CASE*/		"Задает IgnoreCase=off/on/auto",
	/*FORMAT*/		"Работает аналогично sprintf. Первый параметр указывает количество аргументов(!=0). Самый первый аргумент - формат",
	/*INSTALL_PKG*/	"Берет параметрами имя файла-архива и каталог для распаковки. См.проект CAB", 
	/*REPLACE_BLOCK*/"Указывает грамматическому модулю, какие блоки текста заменить",
	/*EXTERN */		"Вызов внешней процедуры(функции). При необходимости кладет в стек (PUSH) возвращаемое функцией значение",
	/*SAVE_FILES*/	"",
	/*RESERVED*/	"Появление этой команды свидетельствует об ошибочной компиляции"
};
//------------------------------------------------------------------------
CPCodeList::CPCodeList(const IBuildContext & BuildContext,cstr szFileName)
{
	string_smart strFileName (include_backslash(BuildContext.GetIntermediateDirectory()),extract_filename(szFileName),".lst");
	remove(strFileName.c_str());

	#ifdef __DIAGNOSTIC__
	if (!m_DiagnosticFile.open(strFileName .c_str(),"wt"))
		errors::general_warning(Format("Can't open file '%s' for writing",strFileName .c_str()).c_str());
	#endif	

	StartPoint=0;
	isStartPoint=false;
	
	CreateNewFragment();
}
//------------------------------------------------------------------------
CPCodeList::~CPCodeList()
{
	_ASSERT(m_Lists.size()<=1);	//В деструкторе кол-во списков должно быть
	Clear();					//не более одного
}
//------------------------------------------------------------------------
void CPCodeList::Clear()	
{
	if (m_Lists.size()>0)
	{
		#ifdef __DIAGNOSTIC__
		for (size_t i=0;i<CurList().size();i++)
			REPORT(CurList()[i],i);
		#endif
	}
	while (!m_Lists.empty()) 
		m_Lists.pop();
}
//------------------------------------------------------------------------
void CPCodeList::CreateNewFragment()
{
	m_Lists.push(list_commands());
}
//------------------------------------------------------------------------
void CPCodeList::DestroyFragment(bool AppendToParent)
{
	_ASSERT(m_Lists.size()>0);
	if (AppendToParent)
	{
		list_commands child = m_Lists.pop_val();	//Вытаскиваем его из стека
		
		//Добавляем в голову следующего элемента
		_ASSERT(m_Lists.size()>0);
		copy(child.begin(),child.end(),	inserter(CurList(),CurList().end()));
	}
	else
		m_Lists.pop();
}
//------------------------------------------------------------------------
void CPCodeList::Add(const structs::code_cmd &Cmd)
{
	CurList().push_back(Cmd);
}
//------------------------------------------------------------------------
void CPCodeList::InsertFront(const structs::code_cmd & src)
{
	CurList().insert(CurList().begin(),src);
}
//------------------------------------------------------------------------
void CPCodeList::EraseFrom(long LineNumber)
{
	
	if ((LineNumber>=CurList().size()) || (LineNumber==-1))
		return;
	CurList().erase(CurList().begin()+LineNumber,CurList().end());
}
//------------------------------------------------------------------------
CPCodeList::list_commands::iterator CPCodeList::OptimizeLevels(list_commands::iterator i)
{
	list_commands::iterator j=i++;
	_ASSERT((*j).Operation==NEW_LEVEL);
	_ASSERT(j!=CurList().end());
	_ASSERT(i!=CurList().end());

	for (; (*i).Operation!=EXIT_LEVEL; ++i)
	{
		if ((*i).Operation==NEW_LEVEL) 
			i=OptimizeLevels(i);
		else if (oneof((*i).Operation,NEW,NEWA,POP_NEW))
			return i;
		_ASSERT(i!=CurList().end());
	}
	
	_ASSERT((*i).Operation==EXIT_LEVEL);
	(*i).Operation = NOP;
	(*j).Operation = NOP;

	return i;
}
//------------------------------------------------------------------------
static bool is_nop(const structs::code_cmd & cmd){
	return cmd.Operation==NOP;
}
static bool is_new_lvl(const structs::code_cmd & cmd){
	return cmd.Operation==NEW_LEVEL;
};
static bool is_blank_pair(const structs::code_cmd & cmd1,const structs::code_cmd & cmd2){
	return ((cmd1.Operation==STORE) && (oneof(cmd2.Operation,RESTORE,UNSTORE))) ||
			((cmd1.Operation==RESTORE) && (cmd2.Operation==RESTORE));
}
static bool is_subr_start(const structs::code_cmd & cmd){
	return cmd.Operation==SUBROUTINE;
}
static bool is_subr_end(const structs::code_cmd & cmd){
	return cmd.Operation==ENDSUBR;
}
static bool is_subr_body(const structs::code_cmd & cmd){
	return !oneof(cmd.Operation,NEW_LEVEL,EXIT_LEVEL,RET_S);
};

//------------------------------------------------------------------------
void CPCodeList::Optimize()
{
	//Выкидываем подряд идущие STORE+RESTORE, STORE+UNSTORE, RESTORE+RESTORE
	for (list_commands::iterator it = CurList().begin(); ;)
	{
		it = adjacent_find(it,CurList().end(),is_blank_pair);
		if (it==CurList().end())
			break;
		(*it++).Operation = NOP;
		(*it++).Operation = NOP;
	}
	
	//Оптимизация на размер: удаляем пустые subroutines, и на скорость: отменяем бесполезные вызовы
	for (list_commands::iterator it = CurList().begin(); ;)
	{	
		list_commands::iterator subr_it = find_if(it,CurList().end(),is_subr_start);
		it = find_if(subr_it,CurList().end(),is_subr_end);
		if (it==CurList().end())
			break;

		if (find_if(subr_it+1,it,is_subr_body)==it)
		{
			ASSERT_CHECK((pIGrmKrnl->ReplaceFunctionNameToAddr)((*subr_it).Op1.GetSzText(),NULL));
			fill(subr_it,++it,cmd(NOP));
		}
	}
	
	// Оптимизация на размер и скорость: удаляем холостые NEW_LEVEL и EXIT_LEVEL
/*	for (list_commands::iterator it = CurList().begin(); it!=CurList().end() ;)
	{
		it = find_if(it,CurList().end(),is_new_lvl);
		if (it==CurList().end())
			break;

		it=OptimizeLevels(it);
	}
*/
	//Удаляем все NOP
	CurList().erase(remove_if(CurList().begin(), CurList().end(),is_nop),
					CurList().end());

#ifdef _DEBUG
	// check
	int Levels=0;
	int Subroutines=0;

	for (int i=0;i<CurList().size();i++)
	{
		if (CurList()[i].Operation==NEW_LEVEL)
			Levels++;
		else if (CurList()[i].Operation==EXIT_LEVEL)
			Levels--;
		else if (CurList()[i].Operation==SUBROUTINE)
			Subroutines++;
		else if (CurList()[i].Operation==ENDSUBR)
			Subroutines--;
	}
	_ASSERT(Levels==0);
	_ASSERT(Subroutines==0);
#endif

}
//------------------------------------------------------------------------
#pragma warning (disable : 4101)
inline void CPCodeList::WriteToDiagnosticFile(structs::code_cmd &Cmd, size_t LineNumber)
{
	static	char_t nr[]="\n\r";
	static	char_t n[]="\n";
	static	char_t space[]="              ";
	static  char_t skip[]="  -  ";
	static  char_t remark[]="// ";
	static	char_t LBracket[]="{";
	static	char_t RBracket[]="}";
	static	char_t strIndex[20];
	static	char_t * Name;

	static	int  NameLen;
	
	if (!m_DiagnosticFile.isopen()) return;
      	
	try
	{
		_ASSERT(Cmd.Operation<(sizeof(CommandNames)<<2)); // size of pointer is 4 bytes

		if ((Cmd.Operation==FUNC)|| (Cmd.Operation==SUBROUTINE)) // для читабельности
			m_DiagnosticFile.write(n,1);


		sprintf(strIndex,"%-4u (%-4u)",LineNumber,Cmd.GetSourceLine());
		m_DiagnosticFile.write(strIndex, _tcslen(strIndex));
		m_DiagnosticFile.write(space,2);
		m_DiagnosticFile<<CommandNames[Cmd.Operation];
		NameLen=2+_tcslen(CommandNames[Cmd.Operation])+_tcslen(strIndex);
		
		if (!Cmd.Op1.IsNoClass())
		{
			sprintf(strIndex,"%-2u ",Cmd.Op1.Index);
			m_DiagnosticFile.write(space,1);
			m_DiagnosticFile<<(char)Cmd.Op1.GetClass();
			m_DiagnosticFile<<strIndex;

			NameLen+=_tcslen(strIndex)+2;
		}
		else 
		{
			m_DiagnosticFile.write(skip,sizeof(skip)-1);
			NameLen+=sizeof(skip)-1;
		}
		if (!Cmd.Op2.IsNoClass())
		{
			sprintf(strIndex,"%-2u ",Cmd.Op2.Index);
			m_DiagnosticFile.write(space,1);
			m_DiagnosticFile<<(char)Cmd.Op2.GetClass();
			m_DiagnosticFile<<strIndex;
			NameLen+=_tcslen(strIndex)+2;
		}
		else 
		{
			m_DiagnosticFile.write(skip,sizeof(skip)-1);
			NameLen+=sizeof(skip)-1;
		}

		if (!Cmd.Op3.IsNoClass())
		{
			sprintf(strIndex,"%-2u ",Cmd.Op3.Index);
			m_DiagnosticFile.write(space,1);
			m_DiagnosticFile<<(char)Cmd.Op3.GetClass();
			m_DiagnosticFile<<strIndex;
			NameLen+=_tcslen(strIndex)+2;
		}
		else 
		{
			m_DiagnosticFile.write(skip,sizeof(skip)-1);
			NameLen+=sizeof(skip)-1;
		}

		if (!Cmd.Op4.IsNoClass())
		{
			sprintf(strIndex,"%-2u ",Cmd.Op4.Index);
			m_DiagnosticFile.write(space,1);
			m_DiagnosticFile<<(char)Cmd.Op4.GetClass();
			m_DiagnosticFile<<strIndex;
			NameLen+=_tcslen(strIndex)+2;
		}
		else 
		{
			m_DiagnosticFile.write(skip,sizeof(skip)-1);
			NameLen+=sizeof(skip)-1;
		}

		if (37-NameLen>0)
			m_DiagnosticFile.write(space, 37-NameLen);
		m_DiagnosticFile.write(remark,sizeof(remark)-1);


		if ((!Cmd.Op1.IsNoClass())&& (Cmd.Op1.GetSzText()))
		{
			m_DiagnosticFile.write(LBracket,sizeof(LBracket)-1);
			m_DiagnosticFile<<Cmd.Op1.GetSzText();
			m_DiagnosticFile.write(RBracket,sizeof(RBracket)-1);
		}
		
		if ((!Cmd.Op2.IsNoClass()) && (Cmd.Op2.GetSzText()))
		{
			m_DiagnosticFile.write(LBracket,sizeof(LBracket)-1);
			m_DiagnosticFile<<string_smart(Cmd.Op2.GetSzText()).trans_ctrl();
			m_DiagnosticFile.write(RBracket,sizeof(RBracket)-1);
		}

		if ((!Cmd.Op3.IsNoClass()) && (Cmd.Op3.GetSzText()))
		{	
			m_DiagnosticFile.write(LBracket,sizeof(LBracket)-1);
			m_DiagnosticFile<<Cmd.Op3.GetSzText();
			if (Cmd.Op3.GetSzText()[0]=='\"')
				m_DiagnosticFile.write("\"",sizeof(char));
			m_DiagnosticFile.write(RBracket,sizeof(RBracket)-1);
		}

		if ((!Cmd.Op4.IsNoClass()) && (Cmd.Op4.GetSzText()))
		{	
			m_DiagnosticFile.write(LBracket,sizeof(LBracket)-1);
			m_DiagnosticFile<<Cmd.Op4.GetSzText();
			if (Cmd.Op4.GetSzText()[0]=='\"')
				m_DiagnosticFile.write("\"",sizeof(char));
			m_DiagnosticFile.write(RBracket,sizeof(RBracket)-1);
		}

		m_DiagnosticFile.write(space,1);
		m_DiagnosticFile<<CommandComments[Cmd.Operation];
		m_DiagnosticFile.write(nr,1);
/*
#ifdef _DEBUG
#pragma message ("Remember that 'File.Flush' in " __FILE__ " works very slowly")
		File.Flush();
#endif
		*/
	}
	catch (...)
	{
		output("Exception in writing PCodeList\n");
	}
}
#pragma warning (default: 4101)



