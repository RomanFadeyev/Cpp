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

// ��� ������ DIV � SUB ������� (�����������) � ��������(����������) ������������ �������
// ��, � ���� �������� ������ 255, �������� ����� ������� STORE, � � ���� ������ ������� �� ������ ����� ����������

#include "DefineCommands.h" // enum ECodeOperation { NOP,PUSH,....
// NOP
// PUSH x y - x- ������� ����������� y - ������ � ������� ������ ����������� ��� � ���������
// POP  x
// ADD, SUB, MUL, DIV, RMNDR(������� �� �������) - ������ �� ����� ����������� ������ ������� (��������, ��������), ����� �����
// GET_JZ - �������� ������ �� ����� ���������� � ���������, ���� �� = 0
// JMP x
// NEW x
// ASSIGN - ����� ��������� �� ����� ������-�������������, ����� - ������ �������������
// ENTER - ��������� � ���������
// STORE - ������ � ���� ���������� (�� PUSH !!!!) STORE_GLBL == STORE 0,xx
// UNSTORE - ����������� �� ����� ����������
// RESTORE - ������� ���� ����������

// ENDF - ����� ���������. ��������� �������������� RET
// PUSH_EMPTY	
// FUNC - �������� ��������� � �������, ����� ��� ������������
// PRINT - �������� ������������� ����� ���� ���������
// NEWA - ��������� �������, ����������� ������� ������� �� ����� ����������
// POP_MEW - ������� ��������� ���������� � ��������� � ��� �������� �� �����, ���� ���������� ��� ����������, ��� ������������
// POP_ALIAS ��������� ��� ���������� �� ������. ����� ���������� ������� �� �����

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
