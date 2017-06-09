/********************************************************************
	created:	2002/05/11
	created:	11:5:2002   17:28
	filename: 	$(Projects)\sources\mtdev\bars\watchbar.cpp
	file path:	$(Projects)\sources\mtdev\bars

	file base:	watchbar
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Docked-Bar для отладчика
*********************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "WatchBar.h"

//Создание переходника для объекта
static i_objects::object_thunk<CbarWatch> barWatch(__MSG_PREFIX__);

long  CbarWatch::CWatchListN::m_nWatches=0;

//Get default font
static CAutoFont & GetDefaultGuiFont()
{
	static CAutoFont Font;
	static bool bInited=false;
	
	if (!bInited){
		bInited=true;
		Font.CreateStockObject (DEFAULT_GUI_FONT);
	}
	return Font;
};


///////////////////////////////////////////////////////////////////////////
// CbarWatch::CStackList_B
///////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
typedef CbarWatch::CStackList_B CbarWatch_CStackList_B;
BEGIN_MESSAGE_MAP(CbarWatch_CStackList_B, CListBoxEx)
	ON_MESSAGE_VOID(CM_REALIGN,msgOnRealign)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, cmdOnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, cmdupOnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, cmdOnEditClear)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

//------------------------------------------------------------------------
WATCHITEM CbarWatch::CStackList_B::GetItem(size_t nIndex)
{
	return WATCHITEM(); //не реализовано
}
//------------------------------------------------------------------------
void CbarWatch::CStackList_B::SetItems(WATCHITEM::vector_items & WatchItems)
{
	BeginUpdate();
	LOG_PROTECT_CODE_BEGIN
	Clear();
	for (size_t i=0; i<WatchItems.size();i++){
		InsertString(0,""),
		SetItem(0,WatchItems[i]);
	}
	LOG_PROTECT_CODE_END
	EndUpdate();
}			
//------------------------------------------------------------------------
void CbarWatch::CStackList_B::SetItem(size_t nIndex, const WATCHITEM & WatchItem)
{
	string_smart s;
	if (WatchItem.nCol==-1)
		s = Format("%s, %s, %d",WatchItem.strName.c_str(),WatchItem.strContext.c_str(),WatchItem.nLine);
	else
		s = Format("%s, %s, <%d,%d>",WatchItem.strName.c_str(),WatchItem.strContext.c_str(),WatchItem.nLine,WatchItem.nCol);

	DeleteString(nIndex);
	InsertString(nIndex,s.c_str());
	SetItemData(nIndex,MAKELONG(WatchItem.nLine,WatchItem.nCol));
}
//-------------------------------------------------------------------------
bool CbarWatch::CStackList_B::LocateToPos(string_smart strFileName, long nLine, long nPos)
{
	CDocument* pDoc = AfxGetApp()->OpenDocumentFile(strFileName.c_str());
	//Приводим к документу исходников (полученный документ должен быть с интерфейсом ISourceDocLink )
	ISourceDocLink * pDocIface = dynamic_cast <ISourceDocLink *>(pDoc);
	if (!pDoc)
		return false;
	pDocIface->GetViewLink()->GetObject()->SetFocus();
	pDocIface->GetViewLink()->GotoPos(nLine,nPos);
	return true;
}
//------------------------------------------------------------------------	
void CbarWatch::CStackList_B::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);
	if (GetCurSel()==-1)	//Не убирать
		return;				//

	CString Str; 
	CListBox::GetText(GetCurSel(),Str);	

	string_smart s1(Str.GetLength()),s2(Str.GetLength());
	sscanf(Str,"%[^,], %[^,]%s",s1.buffer(),s2.buffer(),s1.buffer()); //см SetItems

	DWORD nPos = GetItemData(GetCurSel());
	LocateToPos(s2,LOWORD(nPos),max(LOWORD(nPos),0));
}
//------------------------------------------------------------------------
BOOL CbarWatch::CStackList_B::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=LBS_NOINTEGRALHEIGHT |  WS_HSCROLL  | WS_VSCROLL;
	return __super::PreCreateWindow(cs);
}
//------------------------------------------------------------------------
int CbarWatch::CStackList_B::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Устанавливаем шрифт
	SetOwner (&m_Owner);
	SetFont(&GetDefaultGuiFont());
	return 0;
}
//------------------------------------------------------------------------
void CbarWatch::CStackList_B::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	__super::OnWindowPosChanging(lpwndpos);
	// Hide horizontal scrollbar:
	ShowScrollBar (SB_HORZ, FALSE);
	ModifyStyle (WS_HSCROLL, 0, SWP_DRAWFRAME);
}
//------------------------------------------------------------------------
void CbarWatch::CStackList_B::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	SetFocus(); //Не убирать
	GetWorkspace()->ShowPopupMenu (IDR_POPUP_OUTPUT, point, AfxGetMainWnd ());
}
//------------------------------------------------------------------------	
void CbarWatch::CStackList_B::msgOnRealign()
{
	CClientDC dc (&m_Owner);
	CFont* pOldFont = dc.SelectObject (&GetDefaultGuiFont());

	int cxExtentMax = 0;
	for (int i = 0; i < GetCount (); i ++)
	{
		CString strItem; CListBox::GetText (i, strItem);
		cxExtentMax = max (cxExtentMax, dc.GetTextExtent (strItem).cx);
	}

	SetHorizontalExtent (cxExtentMax);
	dc.SelectObject (pOldFont);
}
//------------------------------------------------------------------------
void CbarWatch::CStackList_B::cmdOnEditCopy() 
{
	long nLen=1;
	for (int i =0; i<GetCount();i++)
		nLen+=__super::GetTextLen(i);

	string_smart strText(nLen);
	for (int i =0; i<GetCount();i++){
		CString ss; __super::GetText(i,ss);
		strText.cat(ss);
	}

	if (OpenClipboard ())
	{
		EmptyClipboard ();

		HGLOBAL hClipbuffer = ::GlobalAlloc (GMEM_DDESHARE, strText.length() + 1);
		LPTSTR lpszBuffer = (LPTSTR) GlobalLock (hClipbuffer);

		_tcscpy (lpszBuffer, strText.c_str());

		::GlobalUnlock (hClipbuffer);
		::SetClipboardData (CF_TEXT, hClipbuffer);

		CloseClipboard();
	}
}
//------------------------------------------------------------------------
void CbarWatch::CStackList_B::cmdupOnEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(	(::GetFocus()==m_hWnd) &&
		(GetCount()>0) && 
		((GetCount()>1) || __super::GetTextLen(0)>0));
}
//------------------------------------------------------------------------
void CbarWatch::CStackList_B::cmdOnEditClear() 
{
}
//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//--- CbarWatch::CWatchList_B
//////////////////////////////////////////////////////////////////////////

typedef CbarWatch::CWatchList_B CbarWatchList;
BEGIN_MESSAGE_MAP(CbarWatchList, CTreeListCtrl)
//BEGIN_MESSAGE_MAP(CbarWatch::CWatchList_B , CTreeListCtrl)
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_LBUTTONDOWN()
    ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
//------------------------------------------------------------------------
CbarWatch::CWatchList_B::CWatchList_B()
{
}
//------------------------------------------------------------------------
CbarWatch::CWatchList_B::~CWatchList_B()
{
}
//------------------------------------------------------------------------
WATCHITEM CbarWatch::CWatchList_B::GetItem(size_t nIndex)
{
	return WATCHITEM();
}
void CbarWatch::CWatchList_B::Clear(){
	if (!m_hWnd) return;
	BeginUpdate();
	DeleteAllFRows();
	EndUpdate();
}
//------------------------------------------------------------------------
void CbarWatch::CWatchList_B::SetItem(size_t nIndex, const WATCHITEM & WatchItem)
{
	_ASSERT(nIndex<GetItemCount());
	HFROW hRow= GetChildFRowAt(FL_ROOT,nIndex);
}
//------------------------------------------------------------------------
void CbarWatch::CWatchList_B::SetItems(WATCHITEM::vector_items & WatchItems)
{
	BeginUpdate();
	LOG_PROTECT_CODE_BEGIN
	Clear();
	for (size_t i=0; i<WatchItems.size();i++)
		SetItem(FL_ROOT,WatchItems[i]);
	LOG_PROTECT_CODE_END
	EndUpdate();
}			
//------------------------------------------------------------------------
void CbarWatch::CWatchList_B::SetItem(HFROW hStart, const WATCHITEM & WatchItem)
{
	hStart = InsertFRow(hStart,FL_LAST, WatchItem.strName.c_str());
	SetFItemImage(hStart,0,5);
	SetFItemText(hStart, 3, WatchItem.strType.c_str()); 
	SetFItemText(hStart, 4, WatchItem.strContext.c_str()); 
	switch (WatchItem.nKind){
		case WATCHITEM::wikSimple:
			SetFItemText(hStart, 2, WatchItem.value.as_cstr()); 
			break;
		case WATCHITEM::wikStruct:
			for (size_t i=0;i<WatchItem.ChildItems.size();i++){
				//HFROW hChildRow = InsertFRow(hStart, FL_LAST,"");
				SetItem(hStart,WatchItem.ChildItems[i]);
			}
			break;
		case WATCHITEM::wikArray:
			for (size_t i=0;i<WatchItem.ChildItems.size();i++){
				//HFROW hChildRow = InsertFRow(hStart, FL_LAST,"");
				SetItem(hStart,WatchItem.ChildItems[i]);
			}
			break;
	}
}
//------------------------------------------------------------------------
void CbarWatch::CWatchList_B::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
    __super::OnWindowPosChanging(lpwndpos);
    
    // Hide horizontal scrollbar:
    ShowScrollBar (SB_HORZ, FALSE);
    ModifyStyle (WS_HSCROLL, 0, SWP_DRAWFRAME);
}
//------------------------------------------------------------------------
void CbarWatch::CWatchList_B::OnLButtonDown(UINT nFlags, CPoint point) 
{
    __super::OnLButtonDown(nFlags, point);
    Invalidate ();
    UpdateWindow ();
}
//------------------------------------------------------------------------
int CbarWatch::CWatchList_B::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_LABELTIP);

    //Загрузка ImageList
    m_imageList.Create(IDB_DEBUG_VIEW,16, 0, RGB (255, 255, 255));
    SetImageList(&m_imageList, LVSIL_SMALL);        
    // если нужно, чтобы элементы списка были редактируемыми, вызываем следующую функцию
    SetReadOnly(FALSE);

    // добавляем колонки. третий параметр равен TRUE, для колонки с деревом
    InsertFColumn(0, "Status",  FALSE,  LVCFMT_LEFT, 10);
    InsertFColumn(1, "Name",    TRUE,   LVCFMT_LEFT, 100);
    InsertFColumn(2, "Value",   FALSE,  LVCFMT_LEFT, 200);                 
    InsertFColumn(3, "Type",	FALSE,  LVCFMT_LEFT, 100);                 
    InsertFColumn(4, "Context", FALSE,  LVCFMT_LEFT, 200);                 
    //SetFItemCheckboxImages(NULL, 0, 5, 6);
    SetFColumnImageOffset(1,false);
    return 0;
}
//------------------------------------------------------------------------
void CbarWatch::CWatchList_B::OnDestroy()
{
	Clear();
	__super::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////
// CbarWatch::CCallStack
///////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
//typedef CbarWatch::CCallStack CbarWatch_CCallStack;
//BEGIN_MESSAGE_MAP(CbarWatch_CCallStack, CListBox)
//BEGIN_MESSAGE_MAP(CbarWatch::CCallStack, CListBox)
//END_MESSAGE_MAP()

//------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// CbarWatch::CParseStack
///////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
typedef CbarWatch::CParsePanel CbarWatch_CParsePanel;
BEGIN_MESSAGE_MAP(CbarWatch_CParsePanel, CStatic)
	//BEGIN_MESSAGE_MAP(CbarWatch::CParseStack, CListBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

//------------------------------------------------------------------------
int CbarWatch::CParsePanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy; rectDummy.SetRectEmpty ();
	
	m_wndParseStack.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL, rectDummy, this, -1)	;
	m_wndTextBehind.Create(WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | ES_RIGHT | WS_VSCROLL,rectDummy,this,-1);
	m_wndTextAhead.Create(WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_VSCROLL, rectDummy,this,-1);
	
	m_wndTextBehind.ModifyStyleEx(0,WS_EX_STATICEDGE);
	m_wndTextAhead.ModifyStyleEx(0,WS_EX_STATICEDGE);

	m_wndTextBehind.SetFont(&GetDefaultGuiFont());
	m_wndTextBehind.SetWindowText("Text behind");
	
	m_wndTextAhead.SetFont(&GetDefaultGuiFont());
	m_wndTextAhead.SetWindowText("Text ahead");
	return 0;
}
//-------------------------------------------------------------------------
void CbarWatch::CParsePanel::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);

	CRect rc; GetClientRect(rc);

	size_t nH = GetDefaultGuiFont().CalcTextRect("A").bottom*2+2;

	m_wndTextBehind.SetWindowPos(NULL,rc.left, rc.top,rc.Width()/2-1,rc.top+nH,SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	m_wndTextAhead.SetWindowPos(NULL,rc.Width()/2+1, rc.top,rc.Width(),rc.top+nH,SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	
	m_wndParseStack.SetWindowPos(NULL, rc.left, rc.top+nH+1, rc.Width(), rc.Height()-(rc.top+nH+1), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
}


//////////////////////////////////////////////////////////////////////////
//--- CbarWatch
//////////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4355)
CbarWatch::CbarWatch():m_wndCallStack(*this),m_wndParsePanel(*this)
{
}
//------------------------------------------------------------------------
CbarWatch::~CbarWatch()
{
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CbarWatch, Cbar_B)
    ON_WM_CREATE()
    ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_WATCH, cmdOnViewWatch)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCH, cmdupOnViewWatch)
END_MESSAGE_MAP()
//------------------------------------------------------------------------
int CbarWatch::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    CRect rectDummy;
    rectDummy.SetRectEmpty ();

    // Create tabs window:
    if (!m_wndTabs.Create (CBCGTabWnd::STYLE_FLAT_SHARED_HORZ_SCROLL, rectDummy, this, 1))
    {
        TRACE0("Failed to create output tab window\n");
        return -1;      // fail to create
    }

    // Create output panes:
    const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT;
	
	//ПРи изменении кол-ва объектов необзодимо не забыть изменить этот список
	CWatchList_B * pWndPanes[] = {&m_wndWatchAuto,&m_wndWatchLocal};//,&m_wndWatchesN[0],&m_wndWatchesN[1],&m_wndWatchesN[2],&m_wndWatchesN[3]};
	for (int i=0; i<arrlength(pWndPanes);i++)
			pWndPanes[i]->Create (dwStyle, rectDummy, &m_wndTabs, i+1);
	m_wndCallStack.Create(dwStyle, rectDummy, &m_wndTabs, arrlength(pWndPanes))	;
	m_wndParsePanel.Create(NULL,WS_CHILD | WS_VISIBLE,rectDummy,&m_wndTabs);
	
	for (int i=0; i<arrlength(pWndPanes);i++)
		m_wndTabs.AddTab (pWndPanes[i], pWndPanes[i]->GetTitle(),(UINT)-1);
	m_wndTabs.AddTab (&m_wndCallStack, m_wndCallStack.GetTitle(),(UINT)-1);
	//m_wndTabs.AddTab (&m_wndParseStack, m_wndParseStack.GetTitle(),(UINT)-1);
	m_wndTabs.AddTab (&m_wndParsePanel, m_wndParsePanel.GetTitle(),(UINT)-1);
	
    SetFlatTabs(FLAT_TABS);
    return 0;
}
//------------------------------------------------------------------------
void CbarWatch::OnSize(UINT nType, int cx, int cy) 
{
    __super::OnSize(nType, cx, cy);

    CRect rc; GetClientRect(rc);

    m_wndTabs.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height (), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	for (int i=0;i<m_wndTabs.GetTabsNum();i++)	
		m_wndTabs.GetTabWnd(i)->SendMessage(CM_REALIGN,0,0);
}
//-------------------------------------------------------------------------
void CbarWatch::cmdOnViewWatch()
{
	ToggleVisible();
}
//-------------------------------------------------------------------------
void CbarWatch::cmdupOnViewWatch(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(IsVisible());
}



