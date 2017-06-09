/********************************************************************
created:		2002/03/10
created:		10:3:2002	0:33
filename:		$(Projects)\Sources\MTDEv\MainFrm.cpp
file path:		$(Projects)\Sources\MTDEv

file base:		MainFrm
file ext:		cpp
author:			Fadeyev	R.V.

purpose:		Основной фрейм программы
*********************************************************************/

#include "stdafx.h"
#include <GUIService.h>
#include <algorithm>

#include "Resource.h"
#include "MainFrm.h"
#include "Names.h"
#include "FindComboBox.h"
#include "EditionRestrictions.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

#define CM_SYNC_INVOKE WM_USER+0xFF

const int  iMaxUserToolbars		= 10;
const UINT uiFirstUserToolBarId	= AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId	= uiFirstUserToolBarId + iMaxUserToolbars -	1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_BUILD_TOOLBAR, cmdOnViewBuildToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BUILD_TOOLBAR,	cmdupOnViewBuildToolbar)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MDITABS_ICONS, OnMditabsIcons)
	ON_UPDATE_COMMAND_UI(ID_MDITABS_ICONS, OnUpdateMditabsIcons)
	ON_COMMAND(ID_MDITABS_TOP, OnMditabsTop)
	ON_UPDATE_COMMAND_UI(ID_MDITABS_TOP, OnUpdateMditabsTop)
	ON_COMMAND(ID_MDITABS_BOTTOM, OnMditabsBottom)
	ON_UPDATE_COMMAND_UI(ID_MDITABS_BOTTOM,	OnUpdateMditabsBottom)
	ON_COMMAND(ID_VIEW_MDI_TABS, cmdOnViewMdiTabs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MDI_TABS, cmdupOnViewMdiTabs)
	ON_COMMAND(ID_MDITABS_CLOSE, cmdOnMditabsClose)
	ON_UPDATE_COMMAND_UI(ID_MDITABS_CLOSE, OnUpdateMditabsClose)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_REGISTERED_MESSAGE(BCGM_TOOLBARMENU,	OnToolbarContextMenu)
	ON_COMMAND_RANGE(ID_VIEW_USER_TOOLBAR1,	ID_VIEW_USER_TOOLBAR10,	OnToolsViewUserToolbar)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_USER_TOOLBAR1, ID_VIEW_USER_TOOLBAR10, OnUpdateToolsViewUserToolbar)
	ON_REGISTERED_MESSAGE(BCGM_CUSTOMIZEHELP, OnHelpCustomizeToolbars)
	ON_COMMAND(ID_VIEW_DEBUG_TOOLBAR, cmdOnViewDebugToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUG_TOOLBAR,	cmdupOnViewDebugToolbar)
	ON_COMMAND(ID_VIEW_EDIT_TOOLBAR, cmdOnViewEditToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDIT_TOOLBAR, cmdupOnViewEditToolbar)
	ON_MESSAGE(CM_SYNC_INVOKE,OnSyncInvoke)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

static UINT	indicators[] =
{
	ID_SEPARATOR //ID_INDICATOR_POS,ID_INDICATOR_CAPS,ID_INDICATOR_OVR,ID_INDICATOR_NUM,ID_INDICATOR_SCRL
};

static UINT	basic_commands[]=
{
		ID_FILE_NEW,					ID_FILE_OPEN,					ID_FILE_SAVE,
		ID_FILE_PRINT,					ID_APP_EXIT,					ID_APP_ABOUT,	
		ID_FILE_CLOSE,					ID_FILE_CLOSE_ALL,				ID_FILE_PROJECT_OPEN,
		ID_FILE_PROJECT_SAVE,			ID_FILE_PROJECT_CLOSE,
		
		ID_VIEW_TOOLBAR,				ID_VIEW_CUSTOMIZE,				ID_VIEW_TOOLBARS,
		ID_VIEW_WHITESPACE,				ID_VIEW_INDENTGUIDES,			ID_VIEW_FULLSCREEN,	
		
		ID_EDIT_ZOOM_IN,				ID_EDIT_ZOOM_OUT,				ID_EDIT_ZOOM_RESET,				
		ID_EDIT_FOLD_ALL,				ID_EDIT_FOLD_CUR,				ID_DEBUG_START_DEBUG,			
		ID_DEBUG_START_RUN,				ID_DEBUG_STOP,
		ID_EDIT_FIND,					ID_EDIT_REPLACE,				
		ID_EDIT_NEXT_BOOKMARK,			ID_EDIT_PREV_BOOKMARK,			ID_EDIT_TOGGLE_BOOKMARK,
		ID_EDIT_GOTOLINE,				ID_EDIT_PASTE,					ID_EDIT_UNDO,
		ID_EDIT_CUT,	

		ID_BUILD_COMPILE_SOURCE,		ID_BUILD_BUILD_PROJECT,			ID_BUILD_CLEAN, ID_BUILD_STOP_BUILD,
		
		ID_TOOLS_MACRO,					ID_WINDOW_TILE_HORZ,	
		
		ID_FILE_MRU_PROJECT1,	ID_FILE_MRU_PROJECT2,	ID_FILE_MRU_PROJECT3,	
		ID_FILE_MRU_PROJECT4,	ID_FILE_MRU_PROJECT5,	ID_FILE_MRU_PROJECT6,	
		ID_FILE_MRU_PROJECT7,	ID_FILE_MRU_PROJECT8,	ID_FILE_MRU_PROJECT9,	
		ID_FILE_MRU_PROJECT10,	ID_FILE_MRU_PROJECT11,	ID_FILE_MRU_PROJECT12,	
		ID_FILE_MRU_PROJECT13,	ID_FILE_MRU_PROJECT14,	ID_FILE_MRU_PROJECT15,		
		ID_FILE_MRU_PROJECT16,
				
		ID_FILE_NEW_DOC1,	ID_FILE_NEW_DOC2,	ID_FILE_NEW_DOC3,	
		ID_FILE_NEW_DOC4,	ID_FILE_NEW_DOC5,	ID_FILE_NEW_DOC6,	
		ID_FILE_NEW_DOC7,	ID_FILE_NEW_DOC8,	ID_FILE_NEW_DOC9,	
		ID_FILE_NEW_DOC10,	ID_FILE_NEW_DOC11,	ID_FILE_NEW_DOC12,	
		ID_FILE_NEW_DOC13,	ID_FILE_NEW_DOC14,	ID_FILE_NEW_DOC15,		
		ID_FILE_NEW_DOC16,

		ID_PARSING_COMPLETE_REPORT,ID_PARSING_REPORT_TO_FILE,ID_PARSING_SHORT_REPORT,		
		
		ID_OUTPUT_OPTIONS_TEXTCOLOR, ID_OUTPUT_OPTIONS_BACKCOLOR,

		ID_PROJECT_ADD_FILES,	ID_PROJECT_SET_ACTIVE,	ID_PROJECT_UNLOAD, ID_PROJECT_ADD_RULE,
		ID_PROJECT_BUILD_SYNTAX_DIAGRAM, ID_PROJECT_BUILD_SYNTAX_TREE, ID_PROJECT_EXPORT_TO_XBNF,
		ID_PROJECT_ADD_TO_KNOWLEDGEBASE, ID_PROJECT_SETTINGS
};

