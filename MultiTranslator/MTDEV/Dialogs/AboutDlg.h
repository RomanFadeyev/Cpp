/********************************************************************
	created:	2002/01/07
	created:	7:1:2002   17:11
	filename: 	$(Projects)\Sources\MTDEv\AboutDlg.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	AboutDlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	About Dialog
*********************************************************************/

#if !defined(AFX_ABOUTDLG_H__59E41DA4_590E_4784_88D2_492061611ABA__INCLUDED_)
#define AFX_ABOUTDLG_H__59E41DA4_590E_4784_88D2_492061611ABA__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000 

#include <CWaterRoutine.h>
#include <DIBSectionLite.h>
#include "CObjectEx.h" //Один из родителей
#include "MTDevDialog_B.h" //Один из родителей
#include "afxwin.h"

class CdlgAbout : public CdlgDialog_B,public CCObjectEx, public IAboutLink	
{
public:
	CdlgAbout();
protected:
	//{{AFX_DATA(CdlgAbout)
	enum { IDD = IDD_ABOUTBOX };
	CBCGURLLinkButton	m_btnMail;
	CBCGURLLinkButton	m_btnUrl;
	CString	m_strVersion;
	CStatic m_wndLogo;
	//}}AFX_DATA
	CWaterRoutine	m_myWater;
	CDIBSectionLite	m_bmpRenderSource;
	CDIBSectionLite m_bmpRenderTarget;
	DWORD			m_nRenderTimer;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	override void Run()	{DoModal();}

	afx_msg void OnDestroy();
	CString m_strCopyright;
};

#endif // !defined(AFX_ABOUTDLG_H__59E41DA4_590E_4784_88D2_492061611ABA__INCLUDED_)
