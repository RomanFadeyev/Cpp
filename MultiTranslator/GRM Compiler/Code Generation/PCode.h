// PCode.h: interface for the CPCodeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCODE_H__07AA0C47_0559_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_PCODE_H__07AA0C47_0559_11D4_A1F4_0000E8D9F1D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseStructs.h"

//#define __DIAGNOSTIC__

// для команд DIV и SUB делимое (уменьшаемое) и делитель(вычитаемое) переставлены местами
// Те, у кого значение больше 255, работают через команду STORE, и в этом случае команда не должна иметь параметров

#include "DefineCommands.h" // enum ECodeOperation { NOP,PUSH,....
// NOP
// PUSH x y - x- уровень вложенности y - индекс в текущем уровне вложенности или в структуре
// POP  x
// ADD, SUB, MUL, DIV, RMNDR(остаток от деления) - первым из стека извлекается правый операнд (например, делитель), затем левый
// GET_JZ - выбирает елемет из стека переменных и переходит, если он = 0
// JMP x
// NEW x
// ASSIGN - певым извлекает из стека объект-присваиваемое, затем - объект присваиватель
// ENTER - вхождение в структуру
// STORE - кладет в стек переменных (не PUSH !!!!) STORE_GLBL == STORE 0,xx
// UNSTORE - вытаскивает из стека переменных
// RESTORE - очищает стек переменных

// ENDF - конец процедуры. Выполняет принулительный RET
// PUSH_EMPTY	
// FUNC - отмечает вхождение в функцию, нужна для компоновщика
// PRINT - работает исключительно через стек перменных
// NEWA - выделение массива, размерность массива берется из стека переменных
// POP_MEW - создает временную переменную и скидывает в нее значение из стека, если переменная уже существует, она уничтожается
// POP_ALIAS необходим при параметрах по ссылке. Адрес переменной берется из стека

class CPCodeList: public IPCodeList
{
protected:
	typedef structs::code_cmd cmd;
	base_utils::file_std m_DiagnosticFile;
	
	size_t	StartPoint;
	bool	isStartPoint;

	typedef vector<structs::code_cmd> list_commands;
	stack_ex<list_commands>	m_Lists;
	
	static cstr CommandNames[];
	static cstr CommandComments[];

	list_commands::iterator OptimizeLevels(list_commands::iterator i);

	bool	SaveData(stream & Stream, bool isDebug);
	void	WriteToDiagnosticFile (structs::code_cmd &Cmd, size_t LineNumber);	

	list_commands & CurList()	{return m_Lists.top();}
public:
	void	Optimize();
	void	Link();

	override void	EraseFrom(long LineNumber);
	override void	Add (const structs::code_cmd & src);
	override void	InsertFront(const structs::code_cmd & src);
	override void	CreateNewFragment();
	override void	DestroyFragment(bool AppendToParent);

	override size_t GetCount() const				{return m_Lists.top().size();}
	override const cmd & GetAt(size_t nPos) const	{_ASSERT(nPos<GetCount()); return m_Lists.top()[nPos];}
	override void	Reserve(size_t nSize){}
	
	void Clear();

	CPCodeList(const IBuildContext & BuildContext,cstr szFileName);
	virtual ~CPCodeList();
};

#endif // !defined(AFX_PCODE_H__07AA0C47_0559_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
