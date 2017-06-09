/********************************************************************
	created:	2002/09/21
	created:	21:9:2002   13:03
	filename: 	$(Projects)\sources\mtdev\dialogs\gotolinedlg.cpp
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	gotolinedlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"

#include "Resource.h"
#include "GotoLineDlg.h"

#include <BaseUtils.h>

#include "Names.h"
#include "BaseStructs.h"

//Создание переходника для объекта
static i_objects::object_thunk< CdlgGotoLine > GotoLineObject(__MSG_PREFIX__);

/////////////////////////////////////////////////////////////////////////////
// CdlgGotoLine dialog

CdlgGotoLine::CdlgGotoLine(CWnd* pParent /*=NULL*/)
	: CdlgDialog_B(CdlgGotoLine::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgGotoLine)
	m_strCurLine = _T("");
	m_strLastLine = _T("");
	m_dwNewLine = 0;
	//}}AFX_DATA_INIT
}
//------------------------------------------------------------------------
void CdlgGotoLine::DoDataExchange(CDataExchange* pDX)
{
	CdlgDialog_B::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GOTOLINE_CURRENT_LINE, m_strCurLine);
	DDX_Text(pDX, IDC_GOTOLINE_LAST_LINE, m_strLastLine);
	DDX_Text(pDX, IDC_GOTOLINE_GOTOLINE, m_dwNewLine);
	DDX_Control(pDX, IDC_GOTOLINE_GOTOLINE, m_wndGotoLine);
}

BEGIN_MESSAGE_MAP(CdlgGotoLine, CdlgDialog_B)
END_MESSAGE_MAP()

//------------------------------------------------------------------------
bool CdlgGotoLine::Run(long dwCurLine, long dwLastLine, OUT long & out_dwNewLine)
{
	m_strCurLine=base_utils::itosf(dwCurLine+1,' ' ).c_str();
	m_strLastLine=base_utils::itosf(dwLastLine+1,' ').c_str();
	if (DoModal()==IDOK)
		return (out_dwNewLine=m_dwNewLine-1,true);
	else
		return false;
}
