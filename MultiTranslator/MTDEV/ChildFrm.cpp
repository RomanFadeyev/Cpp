/********************************************************************
	created:	2002/09/21
	created:	21:9:2002   13:14
	filename: 	$(Projects)\sources\mtdev\childfrm.cpp
	file path:	$(Projects)\sources\mtdev
	file base:	childfrm
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "ChildFrm.h"
#include <BaseUtils.h>
#include ".\childfrm.h"
using namespace base_utils;
/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CBCGMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CBCGMDIChildWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction
#pragma warning(disable:4355)
CChildFrame::CChildFrame(): m_persistImpl(this),m_bFirstActivate(true)
{
}
//------------------------------------------------------------------------
CChildFrame::~CChildFrame()
{
}
//------------------------------------------------------------------------
BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !__super::PreCreateWindow(cs) )
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
} 
//------------------------------------------------------------------------
BOOL CChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext/* = NULL*/)
{
	BOOL bRes = __super::LoadFrame(nIDResource,dwDefaultStyle,pParentWnd,pContext);
	if (!bRes)
		return FALSE;

	CDocument *pDoc = pContext->m_pCurrentDoc;
	m_persistImpl.SetProfileHeading(string_smart("Documents\\History\\",(pDoc) ? pDoc->GetPathName() : "Common","\\Window Pos").c_str());
	m_persistImpl.Load();
	
	//Если включены Tabs - то избавляемся от Caption и от всех системных кнопок
	if (m_persistImpl.getCmdShow()==SW_SHOWMAXIMIZED)
		ModifyStyle(WS_SYSMENU|WS_BORDER,0,0);

	return bRes;
}
//------------------------------------------------------------------------
void CChildFrame::ActivateFrame(int nCmdShow)
{
	if (m_bFirstActivate){
		m_persistImpl.Apply();
		nCmdShow=m_persistImpl.getCmdShow();
	}
	__super::ActivateFrame(nCmdShow);
	m_bFirstActivate=false;
}
//------------------------------------------------------------------------
BOOL CChildFrame::DestroyWindow()
{
	m_persistImpl.Save();
	return __super::DestroyWindow();
}
//------------------------------------------------------------------------
void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	if (nType==SIZE_MAXIMIZED){
		CWnd * pParent = GetParent();
		CWnd * pChild = pParent->GetWindow(GW_CHILD);
		while (pChild!=NULL){
			if (pChild->GetStyle() & WS_SYSMENU)
				pChild->ModifyStyle(WS_SYSMENU | WS_BORDER,0,0);
			pChild=pChild->GetWindow(GW_HWNDNEXT);
		}
	}
	else if (nType==(SIZE_RESTORED) || (nType==(SIZE_MINIMIZED))){
		if (GetParentFrame()->GetActiveFrame()==this)
		{
			CWnd * pParent = GetParent();
			CWnd * pChild = pParent->GetWindow(GW_CHILD);
			while (pChild!=NULL){
				if (!(pChild->GetStyle() & WS_SYSMENU))
					pChild->ModifyStyle(0,WS_BORDER | WS_SYSMENU,0);
				pChild=pChild->GetWindow(GW_HWNDNEXT);
			}
		}
	}
	__super::OnSize(nType, cx, cy);
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics
#ifdef _DEBUG
void CChildFrame::AssertValid() const{
	__super::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const{
	__super::Dump(dc);
}
#endif //_DEBUG