//Макрос для создания панелей инструментов
#define	CREATE_TOOLBAR(Toolbar,IDW , ID, IDS, Style) {\
	CString	strToolBarTitle;\
	if (!Toolbar.Create(this, Style,IDW	) || !(Toolbar.LoadToolBar(ID))	|| !(strToolBarTitle.LoadString(IDS))){\
	TRACE0("Failed to create toolbar\n");return -1;}\
	Toolbar.SetWindowText (strToolBarTitle);\
	Toolbar.EnableDocking(CBRS_ALIGN_ANY);\
	Toolbar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, IDS_CUSTOMIZE);}
//Макрос для создания MenuBar
#define	CREATE_MENUBAR(MenuBar){\
	if (!MenuBar.Create	(this)){\
	TRACE0("Failed to create menubar\n");return -1;}\
	MenuBar.SetBarStyle(MenuBar.GetBarStyle() |	CBRS_SIZE_DYNAMIC |	CBRS_TOOLTIPS |	CBRS_FLYBY);\
	MenuBar.EnableDocking(CBRS_ALIGN_ANY);}
//Макрос для создания StatusBar
#define	CREATE_STATUSBAR(StatusBar)\
	if (!StatusBar.Create(this)	||!StatusBar.SetIndicators(indicators, arrlength(indicators))){\
	TRACE0("Failed to create status bar\n");return -1;}

#ifndef _BCGCONTROLBAR_STATIC_
	BCGCONTROLBARDLLEXPORT CObList	gAllSizingControlBars;
#else
	extern CObList	gAllSizingControlBars;
#endif
//BCGCONTROLBARDLLEXPORT CObList	gAllToolbars;

