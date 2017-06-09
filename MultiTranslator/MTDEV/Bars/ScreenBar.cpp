/********************************************************************
	created:	2002/05/10
	created:	10:5:2002   23:38
	filename: 	$(Projects)\sources\mtdev\bars\screenbar.cpp
	file path:	$(Projects)\sources\mtdev\bars

	file base:	screenbar
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реализация окна вывода(). Окно вывода 
				размещается в плавающем окне (conBarScreen)
*********************************************************************/

#include "stdafx.h" 
#include "ScreenBar.h" 

//Создание переходника для объекта
static i_objects::object_thunk<CbarScreen> barScreen(__MSG_PREFIX__);

BEGIN_MESSAGE_MAP(CbarScreen, Cbar_B)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_SCREEN, cmdOnViewScreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN, cmdupOnUpdateViewScreen)
	ON_COMMAND(ID_INDEPENDENT_BAR_SCREEN, cmdOnIndependentBar)
	ON_UPDATE_COMMAND_UI(ID_INDEPENDENT_BAR_SCREEN, cmdupOnIndependentBar)
END_MESSAGE_MAP()

//------------------------------------------------------------------------
#pragma warning(disable: 4355) //warning C4355: 'this' : used in base member initializer list
CbarScreen::CbarScreen():m_wndOutputScreen(this)
{
}
#pragma warning(default: 4355) 
//------------------------------------------------------------------------
CbarScreen::~CbarScreen()
{
}
//------------------------------------------------------------------------
void CbarScreen::OnCreateAppMainWnd() {
	i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);
	if ((AfxGetApp()->GetProfileInt(szProfileOutputScreen,szParamIsDelegated,false)))
		DoDelegate();
}
//------------------------------------------------------------------------
void CbarScreen::OnCloseAppMainWnd(){
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(szProfileOutputScreen,szParamIsDelegated,IsDelegated());
}
//------------------------------------------------------------------------
bool CbarScreen::IsVisibleDefault() const
{
	return false;
	//return (!ApplicationData::APP_AUTOMATED && !ApplicationData::APP_EMBEDDED);
}
//------------------------------------------------------------------------
void CbarScreen::InitDock()
{
	EnableDocking(CBRS_ALIGN_ANY);	

	ASSERT(GetParent()->IsKindOf( RUNTIME_CLASS( CFrameWnd) ));
	CFrameWnd* pFrame = (CFrameWnd*)(GetParent());
	pFrame ->FloatControlBar (this, CPoint (300,200));
}
//------------------------------------------------------------------------
void CbarScreen::DoDelegate(){
	IBarLink * pBar = i_objects::GetInterface<IBarLink>(IBarLink::idBarOutput);
	CBCGTabWnd * pTabWnd = (pBar) ? pBar->GetTabWnd() : NULL;
	Delegate(&m_wndOutputScreen,pTabWnd);
}
//------------------------------------------------------------------------
void CbarScreen::CreateWnd(CFrameWnd* pParentWnd)
{
	__super::CreateWnd(pParentWnd);
	m_wndOutputScreen.Create  (	WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL|ES_MULTILINE | ES_READONLY, 
								CRect(0,0,100,100), this, 2);
}
//------------------------------------------------------------------------
void CbarScreen::OnSize(UINT nType, int cx, int cy)
{
	Cbar_B::OnSize(nType, cx, cy);
	if (m_wndOutputScreen.m_hWnd)
		m_wndOutputScreen.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER | SWP_NOREPOSITION );
}
//------------------------------------------------------------------------
void CbarScreen::cmdOnViewScreen()
{
	ToggleVisible(); 
}
//------------------------------------------------------------------------
void CbarScreen::cmdupOnUpdateViewScreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (IsVisible());
}
//------------------------------------------------------------------------
void CbarScreen::cmdOnIndependentBar(){
	if (IsDelegated())
		UnDelegate();
	else 
		DoDelegate();
}
//------------------------------------------------------------------------
void CbarScreen::cmdupOnIndependentBar(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(!IsDelegated());
}

