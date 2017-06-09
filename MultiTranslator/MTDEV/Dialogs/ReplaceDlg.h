/********************************************************************
	created:	2002/02/10
	created:	10:2:2002   21:32
	filename: 	$(Projects)\Sources\MTDEv\Dialogs\ReplaceDlg.h
	file path:	$(Projects)\Sources\MTDEv\Dialogs

	file base:	ReplaceDlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Диалог замены текста
*********************************************************************/

#pragma once
#include "MTDevDialog_B.h"	//Один из родителей
#include "CObjectEx.h"		//Один из родителей
#include "BaseStructs.h"	//Для vbdReplace 
#include <MFCUtils.h>


using namespace mfc_utils;
/////////////////////////////////////////////////////////////////////////////
// CdlgReplace dialog

class CdlgReplace : public CdlgDialog_B,public CCObjectEx, public IReplaceTextLink	
{
public:
    // Construction
    CdlgReplace(CWnd* pParent = NULL);   // standard constructor

private:
	void PerformCallback(IReplaceTextLink::vbdReplace::operations lOp);
	IReplaceTextLink::vbdReplace *m_pCurReplaceInfo;

	CRecentlyList m_FindMRU;
	CRecentlyList m_ReplaceMRU;
	
	void PushFindValue();
	void PushReplaceValue();

protected:
	//Обработка глобальных событий
	override void OnRunApp();
	override void OnStopApp();

	//Обработчик глагола vbShow
	override bool Run(vbdReplace & Data);
	
    // Dialog Data
    //{{AFX_DATA(CdlgReplace)
	enum { IDD = IDD_REPLACE };
	CString	m_strFindText;
	CString	m_strReplaceText;
	BOOL	m_bMatchCase;
	BOOL	m_bRegexp;
	int		m_bInSelection;
	BOOL	m_bWholeWord;
	BOOL	m_bWrapAround;
	//}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CdlgReplace)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

    // Implementation

    // Generated message map functions
    //{{AFX_MSG(CdlgReplace)
	afx_msg void cmdOnFindNext();
	afx_msg void cmdOnReplace();
	afx_msg void cmdOnReplaceAll();
	afx_msg void OnBnClickedRadioScope();	
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
