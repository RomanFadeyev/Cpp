/********************************************************************
	created:	2003/06/08
	created:	8:6:2003   19:53
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lexlist.h
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lexlist
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_LEXLIST_H__05369BE6_0293_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_LEXLIST_H__05369BE6_0293_11D4_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once
#include "BaseStructs.h"

class CLexList: public ILexList
{
private:
	mutable long	m_nCurPos; // Ќаступ€т хорошие времена - сделать тип size_t
	vector<structs::lexeme> m_lstLexemes;
	file_std		m_DiagnosticFile;
	
	void	WriteToDiagnosticFile(structs::lexeme & LE);
public:
	CLexList(const IBuildContext & BuildContext,cstr szFileName);
	virtual ~CLexList();

	//ILexList
	override void Add(const structs::lexeme & src);

	override const structs::lexeme & GetAt(size_t nIndex)	const	{_ASSERT(nIndex<GetCount()); return m_lstLexemes[nIndex];}
	override const structs::lexeme & Get()					const	{return GetAt(m_nCurPos);}
	override size_t	GetCount()						const	{return m_lstLexemes.size();}

	override size_t	GetPos()		const	{return m_nCurPos;}
	override void	SetPos(size_t p)const	{_ASSERT(p<=GetCount());m_nCurPos=p;}
	override bool	IsEnd()			const	{_ASSERT(m_nCurPos<=GetCount());return m_nCurPos==GetCount();}
	override bool	IsStart()		const	{_ASSERT(m_nCurPos>=0);return m_nCurPos==0;}

	override void	Next()			const	{_ASSERT(!IsEnd());m_nCurPos++;}
	override void	Prev()			const	{_ASSERT(!IsStart());m_nCurPos--;}

	// these functions don't move pointer 
	override const structs::lexeme & GetPrev()	const	{_ASSERT(!IsStart()); return GetAt(m_nCurPos-1);}	
	override const structs::lexeme & GetNext()	const	{_ASSERT(!IsEnd()); return GetAt(m_nCurPos+1);}	

	void Clear(){m_lstLexemes.clear();}
};

#endif // !defined(AFX_LEXLIST_H__05369BE6_0293_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