/////////////////////////////////////////////////////////////////////////////
// CbarScreen::COutputScreen
/////////////////////////////////////////////////////////////////////////////
CbarScreen::COutputScreen::COutputScreen(CBCGSizingControlBar * pBar)
{
	m_pParentBar		= pBar;
	m_bIsAutoClose		= false;	
	m_bIsAutoClear		= true;
	m_bIsVisible		= true;
	m_bIsOEMConvert		= true;
	m_bIsAltConversion	= false;
	
	SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	SetBkColor(GetSysColor(COLOR_WINDOW));
}
//------------------------------------------------------------------------
CbarScreen::COutputScreen::~COutputScreen()
{
	SaveSettings();
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::StartSession(bool bClear/*=true*/)
{
	if ((bClear) && (m_bIsAutoClear))
		Clear();
	//Убеждаемся, что наш Bar отображается на экране
	//Не вызывать напрямую, только через SendMessage, 
	//так как эта процедура может вызваться из другого
	//потока, а нам нужно вернуться в свой родной
	SendMessage(CM_ACTIVATE,0,0);
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::StopSession(bool bClear/*=false*/)
{
	if (bClear)
		Clear();
	SendMessage(CM_ACTIVATE,0,0);
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::Print(cstr szText)
{
	string_smart s; 
	
	if (m_bIsOEMConvert){
		s.reserve(strlen(szText));
		OemToChar(szText,s.buffer());
		szText=s.c_str();
	}

	CEdit::SetSel(-2, -1);
	CEdit::ReplaceSel(szText);
}
//-------------------------------------------------------------------------
string_smart CbarScreen::COutputScreen::GetText()
{
	CString S;
	GetWindowText(S);
	return (string_smart(S));
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::Clear()
{
	//Вариант с SetSel и Clear не работает, видимо, из-за того, что Edit - ReadOnly
	LOG_PROTECT_CODE(SetWindowText(""));
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::SaveSettings()
{
    CWinApp* pApp = AfxGetApp();
    pApp->WriteProfileInt(szProfileOutputScreen,szParamVisible,    (UINT)this->m_bIsVisible);
    pApp->WriteProfileInt(szProfileOutputScreen,szParamOEM,        (UINT)this->m_bIsOEMConvert);
    pApp->WriteProfileInt(szProfileOutputScreen,szParamAltConversion,(UINT)this->m_bIsAltConversion);
    pApp->WriteProfileInt(szProfileOutputScreen,szParamAutoClose,  (UINT)this->m_bIsAutoClose);
    pApp->WriteProfileInt(szProfileOutputScreen,szParamAutoClear,  (UINT)this->m_bIsAutoClear);
    pApp->WriteProfileInt(szProfileOutputScreen,szParamTextColor,  (UINT)this->m_clrText);
    pApp->WriteProfileInt(szProfileOutputScreen,szParamBackColor,  (UINT)this->m_clrBkgnd);
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::LoadSettings()
{
    CWinApp* pApp = AfxGetApp();
    m_bIsVisible            =   pApp->GetProfileInt(szProfileOutputScreen,szParamVisible,true)!=0;
    m_bIsAutoClose          =   pApp->GetProfileInt(szProfileOutputScreen,szParamAutoClose,false)!=0;
    m_bIsAutoClear          =   pApp->GetProfileInt(szProfileOutputScreen,szParamAutoClear,true)!=0;
    m_bIsOEMConvert         =   pApp->GetProfileInt(szProfileOutputScreen,szParamOEM,false)!=0;
    m_bIsAltConversion      =   pApp->GetProfileInt(szProfileOutputScreen,szParamAltConversion,true)!=0;
    SetTextColor((COLORREF)pApp->GetProfileInt(szProfileOutputScreen,szParamTextColor,GetSysColor(COLOR_WINDOWTEXT)));
    SetBkColor((COLORREF)pApp->GetProfileInt(szProfileOutputScreen,szParamBackColor,GetSysColor(COLOR_WINDOW)));
}
//------------------------------------------------------------------------
typedef CbarScreen::COutputScreen CbarOutputScreen;
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CbarOutputScreen, CEdit)
//BEGIN_MESSAGE_MAP(CbarScreen::COutputScreen, CEdit)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()	
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_MESSAGE_VOID(CM_ACTIVATE,msgOnActivate)

	ON_COMMAND(ID_OUTPUT_OPTIONS_ALTCONVERSION, cmdOnAltConversion)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_OPTIONS_ALTCONVERSION, cmdupOnAltConversion)
	ON_COMMAND(ID_OUTPUT_OPTIONS_AUTOCLEAR, cmdOnAutoClear)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_OPTIONS_AUTOCLEAR, cmdupOnAutoClear)
	ON_COMMAND(ID_OUTPUT_OPTIONS_AUTOCLOSE, cmdOnAutoClose)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_OPTIONS_AUTOCLOSE, cmdupOnAutoClose)
	ON_COMMAND(ID_OUTPUT_OPTIONS_BACKCOLOR, cmdOnBackColor)
	ON_COMMAND(ID_OUTPUT_OPTIONS_CLEAR, cmdOnClear)
	ON_COMMAND(ID_OUTPUT_OPTIONS_OEM, cmdOnOEM)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_OPTIONS_OEM, cmdupOnOEM)
	ON_COMMAND(ID_OUTPUT_OPTIONS_SIZETOCONTENT, cmdOnSizeToContent)
	ON_COMMAND(ID_OUTPUT_OPTIONS_TEXTCOLOR, cmdOnTextColor)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_OPTIONS_SIZETOCONTENT, cmdupOnSizeToContent)
	ON_COMMAND(ID_EDIT_COPY, cmdOnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, cmdupOnEditCopy)
	ON_COMMAND(ID_FILE_SAVE, cmdOnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, cmdupOnFileSave)
	ON_COMMAND(ID_EDIT_SELECT_ALL, cmdOnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, cmdupOnEditSelectAll)
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::msgOnActivate()
{
	((Cbar_B*)m_pParentBar)->Show();
	//for (CWnd * pWnd = GetParent(); (pWnd) && (pWnd!=AfxGetMainWnd()); pWnd=pWnd->GetParent())
	//	pWnd->SendMessage(CM_SHOW,0,0);
}
//------------------------------------------------------------------------
int CbarScreen::COutputScreen::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_Font.CreateStockObject (DEFAULT_GUI_FONT);
	
	SetFont(&m_Font);
	return 0;
}
//------------------------------------------------------------------------
HBRUSH CbarScreen::COutputScreen::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetTextColor( m_clrText );
	pDC->SetBkColor	 ( m_clrBkgnd);
	return m_brBkgnd;
}
//------------------------------------------------------------------------
BOOL CbarScreen::COutputScreen::OnEraseBkgnd(CDC* pDC)
{
	// Set brush to desired background color.
    CBrush backBrush(m_clrBkgnd);
    // Save old brush.
    CBrush* pOldBrush = pDC->SelectObject(&backBrush);
    CRect rect1; GetClientRect(&rect1);
    CRect rect2; pDC->GetClipBox(&rect2);
    CRect rect3; rect3.IntersectRect(rect1,rect2);
    pDC->PatBlt(rect3.left, rect3.top, rect3.Width(), rect3.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);
    return TRUE;
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	SetFocus();
	GetWorkspace()->ShowPopupMenu (IDR_POPUP_OUTPUT_SCREEN, point,AfxGetMainWnd());
}
//------------------------------------------------------------------------
void CbarScreen::COutputScreen::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
	if (m_bIsAutoClose) 
		for (CWnd * pWnd = GetParent(); (pWnd) && (pWnd!=AfxGetMainWnd()); pWnd=pWnd->GetParent())
			pWnd->SendMessage(CM_HIDE,0,0);
}
//-------------------------------------------------------------------------
//----CbarScreen::COutputScreen: ACTION EVENTS-----------------------------
//-------------------------------------------------------------------------
void CbarScreen::COutputScreen::cmdOnAltConversion(){
	m_bIsAltConversion=!m_bIsAltConversion;
}
void CbarScreen::COutputScreen::cmdOnAutoClear(){
	m_bIsAutoClear=!m_bIsAutoClear;
}
void CbarScreen::COutputScreen::cmdOnAutoClose(){
	m_bIsAutoClose=!m_bIsAutoClose;
}
void CbarScreen::COutputScreen::cmdOnClear(){
	SetWindowText(NULL);
}
void CbarScreen::COutputScreen::cmdOnOEM(){
	m_bIsOEMConvert=!m_bIsOEMConvert;

	CString Text; GetWindowText(Text);
	if (m_bIsOEMConvert)	Text.OemToAnsi();
	else					Text.AnsiToOem();
	SetWindowText(Text);	
}
void CbarScreen::COutputScreen::cmdOnSizeToContent(){
	ASSERT_RETURN_VOID((m_pParentBar)&& (m_pParentBar->IsFloating()));

	CClientDC dc (this);
	CFont* pOldFont = dc.SelectObject (&m_Font);
	
	CString strText; GetWindowText(strText);
	strText.TrimRight(); 
	if (strText.GetLength()!=GetWindowTextLength())
		SetWindowText(strText);
		
	string_list strAllStrings(strText);
	
	int nAutoWidth=0;
	int nAutoHeight=0;
	
	for  (size_t i = 0 ; i<strAllStrings.items().count();i++){
		const string_smart & s= strAllStrings.items().get_at(i);
		CSize aSize = (s.isempty())	
							? dc.GetOutputTextExtent(" ",1)
							: dc.GetOutputTextExtent(s.c_str(),s.length());
		nAutoWidth=max(nAutoWidth,aSize.cx);
		nAutoHeight+=aSize.cy;
	}
	dc.SelectObject (pOldFont);
	if (GetStyle() & WS_VSCROLL) nAutoWidth+=GetSystemMetrics(SM_CXVSCROLL);
	if (GetStyle() & WS_HSCROLL) nAutoHeight+=GetSystemMetrics(SM_CYHSCROLL);
	nAutoWidth+=GetSystemMetrics(SM_CYFRAME);		
	nAutoHeight+=GetSystemMetrics(SM_CXFRAME);

	CRect Screen; ::SystemParametersInfo(SPI_GETWORKAREA,0,&Screen,0);
	CRect rect;		m_pParentBar->GetWindowRect(rect);
	
	if (rect.left+nAutoWidth>Screen.right)
		nAutoWidth=max(rect.right,Screen.right)-rect.left;
	
	if (rect.top+nAutoHeight>Screen.bottom)
		nAutoHeight=max(rect.bottom,Screen.bottom)-rect.top;
	
	m_pParentBar->m_szFloat.cx=max(nAutoWidth+m_pParentBar->GetBorderSize()*2,150);
	m_pParentBar->m_szFloat.cy=max(nAutoHeight+m_pParentBar->GetBorderSize()*2,80);

	//Корректируем левый верхний угол окна. Почему это надо делать, неизвестно
	rect.top-=GetSystemMetrics(SM_CYSMCAPTION)+GetSystemMetrics(SM_CXEDGE); 
	rect.left-=GetSystemMetrics(SM_CYEDGE); 
	
	m_pParentBar->GetParentFrame()->FloatControlBar(m_pParentBar,rect.TopLeft(),
	CBRS_ALIGN_TOP | (m_pParentBar->m_dwDockStyle & CBRS_FLOAT_MULTI) | CBRS_SIZE_DYNAMIC);
}
void CbarScreen::COutputScreen::cmdOnBackColor(){
    CBCGColorDialog dlg (m_clrBkgnd, 0, this);
    if (dlg.DoModal() == IDOK)
    {
		m_clrBkgnd=dlg.GetColor ();
        Invalidate(true);
    }
}
void CbarScreen::COutputScreen::cmdOnTextColor(){
    CBCGColorDialog dlg (m_clrText, 0, this);
    if (dlg.DoModal() == IDOK)
    {
		m_clrText=dlg.GetColor ();
        Invalidate(true);
    }
}
void CbarScreen::COutputScreen::cmdupOnAltConversion(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(m_bIsAltConversion);
}
void CbarScreen::COutputScreen::cmdupOnAutoClear(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(m_bIsAutoClear);
}
void CbarScreen::COutputScreen::cmdupOnAutoClose(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(m_bIsAutoClose);
}
void CbarScreen::COutputScreen::cmdupOnOEM(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(m_bIsOEMConvert);
}
void CbarScreen::COutputScreen::cmdupOnSizeToContent(CCmdUI *pCmdUI){
	pCmdUI->Enable(m_pParentBar->IsFloating());
}
void CbarScreen::COutputScreen::cmdupOnEditCopy(CCmdUI *pCmdUI){
	int nStart,nEnd; GetSel(nStart,nEnd);
	pCmdUI->Enable((::GetFocus()==m_hWnd) && (nEnd>nStart));
}
void CbarScreen::COutputScreen::cmdOnEditCopy(){
	Copy();
}
void CbarScreen::COutputScreen::cmdOnFileSave(){
	IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngrLink);

	string_list	lstFiles;
	if (!pDocMngrLink->SelectFile(lstFiles,NULL,
		string_smart(_T("Screen Output: "),string_res(AFX_IDS_SAVEFILE,HFIND_RESOURCES(AFX_IDS_SAVEFILE)).c_str()).c_str(),
		NULL,false,false,false,false,false))
		return;

	save_file(lstFiles.c_str(),GetText().c_str());
}
void CbarScreen::COutputScreen::cmdupOnFileSave(CCmdUI *pCmdUI)	{
	int n = SendMessage(WM_GETTEXTLENGTH,0,0);
	pCmdUI->Enable(n>0);
}
void CbarScreen::COutputScreen::cmdOnEditSelectAll(){
	SetSel(0,-1);
}
void CbarScreen::COutputScreen::cmdupOnEditSelectAll(CCmdUI *pCmdUI){
}
