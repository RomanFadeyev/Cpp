/********************************************************************
	created:	2002/05/05
	created:	5:5:2002   21:49
	filename: 	$(Projects)\sources\mtdev\workspaceview\workspacebar.cpp
	file path:	$(Projects)\sources\mtdev\workspaceview

	file base:	workspacebar
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	WorkSpaceBar c 2 вкладками
*********************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "WorkspaceBar.h"

#include "Names.h"
#include "BaseStructs.h"

using namespace i_objects;

//Создание переходника для объекта
static i_objects::object_thunk<CbarWorkspace> barWorkspace(__MSG_PREFIX__);

//////////////////////////////////////////////////////////////////////////
// CbarWorkspace
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
CbarWorkspace::CbarWorkspace()
{
}
//------------------------------------------------------------------------
CbarWorkspace::~CbarWorkspace()
{
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CbarWorkspace, Cbar_B)
	//{{AFX_MSG_MAP(CbarWorkspace)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_NCLBUTTONDOWN()
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, cmdupOnViewWorkspace)
	ON_COMMAND(ID_VIEW_WORKSPACE, cmdOnViewWorkspace)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
	ON_WM_DESTROY()
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarWorkspace::OnCreateAppMainWnd()		
{
	//Получаем указатель на MainFrame-интерфейс
	IMainFrameLink * pMainFrame=i_objects::GetInterface<IMainFrameLink>();
	ASSERT_RETURN_VOID(pMainFrame);
	pMainFrame->IncludeToCmdMsgMap(this);
}
//------------------------------------------------------------------------
int CbarWorkspace::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create tabs window:
	if (!m_wndTabs.Create (CBCGTabWnd::STYLE_3D, rectDummy, this, 1))
	{
		TRACE0("Failed to create workspace tab window\n");
		return -1;      // fail to create
	}

	m_wndTabs.SetImageList (IDB_WORKSPACE, 16, RGB (255, 0, 0));

	// Create views:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE;
	
	if (!m_wndClassView.Create (dwViewStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndFileView.Create (dwViewStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	// Attach views to tab:
	m_wndTabs.AddTab (&m_wndClassView, _T("ClassView"), 0 /* Image number */);
	m_wndTabs.AddTab (&m_wndFileView, _T("FileView"), 2 /* Image number */);
	
	SetFlatTabs(FLAT_TABS);
	
	//Считываем из реестра последнюю активную страницу и устанавливаем ее
	int nActiveTab=	AfxGetApp()->GetProfileInt(szProfileWorkSpace,szActiveTab,0);
	if (inrange(nActiveTab,0,m_wndTabs.GetTabsNum()-1))
		m_wndTabs.SetActiveTab(nActiveTab);
	return 0;
}
//------------------------------------------------------------------------
void CbarWorkspace::OnDestroy()
{
	AfxGetApp()->WriteProfileInt(szProfileWorkSpace,szActiveTab,m_wndTabs.GetActiveTab());
	__super::OnDestroy();
}
//------------------------------------------------------------------------
void CbarWorkspace::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy,SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}
//------------------------------------------------------------------------
void CbarWorkspace::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	int iActiveTab = m_wndTabs.GetActiveTab ();
	if (iActiveTab < 0)
		return;
	
	CWnd* pWndTree = m_wndTabs.GetActiveWnd ();
	ASSERT_VALID (pWndTree);
	pWndTree->SendMessage(WM_CONTEXTMENU,WPARAM(pWndTree->m_hWnd),POINTTOPOINTS(point));
	return;
}
//------------------------------------------------------------------------
BOOL CbarWorkspace::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_ESCAPE)
	{
		CBCGFrameWnd * pMainFrame = (CBCGFrameWnd *)AfxGetMainWnd ();
		ASSERT_VALID (pMainFrame);

		if (pMainFrame->GetActivePopup () == NULL)
		{
			pMainFrame->SetFocus ();
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}
//------------------------------------------------------------------------
LRESULT CbarWorkspace::OnChangeActiveTab (WPARAM wp, LPARAM)
{
	int iTab = (int) wp;
	// Active tab was changed...
	return TRUE;
}
//------------------------------------------------------------------------
void CbarWorkspace::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (nHitTest == HTCAPTION)
	{
		int iActiveTab = m_wndTabs.GetActiveTab ();
		if (iActiveTab >= 0 && m_wndTabs.GetTabWnd (iActiveTab) != NULL)
		{
			m_wndTabs.GetTabWnd (iActiveTab)->SetFocus ();
		}
	}
	
	__super::OnNcLButtonDown(nHitTest, point);
}

//------------------------------------------------------------------------
void CbarWorkspace::cmdupOnViewWorkspace(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (IsVisible());
}
//------------------------------------------------------------------------
void CbarWorkspace::cmdOnViewWorkspace() 
{
	ToggleVisible();
}
//------------------------------------------------------------------------
