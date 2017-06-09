/********************************************************************
	created:	2002/08/20
	created:	20:8:2002   9:45
	filename: 	$(Projects)\sources\mtdev\bars\fullscreenbar.cpp
	file path:	$(Projects)\sources\mtdev\bars
	file base:	fullscreenbar
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Панель инструментов из одной кнопки - FullScreen
*********************************************************************/

#include "stdafx.h"
#include "FullScreenBar.h"

using namespace i_objects;

//Создание переходника для объекта
static i_objects::object_thunk<CbarFullScreen> barFullScreen(__MSG_PREFIX__);

IMPLEMENT_DYNAMIC(CbarFullScreen, CBCGToolBar)

BEGIN_MESSAGE_MAP(CbarFullScreen, CBCGToolBar)
	ON_MESSAGE_VOID(CM_SHOW,msgOnShow)
	ON_MESSAGE_VOID(CM_HIDE,msgOnHide)
	ON_COMMAND(ID_VIEW_FULLSCREEN, cmdOnViewFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, cmdupOnViewFullscreen)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
/// CbarFullScreen::IBarLink
//////////////////////////////////////////////////////////////////////////
void CbarFullScreen::CreateWnd(CFrameWnd* pParentWnd)
{
	if	(!Create(pParentWnd, WS_CHILD | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,GetBarID()) ||
		(!LoadToolBar(GetBarIDR())))
		TRACE1("Failed to create  toolbar %s\n", GetBarName());
	CBCGToolBar::AddToolBarForImageCollection (GetBarIDR());
}
//------------------------------------------------------------------------
void CbarFullScreen::InitDock()
{
	EnableDocking(CBRS_ALIGN_ANY);
	SetWindowText (barnFullScreen);
	SetWindowPos(0, 100,100, 0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);   
	GetParentFrame()->FloatControlBar(this, CPoint(100,100));
	EnableTextLabels(true);
}
//------------------------------------------------------------------------
CControlBar * CbarFullScreen::GetObject()
{
	return this;
}

//////////////////////////////////////////////////////////////////////////
/// CbarFullScreen
//////////////////////////////////////////////////////////////////////////
CbarFullScreen::CbarFullScreen()
{
	m_bFullScreen=false;
}
//------------------------------------------------------------------------
CbarFullScreen::~CbarFullScreen()
{
}
//------------------------------------------------------------------------
bool CbarFullScreen::IsVisible()
{
	return (GetStyle () & WS_VISIBLE)!=0;
}
//------------------------------------------------------------------------
void CbarFullScreen::msgOnShow()
{
	CFrameWnd * pFrame=GetParentFrame();
		
	pFrame->ShowControlBar (this,true,false);
	pFrame->RecalcLayout ();
}
//------------------------------------------------------------------------
void CbarFullScreen::msgOnHide()
{
	CFrameWnd * pFrame=GetParentFrame();
		
	pFrame->ShowControlBar (this,false,false);
	pFrame->RecalcLayout ();
}
//------------------------------------------------------------------------
bool CbarFullScreen::IsFullScreen()
{
	return m_bFullScreen;
}
//------------------------------------------------------------------------
void CbarFullScreen::ToggleFullScreen()
{

	m_bFullScreen=!m_bFullScreen;
	if (m_bFullScreen)
		SendMessage(CM_SHOW,0,0);
	else
		SendMessage(CM_HIDE,0,0);

	CFrameWnd * pMainFrame=DYNAMIC_DOWNCAST(CFrameWnd,AfxGetMainWnd());
	
	WINDOWPLACEMENT wpNew;
	if (m_bFullScreen)
	{
		m_bars.clear();
	
		for (HWND hWndChild = ::GetTopWindow(pMainFrame->m_hWnd); hWndChild != NULL;
			hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
		{
			CControlBar* pWnd = DYNAMIC_DOWNCAST(CControlBar,CWnd::FromHandlePermanent(hWndChild));
			if ((pWnd) && (pWnd!=this) && (!pWnd->IsFloating())){
				m_bars[pWnd->GetDlgCtrlID()]=pWnd;
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	    // We'll need these to restore the original state.
		m_wpOrigPlacement.length = sizeof m_wpOrigPlacement;
	    pMainFrame->GetWindowPlacement (&m_wpOrigPlacement);

		RECT rectDesktop; ::GetWindowRect ( ::GetDesktopWindow(), &rectDesktop );
		::AdjustWindowRectEx(&rectDesktop, pMainFrame->GetStyle(), FALSE, pMainFrame->GetExStyle());

		//Adjust RECT to new size of window
        wpNew = m_wpOrigPlacement;
        wpNew.showCmd =  SW_SHOWNORMAL;
	    wpNew.rcNormalPosition = rectDesktop;
	}
	else {
		for (HWND hWndChild = ::GetTopWindow(pMainFrame->m_hWnd); hWndChild != NULL;
			hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
		{
			CControlBar* pWnd = DYNAMIC_DOWNCAST(CControlBar,CWnd::FromHandlePermanent(hWndChild));

			if ((pWnd) && (pWnd!=this) && (!pWnd->IsFloating()) && 
				((GetStyle () & WS_VISIBLE)==0) && 
				(m_bars.find(pWnd->GetDlgCtrlID())!=m_bars.end())) 
			{
				pWnd->ShowWindow(SW_SHOW);
			}
		}
		wpNew = m_wpOrigPlacement;
	}
	pMainFrame->SetWindowPlacement (&wpNew );
	pMainFrame->RecalcLayout();	
}
//------------------------------------------------------------------------
void CbarFullScreen::cmdOnViewFullscreen()
{
	ToggleFullScreen();
}
//------------------------------------------------------------------------
void CbarFullScreen::cmdupOnViewFullscreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(IsFullScreen());
}
//------------------------------------------------------------------------

void CbarFullScreen::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	//Если мы уже не видимы, а режим все еще FullScreen, снимаем это режим
	if ((lpwndpos->flags &  SWP_HIDEWINDOW) && IsFullScreen())
		ToggleFullScreen();
}
