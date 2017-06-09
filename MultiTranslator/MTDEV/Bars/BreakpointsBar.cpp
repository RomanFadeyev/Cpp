/********************************************************************
	created:	2002/10/08
	created:	8:10:2002   22:54
	filename: 	$(Projects)\sources\mtdev\bars\breakpointsbar.cpp
	file path:	$(Projects)\sources\mtdev\bars
	file base:	breakpointsbar
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Docked-Bar для точек останова
*********************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "BreakpointsBar.h"

//Создание переходника для объекта
static i_objects::object_thunk<CbarBreakpoints> barBreakpoints(__MSG_PREFIX__);

#define WM_CHANGE_BREAKPOINTS 1
#define COL_NAME_MODULE _T("Module")
#define COL_NAME_LINE	_T("Line")
#define COL_NAME_STATUS _T("Status")
#define BREAKPOINT_STATUS(x) (x) ? _T("Enabled") : _T("Disabled")

//------------------------------------------------------------------------
#pragma warning( disable : 4355) //using "this"in member intialization list
CbarBreakpoints::CbarBreakpoints(): m_wndView(this)
{
}
//------------------------------------------------------------------------
CbarBreakpoints::~CbarBreakpoints()
{
}
//------------------------------------------------------------------------
void CbarBreakpoints::DoDelegate(){
	IBarLink * pBar = i_objects::GetInterface<IBarLink>(IBarLink::idBarWatch);
	CBCGTabWnd * pTabWnd = (pBar) ? pBar->GetTabWnd() : NULL;
	Delegate(&m_wndView,pTabWnd);
}
//------------------------------------------------------------------------
void CbarBreakpoints::OnCreateAppMainWnd(){
	i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);
	if ((AfxGetApp()->GetProfileInt(szProfileBarBreakpoints,szParamIsDelegated,true)))
		DoDelegate();
}
//------------------------------------------------------------------------
void CbarBreakpoints::OnCloseAppMainWnd(){
	CWinApp* pApp = AfxGetApp();
	if (IsDelegated()){
		int a=0;
	}
	pApp->WriteProfileInt(szProfileBarBreakpoints,szParamIsDelegated,IsDelegated());
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CbarBreakpoints, Cbar_B)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_BREAKPOINTS, cmdOnViewBreakpoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BREAKPOINTS, cmdupOnViewBreakpoints)
	ON_COMMAND(ID_INDEPENDENT_BAR_BREAKPOINTS, cmdOnIndependentBar)
	ON_UPDATE_COMMAND_UI(ID_INDEPENDENT_BAR_BREAKPOINTS, cmdupOnIndependentBar)
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarBreakpoints::CreateWnd(CFrameWnd* pParentWnd)
{
	__super::CreateWnd(pParentWnd);
	ModifyStyle(0,WS_VISIBLE,0); 
	m_wndView.Create(WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL|ES_MULTILINE , CRect(0,0,100,100), this, 1);
}
//------------------------------------------------------------------------
void CbarBreakpoints::OnSize(UINT nType, int cx, int cy) 
{
    __super::OnSize(nType, cx, cy);
	if (m_wndView.m_hWnd)
		m_wndView.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER | SWP_NOREPOSITION );
}
//-------------------------------------------------------------------------
void CbarBreakpoints::cmdOnViewBreakpoints()
{
	ToggleVisible();
}
//-------------------------------------------------------------------------
void CbarBreakpoints::cmdupOnViewBreakpoints(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(IsVisible());
}
//------------------------------------------------------------------------
void CbarBreakpoints::cmdupOnIndependentBar(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(!IsDelegated());
}
//------------------------------------------------------------------------
void CbarBreakpoints::cmdOnIndependentBar(){
	if (IsDelegated())
		UnDelegate();
	else 
		DoDelegate();
}
/////////////////////////////////////////////////////////////////////////////
// CbarBreakpoints::CBPView
/////////////////////////////////////////////////////////////////////////////
CbarBreakpoints::CBPView::CBPView(CBCGSizingControlBar * pBar)
{
	m_pParentBar		= pBar;
}
//------------------------------------------------------------------------
CbarBreakpoints::CBPView::~CBPView()
{
	SaveSettings();
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::SaveSettings()
{
    CWinApp* pApp = AfxGetApp();
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::LoadSettings()
{
    CWinApp* pApp = AfxGetApp();
}
//------------------------------------------------------------------------
IProjectDocLink * CbarBreakpoints::CBPView::GetActiveProjectIface()
{
	IDocMngrLink* pDocMngrLink=i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_NULL(pDocMngrLink);
	
	IProjectDocLink * pProjectDocLink = pDocMngrLink->GetActiveProject();
	return pProjectDocLink;
}
//------------------------------------------------------------------------
CbarBreakpoints::CBPView::BP_INFO CbarBreakpoints::CBPView::GetBpInfo(long nItemIndex)
{
	_ASSERT(GetItemCount()==m_Items.size());
	if (!inrange(nItemIndex,0L,(long)GetItemCount()-1))
		throw(exception(MSG_ERR_IndexOutOfBounds(nItemIndex)));
	nItemIndex = GetItemData(nItemIndex);
	return m_Items[nItemIndex];
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::AddBpInfo(BP_INFO & bp)
{
	ASSERT(bp.m_strSource.c_str());

	int ItemIndex = InsertItem(GetItemCount(),"");
	m_Items.push_back(bp);
	SetItemData(ItemIndex,m_Items.size()-1);

	SetItemText(ItemIndex,colModule,bp.m_strSource.c_str());
	SetItemText(ItemIndex,colLine,itos(bp.m_nLine+1).c_str());
	SetItemText(ItemIndex,colStatus,BREAKPOINT_STATUS(bp.m_bEnabled));
}

//------------------------------------------------------------------------
typedef CbarBreakpoints::CBPView CbarBPView;
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CbarBPView, CListCtrlEx)
//BEGIN_MESSAGE_MAP(CbarBreakpoints::CBPView, CListCtrlEx)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()	
	ON_WM_SIZE()
	ON_WM_KEYDOWN()	
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_MESSAGE(CM_POST,OnPostedMessage)		
	ON_COMMAND(ID_BREAKPOINTS_GOTO, cmdOnBreakpointsGoto)
	ON_UPDATE_COMMAND_UI(ID_BREAKPOINTS_GOTO, cmdupOnBreakpointsGoto)
	ON_COMMAND(ID_BREAKPOINTS_DELETE, cmdOnBreakpointsDelete)
	ON_UPDATE_COMMAND_UI(ID_BREAKPOINTS_DELETE, cmdupOnBreakpointsDelete)
	ON_COMMAND(ID_BREAKPOINTS_DISABLE_ALL, cmdOnBreakpointsDisableAll)
	ON_UPDATE_COMMAND_UI(ID_BREAKPOINTS_DISABLE_ALL, cmdupOnBreakpointsDisableAll)
	ON_COMMAND(ID_BREAKPOINTS_CLEAR_ALL, cmdOnBreakpointsClearAll)
	ON_UPDATE_COMMAND_UI(ID_BREAKPOINTS_CLEAR_ALL, cmdupOnBreakpointsClearAll)
	ON_COMMAND(ID_BREAKPOINTS_ENABLE_ALL, cmdOnBreakpointsEnableAll)
	ON_UPDATE_COMMAND_UI(ID_BREAKPOINTS_ENABLE_ALL, cmdupOnBreakpointsEnableAll)
	ON_COMMAND(ID_BREAKPOINTS_DISABLE, cmdOnBreakpointsDisable)
	ON_UPDATE_COMMAND_UI(ID_BREAKPOINTS_DISABLE, cmdupOnBreakpointsDisable)
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (GetCurrentItem()!=-1)
		GotoBreakpoint(GetBpInfo(GetCurrentItem()));
	*pResult = 0;
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
	if ((nChar==VK_DELETE) && (GetCurrentItem()!=-1)) {
		long NextItem = min(GetCurrentItem(),GetItemCount()-2);
		RemoveBreakpoint(GetBpInfo(GetCurrentItem()));
		SelectItem(NextItem);
	}
}
//------------------------------------------------------------------------
LRESULT CbarBreakpoints::CBPView::OnPostedMessage(WPARAM wParam, LPARAM lParam)
{
	//Если в очерели сообщений есть еще такие же сообщения, данную обработку опускаем
	//Т.О. мы всегда реагируем на самое последнее сообщение
	MSG Msg;
	if (PeekMessage(&Msg, m_hWnd, CM_POST,CM_POST, PM_NOREMOVE))
		return 0;
	//Вычищаем очередь: зачем на лишние циклы? Это вместо вышеприведенного кода, только реагируем на самое первое сообщение
	//for (MSG Msg; PeekMessage(&Msg, m_hWnd, CM_POST,CM_POST, PM_REMOVE););
	SetBreakpoints();
	return 0;
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::GotoBreakpoint(BP_INFO & bpinfo)
{
	IProjectDocLink * pProjectDocLink = GetActiveProjectIface();
	if (!pProjectDocLink)
		return;

	CDocument *	pDoc =AfxGetApp()->OpenDocumentFile(bpinfo.m_strSource.c_str());
	ISourceDocLink*	pDocIface;
	ISourceViewLink* pViewIface;

	if ((pDoc) && (pDocIface = dynamic_cast <ISourceDocLink *> (pDoc )) && (pViewIface = pDocIface->GetViewLink()))
	{
		pViewIface->GotoLine(bpinfo.m_nLine);
		pViewIface->GetObject()->SetFocus();
	}
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::EnableBreakpoint(BP_INFO & bpinfo, bool bEnable)
{
	IProjectDocLink * pProjectDocLink = GetActiveProjectIface();
	if (!pProjectDocLink)
		return;

	BREAKPOINT bp(NULL,bpinfo.m_strSource.c_str());	

	//Удаление осуществляем через документ проекта
	if (pProjectDocLink->GetBreakPoint(bpinfo.m_strSource.c_str(),bpinfo.m_nLine,bp)){
		bp.Enable(bEnable);
		pProjectDocLink->SetBreakPoint(bpinfo.m_strSource.c_str(),bp);
	}
	
	Refresh();	//Иногда это и не нужно, т.к. вызывает evOnChangeDocument, а иногда необходимо
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::EnableBreakpoints(bool bEnable)
{
	//Сначала все точки нужно скопировать, потому что потом начнется каша из событий
	bpinfo_vector Breakpoints;
	Breakpoints.assign(m_Items.begin(),m_Items.end());
	
	//Удаление осуществляем через документ проекта
	for (bpinfo_v_i i=Breakpoints.begin();i!=Breakpoints.end();++i)
		EnableBreakpoint((*i),bEnable);
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::RemoveBreakpoint(BP_INFO & bpinfo)
{
	IProjectDocLink * pProjectDocLink = GetActiveProjectIface();
	if (!pProjectDocLink)
		return;

	//Удаление осуществляем через документ проекта
	pProjectDocLink->ResetBreakPoint(bpinfo.m_strSource.c_str(),bpinfo.m_nLine);
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::RemoveBreakpoints()
{
	//Сначала все точки нужно скопировать, потому что потом начнется каша из событий
	bpinfo_vector Breakpoints;
	Breakpoints.assign(m_Items.begin(),m_Items.end());
	
	//Удаление осуществляем через документ проекта
	for (bpinfo_v_i i=Breakpoints.begin();i!=Breakpoints.end();++i)
		RemoveBreakpoint((*i));
}
//------------------------------------------------------------------------
static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to	the	list view control.
	CListCtrl* pListCtrl = (CListCtrl*)	lParamSort;
   
	CString	   strItem11 = pListCtrl->GetItemText(lParam1, CbarBreakpoints::CBPView::colModule);
	CString	   strItem12 = pListCtrl->GetItemText(lParam1, CbarBreakpoints::CBPView::colLine);
   
	CString	   strItem21 = pListCtrl->GetItemText(lParam2, CbarBreakpoints::CBPView::colModule);
	CString	   strItem22 = pListCtrl->GetItemText(lParam2, CbarBreakpoints::CBPView::colLine);

	int	res = strcmp(strItem21, strItem11);
	if (res==0)
		res = atoi(strItem12)-atoi(strItem22);
	return res;
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::Refresh()
{
	//Не надо обновлять информацию тут же, стоит чуть подождать,
	//пока пройдут все события (это надежнее и исключает холостые циклы)
	PostMessage(CM_POST,WM_CHANGE_BREAKPOINTS,0);	
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::GetEnabledDisabledCount(long & nEnabled, long & nDisabled)
{
	nEnabled=nDisabled=0;
	//Удаление осуществляем через документ проекта
	for (bpinfo_v_i i=m_Items.begin();i!=m_Items.end();++i)
		if ((*i).m_bEnabled)
			nEnabled++;
		else
			nDisabled++;
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::SetBreakpoints()
{
	BeginUpdate();
	long nCurItem = GetCurrentItem();

	LOG_PROTECT_CODE_BEGIN
	DeleteAllItems();
	m_Items.clear();
	
	IProjectDocLink * pProjectDocLink = GetActiveProjectIface();
	if (pProjectDocLink)	//Это не является ошибкой
	{						//Вполне может быть, что активного проекта уже нет (мы ведь работем через PostMessage)
							//Не ставить return,есть  общий код внизу
		IDebugManageLink::vector_breakpoints Breakpoints;
		pProjectDocLink->GetBreakPoints(Breakpoints);

		for (IDebugManageLink::vector_breakpoints::iterator j=Breakpoints.begin(); j!=Breakpoints.end();++j)
			AddBpInfo(BP_INFO(*j));
		
		SortItems(CompareProc, (LPARAM) this);
		if (inrange(nCurItem,0L,(long)GetItemCount()-1))
			SelectItem(nCurItem);
	}
	LOG_PROTECT_CODE_END
	EndUpdate();
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::SetColumnsWidth()
{
//	if (m_hWnd)
//		FitLastColumn();
}
/////////////////////////////////////////////////////////////////////////////
void CbarBreakpoints::CBPView::evOnOpenDocument(evdOpenDocumentInfo &Info)
{
	//Мы обрабатываем только события открытия проекта. Остальные пропускаем
	if (Info.in_nDocID==evdOpenProjectInfo::idProject)
		Refresh();	
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::evOnCloseDocument(evdCloseDocumentInfo &Info)
{
	//Мы обрабатываем только события открытия проекта. Остальные пропускаем
	if (Info.in_nDocID==evdCloseProjectInfo::idProject)
		Refresh();
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::evOnChangeDocument(evdChangeDocumentInfo &Info)
{
	if (Info.in_nChangeType==evdChangeSourceDocInfo::changeBreakpoints)
		Refresh();
}
//------------------------------------------------------------------------
BOOL CbarBreakpoints::CBPView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=LVS_NOSORTHEADER | LVS_REPORT;
	return CListCtrlEx::PreCreateWindow(cs);
}
//------------------------------------------------------------------------
int CbarBreakpoints::CBPView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(WS_BORDER,0);
	ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE,0);
	AddColumn(COL_NAME_MODULE,LVCFMT_LEFT,200);
	AddColumn(COL_NAME_LINE,LVCFMT_LEFT,100);
	AddColumn(COL_NAME_STATUS,LVCFMT_LEFT,100);
	SetExtendedStyle(LVS_EX_LABELTIP | LVS_EX_FULLROWSELECT);
	return 0;
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	SetColumnsWidth();
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	//Так мы скрываем элементы меню
	class CBCGToolBarEx: public CBCGToolBar{
		friend void CbarBreakpoints::CBPView::OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	};
	//Удаляем предыдущие настройки
	POSITION pos = CBCGToolBarEx::m_lstUnpermittedCommands.Find(ID_BREAKPOINTS_DISABLE_ALL);
	if (pos!=NULL)
		CBCGToolBarEx::m_lstUnpermittedCommands.RemoveAt(pos);
	pos = CBCGToolBarEx::m_lstUnpermittedCommands.Find(ID_BREAKPOINTS_ENABLE_ALL);
	if (pos!=NULL)
		CBCGToolBarEx::m_lstUnpermittedCommands.RemoveAt(pos);
	
	//Устанавливаем новые
	long nE,nD;
	GetEnabledDisabledCount(nE,nD);
	if (nE==0)
		CBCGToolBarEx::m_lstUnpermittedCommands.AddTail(ID_BREAKPOINTS_DISABLE_ALL);
	if (nD==0)
		CBCGToolBarEx::m_lstUnpermittedCommands.AddTail(ID_BREAKPOINTS_ENABLE_ALL);

	GetWorkspace()->ShowPopupMenu (IDR_POPUP_BREAKPOINTS, point,AfxGetMainWnd());
}
//-------------------------------------------------------------------------
//----CbarBreakpoints::CBPView: ACTION EVENTS-----------------------------
//-------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdOnBreakpointsGoto()
{
	ASSERT_RETURN_VOID(GetCurrentItem()!=-1);
	GotoBreakpoint(GetBpInfo(GetCurrentItem()));
}
//-------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdupOnBreakpointsGoto(CCmdUI *pCmdUI){
	pCmdUI->Enable(GetCurrentItem()!=-1);}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdOnBreakpointsDelete()
{
	ASSERT_RETURN_VOID(GetCurrentItem()!=-1);
	RemoveBreakpoint(GetBpInfo(GetCurrentItem()));
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdupOnBreakpointsDelete(CCmdUI *pCmdUI){
	pCmdUI->Enable(GetCurrentItem()!=-1);}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdOnBreakpointsDisable(){
	ASSERT_RETURN_VOID(GetCurrentItem()!=-1);
	EnableBreakpoint(GetBpInfo(GetCurrentItem()),!GetBpInfo(GetCurrentItem()).m_bEnabled);
}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdupOnBreakpointsDisable(CCmdUI *pCmdUI){
	pCmdUI->Enable(GetItemCount()>0);}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdOnBreakpointsDisableAll(){
	EnableBreakpoints(false);}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdupOnBreakpointsDisableAll(CCmdUI *pCmdUI){
	pCmdUI->Enable(GetItemCount()>0);}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdOnBreakpointsEnableAll(){
	EnableBreakpoints(true);}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdupOnBreakpointsEnableAll(CCmdUI *pCmdUI){
	pCmdUI->Enable(GetItemCount()>0);}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdOnBreakpointsClearAll(){
	RemoveBreakpoints();}
//------------------------------------------------------------------------
void CbarBreakpoints::CBPView::cmdupOnBreakpointsClearAll(CCmdUI *pCmdUI){
	pCmdUI->Enable(GetItemCount()>0);}
