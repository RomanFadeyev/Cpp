/********************************************************************
	created:	2002/09/21
	created:	21:9:2002   12:56
	filename: 	$(Projects)\sources\mtdev\dialogs\gotolinedlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	gotolinedlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include <ExtControls.h>
#include "CObjectEx.h" //Один из родителей
#include "MTDevDialog_B.h"	//Один из родителей
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CdlgGotoLine dialog
class CdlgGotoLine : public CdlgDialog_B,public CCObjectEx, public IGotoLineLink
{
// Construction
public:
	CdlgGotoLine(CWnd* pParent = NULL);   // standard constructor

protected:
	//{{AFX_DATA(CdlgGotoLine)
	enum { IDD = IDD_GOTOLINE };
	CString	m_strCurLine;
	CString	m_strLastLine;
	long	m_dwNewLine;
	CEditEx	m_wndGotoLine;	
	//}}AFX_DATA

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	override bool Run(long dwCurLine, long dwLastLine, OUT long & out_dwNewLine);
protected:
	DECLARE_MESSAGE_MAP()
};