//////////////////////////////////////////////////////////////////////////
// CMainFrame 
//////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{ 
	m_hwndClickedTabWnd	= NULL;
}
//------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}
//------------------------------------------------------------------------
bool CMainFrame::IncludeToCmdMsgMap(CCmdTarget*	pHandler)
{
	if (!this)
		return false;

	for	(list_targets::iterator	i=m_TargetList.begin();i!=m_TargetList.end();i++)
		if ((*i)==pHandler){
			ASSERT(FALSE);
			return false;
		}
		m_TargetList.push_back(pHandler);
		return true;
}
//------------------------------------------------------------------------
bool CMainFrame::ExcludeFromCmdMsgMap(CCmdTarget* pHandler)
{
	//Удаляем из кеша
	m_TargetMap.clear();

	for	(list_targets::iterator	i=m_TargetList.begin();i!=m_TargetList.end();++i)
		if ((*i)==pHandler){
			m_TargetList.erase(i);
			return true;
		}
		ASSERT(FALSE);
		return false;
}
//------------------------------------------------------------------------
void CMainFrame::SyncInvoke(functors::IFunctorRemote & func)
{
	if (this==NULL)
		return;
	_ASSERT(AfxGetApp());
	SendMessage(CM_SYNC_INVOKE,WPARAM(&func), 0);
}
//------------------------------------------------------------------------
LRESULT CMainFrame::OnSyncInvoke(WPARAM wParam, LPARAM lParam)
{
	LOG_PROTECT_CODE_BEGIN
	functors::IFunctorRemote* pFunctor = (functors::IFunctorRemote*) wParam;
	pFunctor->invoke();
	LOG_PROTECT_CODE_END
	return 0;
}
//------------------------------------------------------------------------
void CMainFrame::ShowTip(cstr szTipText)
{
	ASSERT_RETURN_VOID(szTipText && *szTipText);
	CRect ViewRect;	m_wndStatusBar.GetItemRect(0,&ViewRect);
	ViewRect.left=2;
	m_NotifyWnd.SetPosition(&m_wndStatusBar,ViewRect);
	m_NotifyWnd.Show(szTipText);
}
//------------------------------------------------------------------------
void CMainFrame::SetStatusBarIndicators(const UINT* lpIDArray, int nIDCount, bool bInitiallyDisabled)
{
	m_wndStatusBar.SetIndicators(lpIDArray,nIDCount);
	//По большей части панели всегда выключены, поэтому по умолчанию до первого 
	//Update они все будут отключены
	if (bInitiallyDisabled)
		for (int i=1; i<nIDCount;i++) //C первого, а не с нулевого. Нулевой - оставляем
			m_wndStatusBar.SetPaneStyle(i,m_wndStatusBar.GetPaneStyle(i) | SBPS_DISABLED);

}
//------------------------------------------------------------------------
void CMainFrame::SetStatusMessage(string_adapter strText)
{
	CWnd* pMessageBar = GetMessageBar();
	ASSERT_RETURN_VOID(pMessageBar);
	pMessageBar->SetWindowText(strText.c_str());
}
//------------------------------------------------------------------------
string_smart CMainFrame::GetStatusMessage()
{
	CWnd* pMessageBar = GetMessageBar();
	ASSERT_RETURN(pMessageBar,"");
	CString str;
	pMessageBar->GetWindowText(str);
	return (LPCTSTR)str;
}
//------------------------------------------------------------------------
CView * CMainFrame::GetActiveView() const
{
	return CFrameWnd::GetActiveView();
}
//------------------------------------------------------------------------
int	CMainFrame::OnCreate(LPCREATESTRUCT	lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1){
		LOG_ERROR(MSG_ERR_SysError("Can't create MainFrame window"));
		return -1;
	}

	//Tune notify wnd
	m_NotifyWnd.Tune(true,false,DT_LEFT,RGB(255,255,255),RGB(0,0,128));

	// enable Office XP	look:
	CBCGVisualManager::SetDefaultManager (RUNTIME_CLASS	(CBCGVisualManagerXP));
	CBCGVisualManager::GetInstance ()->SetLook2000 ();  
	//--------------------------
	// Load	toolbar	user images:
	//--------------------------
	string_cstr m_strUserImagesFN = _T("UserImages.bmp");
	string_smart m_strUserImagesPath(ApplicationData::APP_DIR, m_strUserImagesFN);
#ifdef _DEBUG
	//Если отладочная версия, можно поискать в каталоге выше от нашего каталога 
	if (!is_file_exist(m_strUserImagesPath.c_str()))
		m_strUserImagesPath= string_smart(get_abs_path(_T("..\\"),ApplicationData::APP_DIR),
										m_strUserImagesFN);
