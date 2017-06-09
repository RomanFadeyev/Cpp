/********************************************************************
    created:    2002/06/15
    created:    15:6:2002   9:25
    filename:   $(Projects)\sources\mtdev\dialogs\finddlg.h
    file path:  $(Projects)\sources\mtdev\dialogs

    file base:  finddlg
    file ext:   h
    author:     Fadeyev R.V.
    
    purpose:    Диалог поиска текста в текущем документе
*********************************************************************/

#pragma once

#include "CObjectEx.h" //Один из родителей
#include "MTDevDialog_B.h"	//Один из родителей

#include <list>
#include <algorithm>
#include <MFCUtils.h>

using namespace mfc_utils;
/////////////////////////////////////////////////////////////////////////////
// CdlgFind dialog
class CdlgFind : public CdlgDialog_B,public CCObjectEx, public IFindTextLink
{
// Construction
public:
	CdlgFind(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CdlgFind)
	enum { IDD = IDD_FIND };
	int		m_bDirDown;
	BOOL	m_bMatchCase;
	BOOL	m_bRegexp;
	BOOL	m_bWholeWord;
	BOOL	m_bWrapAround;
	int		m_bInSelection;
	CString	m_strText;
	//}}AFX_DATA
	CRecentlyList m_RecentList;	
	vbdFind *m_pCurFindInfo;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgFind)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


protected:
	//Обработка глобальных событий
	override void OnRunApp();
	override void OnStopApp();

	override bool Run(vbdFind & Info);
	
	// Generated message map functions
	//{{AFX_MSG(CdlgFind)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
