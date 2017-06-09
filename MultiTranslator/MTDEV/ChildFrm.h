/********************************************************************
	created:	2002/09/21
	created:	21:9:2002   13:14
	filename: 	$(Projects)\sources\mtdev\childfrm.h
	file path:	$(Projects)\sources\mtdev
	file base:	childfrm
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_CHILDFRM_H__F685084F_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
#define AFX_CHILDFRM_H__F685084F_7AA4_11D3_A74B_0090274409AC__INCLUDED_

#include <PersistFrame.h>	//Для сохранения размеров окна

#pragma once

class CChildFrame : public CBCGMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	virtual ~CChildFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:
	CPersistFrameImpl	m_persistImpl;	
	bool				m_bFirstActivate;

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext = NULL);
	virtual BOOL DestroyWindow();
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

#endif // !defined(AFX_CHILDFRM_H__F685084F_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