#endif

	if (!m_UserImages.Load (m_strUserImagesPath.c_str()))
	{
		TRACE(_T("Failed to	load user images\n"));
		ASSERT (FALSE);
	}
	else
	{
		CBCGToolBar::SetUserImages (&m_UserImages);
	}

	CBCGToolBar::EnableQuickCustomization ();

	// TODO: Define	your own basic commands. Be	sure, that each	pulldown 
	// menu	have at	least one basic	command.

	CList<UINT,	UINT>		lstBasicCoomads;
	for	(int i=0; i<arrlength(basic_commands);i++)
		lstBasicCoomads.AddTail	(basic_commands[i]);
	CBCGToolBar::SetBasicCommands (lstBasicCoomads);

	CBCGPopupMenu::SetForceMenuFocus (FALSE);		// Menu	will not take the focus	on activation:
	CBCGMenuBar::HighlightDisabledItems	();			// Allow disabled menu items to	be highligted (VS look):
	CBCGMenuBar::SetRecentlyUsedMenus(FALSE);		// All menus visible, by default
	CBCGToolBar::AddToolBarForImageCollection (IDR_MENU_IMAGES);// Load	menu items images (not placed on the standard toolbars):
	CBCGToolbarComboBoxButton::SetFlatMode ();
	//--------------------------------------------------------------
	// Create menu bar:
	CREATE_MENUBAR(m_wndMenuBar);

	//--------------------------------------------------------------
	// Create toolbars
	CREATE_TOOLBAR(m_wndToolBar,	 ID_VIEW_TOOLBAR,		IDR_MAINFRAME,IDS_MAIN_TOOLBAR,WS_CHILD	| WS_VISIBLE | CBRS_TOP	 | CBRS_GRIPPER	| CBRS_TOOLTIPS	| CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	CREATE_TOOLBAR(m_wndToolbarBuild,ID_VIEW_BUILD_TOOLBAR,	IDR_BUILD,IDS_BUILD_TOOLBAR,WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER | CBRS_BORDER_3D);
	CREATE_TOOLBAR(m_wndToolbarDebug,ID_VIEW_DEBUG_TOOLBAR,	IDR_DEBUG,IDS_DEBUG_TOOLBAR,WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER | CBRS_BORDER_3D);	
	CREATE_TOOLBAR(m_wndToolbarEdit, ID_VIEW_EDIT_TOOLBAR,	IDR_EDIT,IDS_EDIT_TOOLBAR,WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER | CBRS_BORDER_3D);				


	//--------------------------------------------------------------
	// Create status bar: 
	CREATE_STATUSBAR(m_wndStatusBar);

	//--------------------------------------------------------------
	//Перебор всех Bar-объектов		
	//--------------------------------------------------------------
	for (i_objects::server::template_object_it<IBarLink> i = i_objects::GetServer()->GetObjectIterator(); !i.end(); i.next()) 
		(*i)->CreateWnd(this);

	EnableDocking(CBRS_ALIGN_ANY);	
 
	//Расстановка всех созданных на	стороне	Bar'ов	
	for (i_objects::server::template_object_it<IBarLink> i = i_objects::GetServer()->GetObjectIterator(); !i.end(); i.next()) 
		(*i)->InitDock();

	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndToolbarBuild);		
	DockControlBarLeftOf (&m_wndToolbarEdit, &m_wndToolBar);		
	DockControlBarLeftOf (&m_wndToolbarBuild,&m_wndToolbarEdit);
	DockControlBarLeftOf (&m_wndToolbarDebug,&m_wndToolbarBuild);	

	//Определение вложенных	ControlBar
	for	(int j=1; j<arrlength(bplsHorizontalBottom);j++)
		for	(int i=j; i<arrlength(bplsHorizontalBottom);i++)
		{
			CBCGSizingControlBar* pMasterBar =NULL;
			CBCGSizingControlBar* pSlaveBar	= NULL;
			for	(POSITION posCb	= gAllSizingControlBars.GetHeadPosition	();	posCb != NULL;)
			{
				CBCGSizingControlBar* pBar = (CBCGSizingControlBar*) gAllSizingControlBars.GetNext (posCb);
				ASSERT (pBar !=	NULL);
				CString	strCaption;		pBar ->GetWindowText(strCaption);
				if (strCaption==bplsHorizontalBottom[i-1])
					pMasterBar =pBar;
				else if	(strCaption==bplsHorizontalBottom[i])
					pSlaveBar =pBar;
			}
			if ((pMasterBar) &&	(pSlaveBar))
				DockControlBarLeftOf(pSlaveBar,pMasterBar);
		}

		//----------------------------------------
		// Allow user-defined toolbars operations:
		//----------------------------------------
		InitUserToobars	(NULL,
			uiFirstUserToolBarId,
			uiLastUserToolBarId);

		//------------------------
		// Enable windows manager:
		//------------------------
		EnableWindowsDialog	(ID_WINDOW_MANAGER,	_T("Windows..."), TRUE);

		//----------------------
		// Create vertical font:
		//----------------------
		CFont font;
		if (font.CreateStockObject (DEFAULT_GUI_FONT))
		{
			LOGFONT	lf;
			memset (&lf, 0,	sizeof (LOGFONT));

			if (font.GetLogFont	(&lf) != 0)
			{
				lf.lfOrientation = 900;
				lf.lfEscapement	= 900;

				lf.lfHeight	= -14;
				lf.lfWeight	= 700;
				lf.lfItalic	= TRUE;
				_tcscpy	(lf.lfFaceName,	_T("Arial"));

				m_fontVertCaption.CreateFontIndirect (&lf);
			}
		}
		return 0;
}
//------------------------------------------------------------------------
void CMainFrame::OnDestroy()
{
	//Событие закрытия главного кона
	i_objects::event<ARGS_0> evCloseAppMainWnd(IMainFrameLink::IEventSink::evOnCloseAppMainWnd);

	__super::OnDestroy();
	SaveCustomState();
}
//------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(	!__super::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMainFrame::LoadFrame(UINT	nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		return FALSE;

	LoadCustomState();

	//Tune MDI Tabs Wnd
	CBCGTabWnd & wndTabs = const_cast<CBCGTabWnd&>(m_wndClientArea.GetMDITabs());
	wndTabs.SetTabBorderSize(FLAT_TABS ? 0 : DEFAULT_TAB_BORDER_SIZE);
	wndTabs.SetFlatFrame (TRUE);
	wndTabs.SetNoBorder (TRUE);

	//Здесь мы определяем, работает ли MDI Tabs или нет
	EnableMDITabs (m_MDITabs.m_bMDITabs, m_MDITabs.m_bMDITabIcons, m_MDITabs.m_MDITabLocation,HIDE_NOTABS);
	return TRUE;
}
//------------------------------------------------------------------------
void CMainFrame::LoadCustomState()
{
	CBCGUserToolsManager* pUserToolsManager	= GetWorkspace()->GetUserToolsManager ();
	if (pUserToolsManager != NULL && pUserToolsManager->GetUserTools ().IsEmpty	()){
		for	(int i=0; i<arrlength(usrTools);i++)
		{
			string_list	S(usrTools[i]);
			CBCGUserTool* pTool	= pUserToolsManager->CreateNewTool ();
			pTool->m_strLabel =	S.items().get_at(0).c_str();
			pTool->SetCommand (S.items().get_at(1).c_str());
		}
	}
	m_MDITabs.m_MDITabLocation	= (CBCGTabWnd::Location) GetWorkspace()->GetInt(_T("MDITabLocation"), (int)	CBCGTabWnd::LOCATION_BOTTOM);
	m_MDITabs.m_bMDITabIcons	= GetWorkspace()->GetInt (_T("MDITabIcons"), TRUE);

	//Считываем информацию из реестра через Storage
	ASSERT_RETURN_VOID(singleton_cache::GetStorageLink());
	if (singleton_cache::GetStorageLink()->General_MDIStyle==mtdev_lib::Tabs )
		m_MDITabs.m_bMDITabs = true;
	else
		m_MDITabs.m_bMDITabs = false;
}
//------------------------------------------------------------------------
void CMainFrame::SaveCustomState()
{
	GetWorkspace()->WriteInt (_T("MDITabLocation"),	m_MDITabs.m_MDITabLocation);
	GetWorkspace()->WriteInt (_T("MDITabIcons"),m_MDITabs.m_bMDITabIcons);

	//Записываем информацию в реестр через Storage
	ASSERT_RETURN_VOID(singleton_cache::GetStorageLink());
	singleton_cache::GetStorageLink()->General_MDIStyle = (m_MDITabs.m_bMDITabs) ? mtdev_lib::Tabs : mtdev_lib::StandardMDI;
}
//------------------------------------------------------------------------
void CMainFrame::OnClose()
{
	bool bAutomated = ApplicationData::APP_AUTOMATED ||ApplicationData::APP_EMBEDDED;

	if ((!bAutomated) && (!AfxOleCanExitApp()))
		return;

	//Опрашиваем, а можно ли закрыться
	evdCloseQueryMainWnd CanClose;
	i_objects::event<ARGS_1(evdCloseQueryMainWnd &)>(IMainFrameLink::IEventSink::evOnCloseQueryAppMainWnd,CanClose);

	//Закрываем все документы
	if (CanClose.IsCanClose()){
		IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
		if ((pDocMngrLink) && (!pDocMngrLink->CloseAllDocuments())){
			CanClose.SupressClose();
		}
	}
	
	if (CanClose.IsCanClose())
	{
		ShowWindow(SW_HIDE);
		__super::OnClose();

		//Если окно почему-то не разрушилось
		if (AfxGetApp()->m_pMainWnd){
			//Мы - OLE-сервер
			if ((bAutomated) && 
				(!AfxOleCanExitApp()) && 
				(guiMsgConfirm(GetSafeHwnd(),string_res(IDS_AUTOMATION_SERVER_CLOSING,HRESOURCES).c_str())))
			{
				delete this;
				AfxPostQuitMessage(0);
			}
			else	//Восстанавливаем обратно
				ShowWindow(SW_SHOW);
		}
	}
}
//------------------------------------------------------------------------
void CMainFrame::OnStorageDataChanged(IStorageLink::EParam nParameterChanged)
{
	//MDI Style
	if (nParameterChanged == IStorageLink::pmGeneral_MDIStyle)
	{
		m_MDITabs.m_bMDITabs =  singleton_cache::GetStorageLink()->General_MDIStyle==mtdev_lib::Tabs;
		EnableMDITabs (m_MDITabs.m_bMDITabs, m_MDITabs.m_bMDITabIcons, m_MDITabs.m_MDITabLocation,HIDE_NOTABS);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext&	dc)	const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
//------------------------------------------------------------------------
void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a	customize toolbars dialog:
	//------------------------------------
	CBCGToolbarCustomize* pDlgCust = new CBCGToolbarCustomize (this,
		TRUE /*	Automatic menus	scaning	*/,
		BCGCUSTOMIZE_MENU_SHADOWS |	BCGCUSTOMIZE_TEXT_LABELS | 
		BCGCUSTOMIZE_LOOK_2000 | BCGCUSTOMIZE_MENU_ANIMATIONS |	BCGCUSTOMIZE_SELECT_SKINS | BCGCUSTOMIZE_NOHELP);

	pDlgCust->EnableUserDefinedToolbars	();

	//-----------------
	// Setup combboxes:
	//-----------------
	BEGIN_LIMITED_EDITION	
	pDlgCust->ReplaceButton	(ID_EDIT_FIND, CFindComboBoxButton());
	END_LIMITED_EDITION

	//-------------------------
	// Setup undo/redo buttons:
	//-------------------------
	CMenu menu;
	menu.LoadMenu (IDR_UNDO);

	CMenu* pPopupMenu =	menu.GetSubMenu	(0);
	VERIFY (pPopupMenu != NULL);

	pDlgCust->ReplaceButton	(ID_EDIT_UNDO, CBCGToolbarMenuButton 
		(ID_EDIT_UNDO, pPopupMenu->GetSafeHmenu	(),	
		CImageHash::GetImageOfCommand (ID_EDIT_UNDO, FALSE)));

	pDlgCust->ReplaceButton	(ID_EDIT_REDO, CBCGToolbarMenuButton 
		(ID_EDIT_REDO, pPopupMenu->GetSafeHmenu	(),	
		CImageHash::GetImageOfCommand (ID_EDIT_REDO, FALSE)));

	pDlgCust->Create ();
}
//------------------------------------------------------------------------
LRESULT	CMainFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
{
	CPoint point (BCG_GET_X_LPARAM(lp),	BCG_GET_Y_LPARAM(lp));

	CMenu menu;
	VERIFY(menu.LoadMenu (IDR_POPUP_TOOLBAR));

	CMenu* pPopup =	menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	SetupToolbarMenu (*pPopup, ID_VIEW_USER_TOOLBAR1, ID_VIEW_USER_TOOLBAR10);

	CBCGPopupMenu* pPopupMenu =	new	CBCGPopupMenu;
	pPopupMenu->SetAutoDestroy (FALSE);
	pPopupMenu->Create (this, point.x, point.y,	pPopup->GetSafeHmenu ());

	return 0;
}
//------------------------------------------------------------------------
void CMainFrame::OnToolsViewUserToolbar	(UINT uiId)
{
	CBCGToolBar* pUserToolBar =	GetUserBarByIndex (uiId	- ID_VIEW_USER_TOOLBAR1);
	if (pUserToolBar ==	NULL)
	{
		ASSERT (FALSE);
		return;
	}

	ShowControlBar (pUserToolBar, !(pUserToolBar->GetStyle () &	WS_VISIBLE), FALSE);
	RecalcLayout ();
}
//------------------------------------------------------------------------
void CMainFrame::OnUpdateToolsViewUserToolbar (CCmdUI* pCmdUI)
{
	CBCGToolBar* pUserToolBar =	GetUserBarByIndex (pCmdUI->m_nID - ID_VIEW_USER_TOOLBAR1);
	if (pUserToolBar ==	NULL)
	{
		pCmdUI->Enable (FALSE);
		return;
	}

	pCmdUI->Enable ();
	pCmdUI->SetCheck (pUserToolBar->GetStyle ()	& WS_VISIBLE);
}
//------------------------------------------------------------------------
LRESULT	CMainFrame::OnToolbarReset(WPARAM wp,LPARAM)
{
	UINT uiToolBarId = (UINT) wp;

	switch (uiToolBarId)
	{
	case IDR_EDIT:
		{
			//--------------------------------------
			// Replace "Find..." button	by combobox:
			//--------------------------------------
			BEGIN_LIMITED_EDITION
				ASSERT (m_wndToolbarEdit.GetSafeHwnd ()	!= NULL);
				m_wndToolbarEdit.ReplaceButton (ID_EDIT_FIND, CFindComboBoxButton());
			END_LIMITED_EDITION
		}
	case IDR_MAINFRAME:
		{
			//-------------------------------------
			// Replace "Undo" button by	menubutton:
			//-------------------------------------
		
			CMenu menu;	VERIFY(menu.LoadMenu (IDR_UNDO));

			CMenu* pPopupMenu =	menu.GetSubMenu	(0);
			VERIFY (pPopupMenu != NULL);

			m_wndToolBar.ReplaceButton (ID_EDIT_UNDO, CBCGToolbarMenuButton	
			(ID_EDIT_UNDO, pPopupMenu->GetSafeHmenu	(),	
			CImageHash::GetImageOfCommand (ID_EDIT_UNDO, FALSE)));

			m_wndToolBar.ReplaceButton (ID_EDIT_REDO, CBCGToolbarMenuButton	
			(ID_EDIT_REDO, pPopupMenu->GetSafeHmenu	(),	
			CImageHash::GetImageOfCommand (ID_EDIT_REDO, FALSE)));
		}
		//-------------------------------------
		// Replace "New" button by	menubutton:
		//-------------------------------------
		{
			CMenu menu; VERIFY (menu.LoadMenu (IDR_MAINFRAME));
			
			CMenu* pMenuFile = menu.GetSubMenu (0);
			VERIFY (pMenuFile != NULL);

			CMenu* pMenuNew = pMenuFile->GetSubMenu (0);
			VERIFY (pMenuNew != NULL);

			CBCGToolbarMenuButton btnNew (ID_FILE_NEW, pMenuNew->GetSafeHmenu (), 
					CImageHash::GetImageOfCommand (ID_FILE_NEW));
			m_wndToolBar.ReplaceButton (ID_FILE_NEW, btnNew);
		}		
		break;
	case IDR_BUILD:
		break;
	}

	return 0;
}
//------------------------------------------------------------------------
LRESULT	CMainFrame::OnHelpCustomizeToolbars(WPARAM wp, LPARAM lp)
{
	int	iPageNum = (int) wp;

	CBCGToolbarCustomize* pDlg = (CBCGToolbarCustomize*) lp;
	ASSERT_VALID (pDlg);

	return 0;
}
//------------------------------------------------------------------------
BOOL CMainFrame::OnShowPopupMenu (CBCGPopupMenu* pMenuPopup)
{
	CBCGMDIFrameWnd::OnShowPopupMenu (pMenuPopup);

	if (pMenuPopup == NULL)
	{
		m_hwndClickedTabWnd	= NULL;
		return TRUE;
	}

	if (pMenuPopup->GetMenuBar ()->CommandToIndex (ID_VIEW_TOOLBARS) >=	0)
	{
		if (CBCGToolBar::IsCustomizeMode ())
		{
			return FALSE;
		}

		pMenuPopup->RemoveAllItems ();

		CMenu menu;
		VERIFY(menu.LoadMenu (IDR_POPUP_TOOLBAR));

		CMenu* pPopup =	menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		SetupToolbarMenu (*pPopup, ID_VIEW_USER_TOOLBAR1, ID_VIEW_USER_TOOLBAR10);
		pMenuPopup->GetMenuBar ()->ImportFromMenu (*pPopup,	TRUE);
	}

	if (pMenuPopup->GetMenuBar ()->CommandToIndex (ID_EDIT_UNDO_DUMMY) >= 0)
	{
		if (CBCGToolBar::IsCustomizeMode ())
		{
			return FALSE;
		}

		pMenuPopup->RemoveAllItems ();
		pMenuPopup->InsertItem (CBCGToolbarMenuButton (
			IDS_DUMMY_UNDO_ACTION, NULL, -1, _T("Typing")));
	}

	int	iIndex = -1;
	if (m_wndMenuBar.GetDroppedDownMenu	(&iIndex) == 
		pMenuPopup->GetParentButton	() &&
		iIndex == (m_wndMenuBar.GetSystemMenu () ==	NULL ? 0 : 1))		// "File" menu
	{
		pMenuPopup->EnableMenuLogo (30);
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD nID   = LOWORD(wParam);
	WORD nCode = HIWORD(wParam);	
	HWND hWndCtrl = (HWND)lParam;

	// default routing for command messages (through closure table)
	if ((hWndCtrl == NULL) && (nID != 0))
	{
		CWnd* pWndFocus	= GetFocus ();
		if ((pWndFocus != NULL)	&& (pWndFocus !=this)){
			list_targets::iterator	i = std::find(m_TargetList.begin(),m_TargetList.end(),pWndFocus);
			if (i!=m_TargetList.end())
				if (((CMainFrame*)pWndFocus)->OnCommand(wParam,lParam))	//Преобразование из-за того, что компилятор глючит с access to protected
					return TRUE;
		}
	}

	return CMDIFrameWnd::OnCommand(wParam, lParam);
}
//------------------------------------------------------------------------
BOOL CMainFrame::OnCmdMsg(UINT nID,	int	nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	BOOL  res =	false;

	LOG_PROTECT_CODE_BEGIN //основной try для механизма	сообщенгий всей	программы

	//Если есть	элемент	с фокусом, аытаемся	определить,	к чему он относится	и обработать команду	
	//Есть здесь одно слабое место - предполагается, что переданный CmdTarget - 
	//тот саиый, который базовый для CWnd. Иначе эта проверка работать не будет, нужно использовать
	//медленный DYNAMIC_CAST
	CWnd* pWndFocus	= GetFocus ();
	if ((pWndFocus != NULL)	&& (pWndFocus !=this)){
//		list_targets::iterator	i = find(m_TargetList.begin(),m_TargetList.end(),pWndFocus);
//		if (i!=m_TargetList.end())
//			if ((*i)->OnCmdMsg	(nID, nCode, pExtra, pHandlerInfo))
		if (pWndFocus->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
	}

	res=CMDIFrameWnd::OnCmdMsg(nID,	nCode, pExtra, pHandlerInfo);

	if ((!res) && AfxGetMainWnd())	//Его уже могли разрушить
	{
		__int64 iKey = __int64(nID) | (__int64(nCode)<<32);
		CCmdTarget * pTarget=NULL;

		//Попробуем найти в кеше
		map_targets::iterator i = m_TargetMap.find(iKey);
		if (i!=m_TargetMap.end())
		{
			cache_data & data = (*i).second;
			//ПРежний раз ничего не нашли, делаем пауза на 0.1 сек
			if (data.m_pTarget==NULL) {
				if (clock()-data.m_nLastUpdate<CLOCKS_PER_SEC/10)
					return false;
			}
			else {
				pTarget = data.m_pTarget;
				res = pTarget->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
			}
		}

		//Кеш не помог, придется перебирать все подряд
		if (!res)
		{
			for	(list_targets::iterator	i=m_TargetList.begin();(!res)&&(i!=m_TargetList.end());i++)
			{
				if ((*i)==pTarget) //Это мы уже обрабатывали
					continue;
				res	= (*i)->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
				if (res)
					pTarget = (*i);
			}

			if (res)
				m_TargetMap[iKey]=cache_data(pTarget,clock());
			else
				m_TargetMap[iKey]=cache_data(NULL,clock());
		}
	}

	
	LOG_PROTECT_CODE_END
	return res;
}
//------------------------------------------------------------------------
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LOG_PROTECT_CODE_BEGIN
	if (message==WM_MOUSEMOVE){
		TRACE1("%d",message);
	}
	return __super::WindowProc(message,wParam,lParam);
	LOG_PROTECT_CODE_END
	return 0;
}
//------------------------------------------------------------------------
void CMainFrame::OnDrawMenuLogo	(CDC* pDC, CBCGPopupMenu* pMenu, const CRect& rectLogo)
{
	ASSERT_VALID (pDC);

	CBCGDrawManager	dm (*pDC);
	dm.FillGradient	(rectLogo, RGB (0, 128,	128), RGB (0, 0, 128));

	pDC->Draw3dRect	(rectLogo, 
		::GetSysColor (COLOR_3DLIGHT), 
		::GetSysColor (COLOR_3DSHADOW));

	int	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldText	= pDC->SetTextColor	(RGB (255, 255,	255));

	CFont* pOldFont	= pDC->SelectObject	(&m_fontVertCaption);

	CPoint ptOrg = CPoint(rectLogo.left	+ 5, rectLogo.bottom - 10);

	pDC->ExtTextOut	(ptOrg.x, ptOrg.y, ETO_CLIPPED,	rectLogo, ApplicationData::APP_NAME, NULL);

	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nOldBkMode);
	pDC->SetTextColor(clrOldText);
}
//------------------------------------------------------------------------
void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd->GetSafeHwnd () ==	m_wndClientArea.GetSafeHwnd	())
	{
		OnToolbarContextMenu (0, MAKELPARAM(point.x, point.y));
	}
	else if	(pWnd->GetSafeHwnd () == m_wndClientArea.GetMDITabs	().GetSafeHwnd ())
	{
		const CBCGTabWnd& wndTab = m_wndClientArea.GetMDITabs ();

		CRect rectTabs;
		wndTab.GetTabsRect (rectTabs);

		CPoint ptTab = point;
		wndTab.ScreenToClient (&ptTab);

		int	iTab = wndTab.GetTabFromPoint (ptTab);
		if (iTab >=	0)
		{
			m_hwndClickedTabWnd	= wndTab.GetTabWnd (iTab)->GetSafeHwnd ();
		}

		GetWorkspace()->ShowPopupMenu (IDR_POPUP_MDI_TABS, point, pWnd);
	}
}
//------------------------------------------------------------------------
void CMainFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	//Если меняется видимость окна, оповещаем всех Bar
	if (lpwndpos->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW)) 
		for (i_objects::server::template_object_it<IBarLink> i = i_objects::GetServer()->GetObjectIterator(); !i.end(); i.next()) 
			(*i)->OnShowMainFrame((lpwndpos->flags & SWP_SHOWWINDOW)!=FALSE);
}
//------------------------------------------------------------------------
void CMainFrame::OnMditabsIcons() 
{
	m_MDITabs.m_bMDITabIcons = !m_MDITabs.m_bMDITabIcons;
	EnableMDITabs (m_MDITabs.m_bMDITabs, m_MDITabs.m_bMDITabIcons, m_MDITabs.m_MDITabLocation,HIDE_NOTABS);
}

