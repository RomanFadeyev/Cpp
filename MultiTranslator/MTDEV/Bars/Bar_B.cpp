/********************************************************************
	created:	2002/05/04
	created:	4:5:2002   23:47
	filename: 	$(Projects)\sources\mtdev\bars\bar_b.cpp
	file path:	$(Projects)\sources\mtdev\bars

	file base:	bar_b
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include <GUIService.h>
#include "Bar_B.h"

using namespace i_objects;

IMPLEMENT_DYNAMIC(Cbar_B, CBCGSizingControlBar)

BEGIN_MESSAGE_MAP(Cbar_B, CBCGSizingControlBar)
	ON_MESSAGE_VOID(CM_SHOW,msgOnShow)
	ON_MESSAGE_VOID(CM_HIDE,msgOnHide)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
/// Cbar_B::IBarLink
//////////////////////////////////////////////////////////////////////////
void Cbar_B::CreateWnd(CFrameWnd* pParentWnd)
{
	if (!Create (GetBarName(), pParentWnd, GetDefaultSize(),TRUE , GetBarID(), WS_CHILD | ((IsVisibleDefault()) ? WS_VISIBLE :0) | GetBarDock()))
		TRACE1("Failed to create  bar %s\n", GetBarName());
}
//------------------------------------------------------------------------
void Cbar_B::InitDock()
{
	EnableDocking(CBRS_ALIGN_ANY);	
	GetParentFrame()->DockControlBar(this);
}
//------------------------------------------------------------------------
void Cbar_B::OnShowMainFrame(bool bVisible){
	if (!IsFloating()) 
		return;

	if ((m_bVisible) && (bVisible) )
		Show();
}
//------------------------------------------------------------------------
CControlBar * Cbar_B::GetObject()
{
	return this;
}
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
/// Cbar_B
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
Cbar_B::Cbar_B():m_hTabWnd(NULL),m_hDelegateWnd(NULL),m_bVisible(false)
{
}
//------------------------------------------------------------------------
Cbar_B::~Cbar_B()
{
}
//------------------------------------------------------------------------
bool Cbar_B::IsDelegated() const{
	return GetDelegateControl() && GetTabWnd();
}
//------------------------------------------------------------------------
bool Cbar_B::IsVisibleDefault() const{
	return true;
}
//------------------------------------------------------------------------
int	 Cbar_B::GetDelegatedPageNo() const {
	CBCGTabWnd * pTabWnd = GetTabWnd();
	for (int i=0; (pTabWnd) && (i<pTabWnd->GetTabsNum());i++)
		if (pTabWnd->GetTabWnd(i)->GetSafeHwnd()==m_hDelegateWnd)
			return i;
	return -1;
}
//------------------------------------------------------------------------
bool Cbar_B::Delegate(CWnd* pWnd, CBCGTabWnd * pTabWnd){
	ASSERT_RETURN_FALSE(!IsDelegated() && pWnd && pTabWnd);

	bool bBarVisible=__super::IsVisible()!=FALSE;
	SendMessage(CM_HIDE,0,0);	
	CString strText; GetWindowText(strText);

	pWnd->SetParent(pTabWnd);
	pTabWnd->AddTab(pWnd,strText);
	//pTabWnd->SetActiveTab(pTabWnd->GetTabsNum()-1);
	
	m_hTabWnd=pTabWnd->m_hWnd;
	m_hDelegateWnd=pWnd->m_hWnd;
	
	//Убеждаемся, что, если мы были видимы в родном окне, мы должеы быть видимы и в чужом
	if (bBarVisible)
		ShowDelegate(true);
	return true;
}
//------------------------------------------------------------------------
bool Cbar_B::UnDelegate(){
	ASSERT_RETURN_FALSE(IsDelegated());
	CBCGTabWnd *pTabWnd = GetTabWnd();
	
	pTabWnd->AutoDestroyWindow(false);
	GetDelegateControl()->SetParent(this);
	pTabWnd->RemoveTab(GetDelegatedPageNo());		
	pTabWnd->AutoDestroyWindow(true);
	
	CRect R; GetClientRect(R);
	::SetWindowPos(m_hDelegateWnd,NULL,0,0,R.Width(),R.Height(),SWP_NOZORDER | SWP_NOREPOSITION );
	SendMessage(CM_SHOW,0,0);
	
	m_hDelegateWnd=NULL;
	m_hTabWnd=NULL;
	return true;
}
//------------------------------------------------------------------------
bool Cbar_B::IsVisible(bool ) const{
	if (!IsDelegated()) return 
		__super::IsVisible()!=0;

	bool bRes=(GetDelegateControl()->GetStyle() & WS_VISIBLE)!=0;
	for (CWnd * pWnd = GetDelegateControl()->GetParent(); (bRes) && (pWnd) && (pWnd!=AfxGetMainWnd()); pWnd=pWnd->GetParent())
		bRes&=(pWnd->GetStyle() & WS_VISIBLE)!=0;

	return bRes;
}
//------------------------------------------------------------------------
void Cbar_B::ShowDelegate(bool bShow)
{
	ASSERT(IsDelegated());
	
	for (CWnd * pWnd = GetDelegateControl()->GetParent(); (pWnd) && (pWnd!=AfxGetMainWnd()); pWnd=pWnd->GetParent())
		pWnd->SendMessage((bShow) ? CM_SHOW : CM_HIDE ,0,0);

	if (bShow){
		CBCGTabWnd * pTabWnd = GetTabWnd();
		if (pTabWnd)
			pTabWnd->SetActiveTab(GetDelegatedPageNo());
	}
}
//------------------------------------------------------------------------
void Cbar_B::ToggleVisible()
{
	if (IsDelegated())
		ShowDelegate(!IsVisible());
	else
		SendMessage((__super::IsVisible()) ? CM_HIDE :CM_SHOW,0,0);
}
//------------------------------------------------------------------------
void Cbar_B::Show()
{
	if (IsDelegated())
		ShowDelegate(true);
	else
		SendMessage(CM_SHOW,0,0);
}
//------------------------------------------------------------------------
void Cbar_B::Hide()
{
	if (IsDelegated())
		ShowDelegate(false);
	else
		SendMessage(CM_HIDE);
}
//------------------------------------------------------------------------
void Cbar_B::msgOnShow()
{
	CFrameWnd * pFrame=GetDockingFrame();
		
	pFrame->ShowControlBar (this,true,false);
	pFrame->RecalcLayout ();
}
//------------------------------------------------------------------------
void Cbar_B::msgOnHide()
{
	CFrameWnd * pFrame=GetParentFrame();
		
	pFrame->ShowControlBar (this,false,false);
	pFrame->RecalcLayout ();
}
//------------------------------------------------------------------------
void Cbar_B::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (((!AfxGetMainWnd()) || (!AfxGetMainWnd()->IsWindowVisible())) && 
		(IsFloating()) &&
		(lpwndpos->flags & SWP_SHOWWINDOW)) 
	{
		m_bVisible=true;
		lpwndpos->flags &= ~(SWP_SHOWWINDOW);
	}
	__super::OnWindowPosChanging(lpwndpos);
}
