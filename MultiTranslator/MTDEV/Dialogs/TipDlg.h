/*************************************************************************
created:	2001/12/29
created:	29:12:2001   11:42
filename: 	$(Projects)\Sources\MTDEv\TipDlg.h
file path:	$(Projects)\Sources\MTDEv
file base:	TipDlg
file ext:	h
author:		Фадеев Р.В. (Fadeyev R.V.)

  purpose:	Диалог "Tip of the Day". Может появляться при старте программы
  а также вызываться из меню			
*************************************************************************/

#if !defined(TIPDLG_H_INCLUDED_)
#define TIPDLG_H_INCLUDED_

#include "CObjectEx.h"
#include "MTDevDialog_B.h"	//Один из родителей
//#include <xShadeButton.h>	// shade buttons

/////////////////////////////////////////////////////////////////////////////
// CdlgTip dialog

class CdlgTip : public CdlgDialog_B,public CCObjectEx, public ITipOfTheDayLink
{
public:
	virtual ~CdlgTip();
	CdlgTip(CWnd* pParent = NULL);	 // standard constructor	

protected:
	void GetNextTipString(CString& strNext);	
	override void LoadSettings();
	override void SaveSettings();

	virtual int DoModal();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	//наследники от CCObjectEx
	override void OnRunApp();
	//from ITipOfTheDayLink
	override void Run() {DoModal();}

	// Dialog Data
	//{{AFX_DATA(CdlgTip)
	enum { IDD = IDD_TIP };
	EXTEND_BUTTON	m_buNext;
	EXTEND_BUTTON	m_buOK;
	BOOL	m_bStartup;
	CString	m_strTip;
	//}}AFX_DATA
	
	FILE* m_pStream;
	string_smart m_strTipFileName;

	// Generated message map functions
	//{{AFX_MSG(CdlgTip)
	afx_msg void OnNextTip();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // !defined(TIPDLG_H_INCLUDED_)