void CMainFrame::OnUpdateMditabsIcons(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (m_MDITabs.m_bMDITabs);
	pCmdUI->SetCheck (m_MDITabs.m_bMDITabIcons);
}

void CMainFrame::OnMditabsTop()	
{
	m_MDITabs.m_MDITabLocation = CBCGTabWnd::LOCATION_TOP;
	EnableMDITabs (m_MDITabs.m_bMDITabs, m_MDITabs.m_bMDITabIcons, m_MDITabs.m_MDITabLocation,HIDE_NOTABS);
}

void CMainFrame::OnUpdateMditabsTop(CCmdUI*	pCmdUI)	
{
	pCmdUI->Enable (m_MDITabs.m_bMDITabs);
	pCmdUI->SetRadio (m_MDITabs.m_MDITabLocation ==	CBCGTabWnd::LOCATION_TOP);
}

void CMainFrame::OnMditabsBottom() 
{
	m_MDITabs.m_MDITabLocation = CBCGTabWnd::LOCATION_BOTTOM;
	EnableMDITabs (m_MDITabs.m_bMDITabs, m_MDITabs.m_bMDITabIcons, m_MDITabs.m_MDITabLocation,HIDE_NOTABS);
}

void CMainFrame::OnUpdateMditabsBottom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (m_MDITabs.m_bMDITabs);
	pCmdUI->SetRadio (m_MDITabs.m_MDITabLocation ==	CBCGTabWnd::LOCATION_BOTTOM);
}

