/********************************************************************
	created:	2003/06/10
	created:	10:6:2003   9:24
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lexlist.cpp
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lexlist
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "LexList.h"
#include "EWQueue.h"


#ifdef __DIAGNOSTIC__
	#define REPORT(x)	WriteToDiagnosticFile(x);
#else
	#define REPORT(x)	
#endif

//------------------------------------------------------------------------
CLexList::CLexList(const IBuildContext & BuildContext,cstr szFileName)
{
	string_smart strFileName (include_backslash(BuildContext.GetIntermediateDirectory()),extract_filename(szFileName),".lexlst");
	remove(strFileName.c_str());

	#ifdef __DIAGNOSTIC__
	if (!m_DiagnosticFile.open(strFileName.c_str(),"wt"))
		errors::w_file_was_not_opened(strFileName.c_str());
	else
		m_DiagnosticFile<<"Lexical List\n\n";
	#endif

	m_nCurPos=0;
}
//------------------------------------------------------------------------
CLexList::~CLexList()
{

}
//------------------------------------------------------------------------
void CLexList::Add(const structs::lexeme & src)
{
	m_lstLexemes.push_back(src);	
	REPORT(m_lstLexemes.back());
}
//------------------------------------------------------------------------
inline void CLexList::WriteToDiagnosticFile(structs::lexeme &LE)
{
	if (m_DiagnosticFile.isopen())
	{
		m_DiagnosticFile<<(char)LE.GetClass();
		m_DiagnosticFile<<Format("%u\t\t",LE.Index);

		_ASSERT(LE.GetSzText());
		m_DiagnosticFile<<str_translate_control(LE.GetSzText());
		
		m_DiagnosticFile<<Format("\t\tPos: %u, %u - %u, %u",LE.GetStartPos().m_nLine+1,LE.GetStartPos().m_nCol+1,LE.GetEndPos().m_nLine+1,LE.GetEndPos().m_nCol+1);
		m_DiagnosticFile<<"\n";
		m_DiagnosticFile.flush();
	}
}