void CMainFrame::cmdOnViewMdiTabs()	
{
	m_MDITabs.m_bMDITabs = !m_MDITabs.m_bMDITabs;
	EnableMDITabs (m_MDITabs.m_bMDITabs, m_MDITabs.m_bMDITabIcons, m_MDITabs.m_MDITabLocation,HIDE_NOTABS);
}

void CMainFrame::cmdupOnViewMdiTabs(CCmdUI*	pCmdUI)	
{
	pCmdUI->SetCheck (m_MDITabs.m_bMDITabs);
}

void CMainFrame::cmdOnMditabsClose() 
{
	ASSERT (m_hwndClickedTabWnd	!= NULL);
	::PostMessage (m_hwndClickedTabWnd,	WM_CLOSE, 0, 0);
}

void CMainFrame::OnUpdateMditabsClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (m_hwndClickedTabWnd	!= NULL);
}
//============================== ACTION	HANDLERS =========================
void CMainFrame::cmdOnViewBuildToolbar() {
	ShowControlBar (&m_wndToolbarBuild,		!guiIsWindowVisible(&m_wndToolbarBuild), FALSE);
}
void CMainFrame::cmdupOnViewBuildToolbar(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck (guiIsWindowVisible(&m_wndToolbarBuild));
}
void CMainFrame::cmdOnViewDebugToolbar(){
	ShowControlBar (&m_wndToolbarDebug,		!guiIsWindowVisible(&m_wndToolbarDebug),FALSE);
}
void CMainFrame::cmdupOnViewDebugToolbar(CCmdUI	*pCmdUI){
	pCmdUI->SetCheck (guiIsWindowVisible(&m_wndToolbarDebug));
}
void CMainFrame::cmdOnViewEditToolbar(){
	ShowControlBar (&m_wndToolbarEdit,		!guiIsWindowVisible(&m_wndToolbarEdit),FALSE);
}
void CMainFrame::cmdupOnViewEditToolbar(CCmdUI *pCmdUI){
	pCmdUI->SetCheck	(guiIsWindowVisible(&m_wndToolbarEdit));
}

