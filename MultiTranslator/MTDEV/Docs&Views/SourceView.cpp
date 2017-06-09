/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   12:57
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\SourceView.cpp
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	SourceView
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реализация отображения документа исходного файла (SourceFile)
*********************************************************************/


#include "stdafx.h"

namespace lexer
{
	#include <Platform.h>	//Это должно быть раньше Scintilla.h
};
using namespace lexer;

#include <Scintilla.h> 

#include <SciLexer.h>
#include <GUIService.h>

#include <BaseUtils.h>
#include <MiscUtils.h>

#include "Resource.h"
#include "SourceDoc.h"

#include "Names.h"
#include "SourceView.h"

using namespace base_utils;
using namespace misc_utils;

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_POS,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_OVR,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

//Для отслеживания текущего указателя команды
CSourceView * CSourceView::pCurComandPointerView=NULL;
DWORD CSourceView::nCommandPointerHandle=0;

/////////////////////////////////////////////////////////////////////////////
// CSourceView
/////////////////////////////////////////////////////////////////////////////
 
IMPLEMENT_DYNCREATE(CSourceView, CView_B)

BEGIN_MESSAGE_MAP(CSourceView, CView_B)
//{{AFX_MSG_MAP(CSourceView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_REPLACE, cmdOnEditReplace)
	ON_COMMAND(ID_EDIT_FIND, cmdOnEditFind)
	ON_COMMAND(ID_EDIT_FIND_COMBO, cmdOnEditFindCombo)
	ON_COMMAND(ID_EDIT_GOTOLINE, cmdOnGotoline) 
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_POS, cmdupOnIndicatorPOS)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, cmdupOnIndicatorOVR)
	ON_COMMAND(ID_EDIT_FIND_NEXT, cmdOnEditFindNext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_NEXT, cmdupOnEditFindNext)
	ON_COMMAND(ID_EDIT_COPY, cmdOnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, cmdOnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, cmdOnEditPaste)
	ON_COMMAND(ID_EDIT_REDO, cmdOnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, cmdOnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, cmdupOnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, cmdupOnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, cmdupOnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, cmdupOnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, cmdupOnEditUndo)
	ON_COMMAND(ID_EDIT_NEXT_BOOKMARK, cmdOnEditNextBookmark)
	ON_COMMAND(ID_EDIT_PREV_BOOKMARK, cmdOnEditPrevBookmark)
	ON_COMMAND(ID_EDIT_TOGGLE_BOOKMARK, cmdOnEditToggleBookmark)
	ON_COMMAND(ID_EDIT_CLEAR_BOOKMARKS, cmdOnEditClearBookmarks)
	ON_COMMAND(ID_EDIT_SELECT_ALL, cmdOnEditSelectAll)
	ON_COMMAND(ID_VIEW_WHITESPACE, cmdOnViewWhitespace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WHITESPACE, cmdupOnViewWhitespace)
	ON_COMMAND(ID_VIEW_INDENTGUIDES, cmdOnViewIndentGuides)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INDENTGUIDES, cmdupOnViewIndentGuides)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_FOLD_CUR, cmdOnEditFoldOne)
	ON_COMMAND(ID_EDIT_FOLD_ALL, cmdOnEditFoldAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FOLD_ALL, cmdupOnEditFoldAll)
	ON_COMMAND(ID_VIEW_FOLD_MARGIN, cmdOnViewFoldMargin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FOLD_MARGIN, cmdupOnViewFoldMargin)
	ON_COMMAND(ID_EDIT_ZOOM_IN, cmdOnEditZoomIn)
	ON_COMMAND(ID_EDIT_ZOOM_OUT, cmdOnEditZoomOut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FOLD_CUR, cmdupOnEditFoldCur)
	ON_COMMAND(ID_EDIT_ZOOM_RESET, cmdOnEditZoomReset)
	ON_COMMAND(ID_EDIT_FIND_IN_FILES, cmdOnEditFindInFiles)
	ON_COMMAND(ID_EDIT_AUTOFORMAT, cmdOnEditAutoFormat)
	ON_UPDATE_COMMAND_UI(ID_EDIT_AUTOFORMAT, cmdupOnEditAutoFormat)
	ON_COMMAND(ID_EDIT_TABIFYSELECTION, cmdOnEditTabifySelection)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TABIFYSELECTION, cmdupOnEditTabifySelection)
	ON_COMMAND(ID_EDIT_UNTABIFYSELECTION, cmdOnEditUntabifySelection)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNTABIFYSELECTION, cmdupOnEditUntabifySelection)
	ON_COMMAND(ID_EDIT_UPPERCASE, cmdOnEditUppercase)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UPPERCASE, cmdupOnEditUppercase)
	ON_COMMAND(ID_EDIT_LOWERCASE, cmdOnEditLowecase)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LOWERCASE, cmdupOnEditLowecase)
	ON_COMMAND(ID_EDIT_INCREASE_LINE_INDENT, cmdOnIncreaseLineIndent)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INCREASE_LINE_INDENT, cmdupOnIncreaseLineIndent)
	ON_COMMAND(ID_EDIT_DECREASE_LINE_INDENT, cmdOnDecreaseLineIndent)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DECREASE_LINE_INDENT, cmdupOnDecreaseLineIndent)
	ON_COMMAND(ID_DEBUG_TOGGLE_BREAKPOINT, cmdOnToggleBreakpoint)
	ON_COMMAND(ID_BREAKPOINTS_CLEAR_ALL, cmdOnClearAllBreakpoints)
	ON_COMMAND(ID_DEBUG_DISABLE_BREAKPOINT, cmdOnDisableBreakpoint)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_DISABLE_BREAKPOINT, cmdupOnDisableBreakpoint)
	ON_COMMAND(ID_EDIT_FIND_DECLARATION, cmdOnFindDeclaration)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_DECLARATION, cmdupOnFindDeclaration)
	ON_COMMAND(ID_EDIT_COMPLETE_WORD, cmdOnEditCompleteWord)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COMPLETE_WORD, cmdupOnEditCompleteWord)
	ON_COMMAND(ID_FILE_PRINT, CView_B::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView_B::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView_B::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_SYNC_EXPLORER, cmdOnEditSyncExplorer)
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma warning(disable: 4355)
//------------------------------------------------------------------------
CSourceView::CSourceView():	m_Messenger(),
							m_Image(*this),
							m_Editor(*this),
							m_Navigation(*this),
							m_nState(0),
							m_nTimerID(0),
							pHeaderFont(0)
{

}
//------------------------------------------------------------------------
CSourceView::~CSourceView()
{
}
//------------------------------------------------------------------------
BOOL CSourceView::PreCreateWindow(CREATESTRUCT& cs)
{
	cstr szEditorClass = m_Editor.GetEditorClassName();
	if (!szEditorClass){
		AfxMessageBox(IDS_CANT_LOAD_EDITOR);
		return false;
	}
	else
		cs.lpszClass=szEditorClass;

    return __super::PreCreateWindow(cs);
}
//------------------------------------------------------------------------
void CSourceView::OnPaint() 
{
	//Обработчик перекрыт для того, чтобы избежать обработки CView::OnPaint
	Default();
}
//------------------------------------------------------------------------
void CSourceView::OnDraw(CDC* pDC)
{
}
//------------------------------------------------------------------------
bool CSourceView::IsActiveWindow()
{
	CWnd * pWnd = GetFocus();
	return pWnd==this;
}
//------------------------------------------------------------------------
void CSourceView::RaiseChangeEvent(int EventType)
{
	string_cstr strDocPath(GetDocument()->GetPath());

	evdChangeDocumentInfo Info(extract_filename(strDocPath).c_str(),strDocPath,*GetDocument());
	Info.in_nChangeType=EventType;
	
	i_objects::event<ARGS_1(evdChangeDocumentInfo &)>(IDocumentLink::IEventSink::evOnChangeDocument,Info);
}
//------------------------------------------------------------------------
#ifdef _DEBUG
void CSourceView::AssertValid() const{
    __super::AssertValid();
}
void CSourceView::Dump(CDumpContext& dc) const{
    __super::Dump(dc);
}
#endif //_DEBUG
//------------------------------------------------------------------------
CSourceDoc* CSourceView::GetDocument() 
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSourceDoc)));
    return (CSourceDoc*)m_pDocument;
}
//------------------------------------------------------------------------
int CSourceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CWaitCursor wait;

    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_Messenger.SetWnd(m_hWnd);
    return 0;
}
//------------------------------------------------------------------------
void CSourceView::OnInitialUpdate()
{
	CWaitCursor wait;

	__super::OnInitialUpdate();
    LOG_PROTECT_CODE(m_Editor.Initialize());	//В Create еще рано, а здесь - в самый раз
}
//------------------------------------------------------------------------
void CSourceView::Serialize(CArchive& ar) 
{
	CWaitCursor wait;

    if (ar.IsStoring())
    {  
		string_smart strText;
		m_Editor.BeforeSave();
		m_Messenger.GetText(strText);
		ar.Write(strText.c_str(),strText.length());
		m_Editor.AfterSave();
    }
    else
    {   // loading code
        const BlockSize = 1024*10; // 10 KB
        char szText[BlockSize]; 
        unsigned ReadedBlock;
	
		//Все точки удаляем. Хотя их и не должно быть, но при перезагрузке
		//документа может такая ситуация возникнуть
		//Если точки не удалить - будет плохо. Информация из Editor уже 
		//уйдет, и нас будут повисшие ссылки
		m_Breakpoints.clear();

		m_Editor.BeforeLoad();	//Подготавливаем редактор к загрузке
        while ((ReadedBlock=ar.Read(szText,BlockSize))>0)
            m_Messenger.AddText(szText,ReadedBlock);
		m_Editor.AfterLoad();	//Обязательно информируем об окончании загрузки!!!
	}
}
//------------------------------------------------------------------------
bool CSourceView::GetBreakPoint(long dwLine, BREAKPOINT & Breakpoint)
{
	bool bEnabled = m_Messenger.IsMarkered(dwLine,MARKER_BREAKPOINT);
	bool bDisabled= m_Messenger.IsMarkered(dwLine,MARKER_DISABLE_BREAKPOINT);
	ASSERT(!bEnabled || !bDisabled); //Одновременно не может быть 2 маркера
			    
	if (bEnabled || bDisabled)
		for (breakpoint_vector::iterator i=m_Breakpoints.begin();i!=m_Breakpoints.end();++i)
		{
			(*i).dwLine = m_Messenger.GetMarkerLine((*i).GetHandle()); //Подправим попутно остальные точки
			ASSERT((*i).GetLine()!=-1);
			if ((*i).GetLine()==dwLine){
				Breakpoint=(*i);
				return true;
			}
		}
	return false;
}
//------------------------------------------------------------------------
void CSourceView::SetBreakPoint(BREAKPOINT & Breakpoint)
{
	{	//Скобки не убирать!
		CAutoState State(*this, opChangingBreakpoints);
		ResetBreakPoint(Breakpoint.GetLine());
		
		Breakpoint.dwHandle = m_Messenger.SetMarker(Breakpoint.GetLine(),(Breakpoint.IsEnabled()) ? MARKER_BREAKPOINT : MARKER_DISABLE_BREAKPOINT);
		Breakpoint.m_pOwner = this;
		
		m_Breakpoints.push_back(Breakpoint);
	}
	
	//Событие изменения точек останова		
	RaiseChangeEvent(evdChangeSourceDocInfo::changeBreakpoints);
}
//------------------------------------------------------------------------
void CSourceView::ResetBreakPoint(long dwLine)
{
	if (GetBreakPoint(dwLine,BREAKPOINT_EX(this)))
	{
		for (breakpoint_vector::iterator i=m_Breakpoints.begin();i!=m_Breakpoints.end();++i)
			if (m_Messenger.GetMarkerLine((*i).GetHandle())==dwLine){
				m_Breakpoints.erase(i);
				break;
			}
		m_Messenger.ClearMarker(dwLine,MARKER_BREAKPOINT);
		m_Messenger.ClearMarker(dwLine,MARKER_DISABLE_BREAKPOINT);

		//Событие изменения точек останова	
		if ((m_nState & opChangingBreakpoints)==0)
			RaiseChangeEvent(evdChangeSourceDocInfo::changeBreakpoints);
	}
}
//------------------------------------------------------------------------
void CSourceView::GetBreakPoints(vector<BREAKPOINT> & Breakpoints)
{
	Breakpoints.clear();
	//Обновляем информацию о точках
	for (breakpoint_vector::iterator i=m_Breakpoints.begin();i!=m_Breakpoints.end();++i){
		long dwLine = m_Messenger.GetMarkerLine((*i).GetHandle());
		_ASSERTE(dwLine!=-1);
		if (dwLine!=-1)	{
			(*i).dwLine = dwLine; Breakpoints.push_back((*i));
		}
		else {
			m_Breakpoints.erase(i);i=m_Breakpoints.begin();
		}
	}
}
//------------------------------------------------------------------------
void CSourceView::EnableBreakpoint(long dwLine, bool bEnable)
{
	BREAKPOINT_EX bp(this); 
	if (GetBreakPoint(dwLine,bp)){
		{
			CAutoState State(*this, opChangingBreakpoints);
			ResetBreakPoint(dwLine);
		}
		bp.Enable(bEnable);
	}
}
//------------------------------------------------------------------------
void CSourceView::GetBookmarks(vector<long> & bookmarks)
{
	bookmarks.clear();
	for (long i=m_Messenger.MarkerNext(0,MARKER_BOOKMARK);i!=-1; i=m_Messenger.MarkerNext(i+1,MARKER_BOOKMARK))
		bookmarks.push_back(i);
}
//------------------------------------------------------------------------
void CSourceView::SetCommandMarker(long dwLine)
{
	m_Messenger.GotoLine(dwLine);
	
	//Уничтожаем старый маркер
	if (pCurComandPointerView==this)
		m_Messenger.ClearMarker(nCommandPointerHandle);
	
	//Устанавливаем новый
	nCommandPointerHandle=m_Messenger.SetMarker(dwLine,MARKER_COMMAND_POINTER);
	pCurComandPointerView=this;
	
	if (!m_nTimerID) 
		m_nTimerID=SetTimer((UINT)-1, 100, 0);
}
//------------------------------------------------------------------------
void CSourceView::OnDestroy() 
{
    __super::OnDestroy();
}
//------------------------------------------------------------------------
BOOL CSourceView::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	bool bPrevModified = m_Editor.m_bModified; //Запоминаем, был ли текст уже изменен к этому моменту
	m_Editor.m_bModified=false; 

    if (message==WM_NOTIFY)
        m_Editor.OnNotifyMsg(wParam,lParam);
    else if (message=WM_COMMAND)
        m_Editor.OnCmdMsg(wParam,lParam);
	
	bool bModified = m_Editor.m_bModified;
	m_Editor.m_bModified |= bPrevModified;	//Восстанавливаем как было

	//Не было ли изменений в тексте
	if (bModified)
		GetDocument()->SetModifiedFlag(m_Editor.m_bModified);

    return __super::OnChildNotify(message, wParam, lParam, pLResult);
}
//------------------------------------------------------------------------
LRESULT CSourceView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LOG_PROTECT_CODE_BEGIN	//Несмотря на то, что такая защита уже есть 
							//в Main, поставить ее еще и здесь будет неплохо

	if (message==WM_KEYDOWN){

		//Всплывающие подсказки
		CShortCut key(wParam,lParam);
		long nCurPos = m_Messenger.GetCurPos();
		if ((key.wKey==VK_TAB) && m_Messenger.IsTipVisible() && (nCurPos>0)){
	
			string_smart strTip=m_Messenger.GetCurTip();
			//Смотрим, сколько пользователь уже ввел символов
			for (long i=nCurPos-1; i>=0; i--){ 
				string_smart strTyped = m_Image.GetTextRange(i,nCurPos);
				//На всякий случай
				if (strTyped.length()>strTip.length())
					break;
				//Итак, мы нашли начало
				if (strncmp(strTyped.c_str(),strTip.c_str(),strTyped.length())==0){
					long n = nCurPos-i; //Сколько символов пропущено
					m_Messenger.InsertText(string_smart(strTip.c_str()+n).c_str(),nCurPos);
					nCurPos+=strTip.length()-n; //Сколько добавлено в текст
					m_Messenger.GotoPos(nCurPos);
					m_Messenger.HideTip();

					PeekMessage(&MSG(),m_hWnd,WM_CHAR,WM_CHAR,PM_REMOVE);
					return 0;
				}
			}
		}
	}

	return __super::WindowProc(message,wParam,lParam);
	LOG_PROTECT_CODE_END
	return 0;
}
//------------------------------------------------------------------------
void CSourceView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	//Установка нашго собственного StatusBar
	IMainFrameLink * pMainFrameLink = singleton_cache::GetMainFrameLink();
	if (pMainFrameLink)
	{
		pMainFrameLink->SetStatusBarIndicators(
			indicators, 
			bActivate ? sizeof(indicators)/sizeof(UINT) : 1, 
			true);
	}
	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
//------------------------------------------------------------------------
void CSourceView::OnRButtonUp(UINT nFlags, CPoint point)
{
	__super::OnRButtonUp(nFlags, point);
}
//------------------------------------------------------------------------
void CSourceView::OnRButtonDown(UINT nFlags, CPoint point)
{
	//Если не щелкаем над выделенным текстом, переводим курсор в новую позицию
	if ((!inrange(	m_Messenger.Perform(SCI_POSITIONFROMPOINT,point.x,point.y),
					m_Messenger.GetSelStart(),
					m_Messenger.GetSelEnd()))
		 &&	(point.x>m_Messenger.GetMarginsWidth()))
	{
		SendMessage(WM_LBUTTONDOWN,MK_LBUTTON,pttodw(point));
		SendMessage(WM_LBUTTONUP,MK_LBUTTON,pttodw(point));
	}
		
	__super::OnRButtonDown(nFlags, point);
}
//------------------------------------------------------------------------
void CSourceView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	long nMenuID= IDR_POPUP_DOC_SOURCE;
	//Detect mouse position
	if (point != CPoint (-1, -1)){
		CPoint ptView = point; ScreenToClient (&ptView);
		if (ptView.x<m_Messenger.GetMarginsWidth())
			nMenuID=IDR_POPUP_DOC_SOURCE_MARGIN;
	}

	GetWorkspace()->ShowPopupMenu (nMenuID, point,this);
}
//------------------------------------------------------------------------
void CSourceView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==m_nTimerID){
		//Уже не мы активны
		if (pCurComandPointerView!=this){
			m_Messenger.ClearMarker(nCommandPointerHandle);
			KillTimer(m_nTimerID);
			m_nTimerID=0;
		}
		//Уже процесс отладки либо кончился, либо ушел в выполнение
		else{
			IRunnerLink	* pRunLink = i_objects::GetInterface<IRunnerLink>();
			_ASSERTE(pRunLink);
			if ((pRunLink) && (!pRunLink->IsPaused())){
				m_Messenger.ClearMarker(nCommandPointerHandle);
				KillTimer(m_nTimerID);
				m_nTimerID=0;
			}
		}
	}
	__super::OnTimer(nIDEvent);
}
//------------------------------------------------------------------------
///////////////////////// Printing ///////////////////////////////////////
void CSourceView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(pDC);

	__super::OnPrepareDC(pDC, pInfo);

	//Отслеживаем, если мы все уже распечатали, выставляем флаг окончания печати
	//Делать это нужно после __super::OnPrepareDC
	if(pDC->IsPrinting()) 
	{
/*	else if (pInfo->m_bPreview){
		pDC->SetMapMode(MM_TWIPS);
	}
*/
		long nTextLen = m_Messenger.GetTextLen();
		long nPrinted = (m_lstPrintPages.size()>0) ? m_lstPrintPages[m_lstPrintPages.size()-1] : 0; 
		
		if (nPrinted>=nTextLen)
			pInfo->m_bContinuePrinting=false;
	}
}
BOOL CSourceView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if (pInfo->m_bPreview)
		m_Messenger.Perform(SCI_SETPRINTMAGNIFICATION,20);
	else
		m_Messenger.Perform(SCI_SETPRINTMAGNIFICATION,-2);

	return DoPreparePrinting(pInfo); //Внимание! Не OnPreparePrinting а  DoPreparePrinting!
}
void CSourceView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	pHeaderFont = new CAutoFont(_T("Arial"));
		
	pHeaderFont->SetHeight(MulDiv(9, pDC->GetDeviceCaps(LOGPIXELSY), 72));
	pHeaderFont->SetBold(true);
	pHeaderFont->SetItalic(true);

	__super::OnBeginPrinting(pDC, pInfo);
}

void CSourceView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	delete pHeaderFont;
	pHeaderFont = NULL;

	__super::OnEndPrinting(pDC, pInfo);
}
void CSourceView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// We must substract the physical margins from the printable area
	RangeToFormat frPrint;
	
	// Get printer resolution
	long DPIx =	pDC->GetDeviceCaps(LOGPIXELSX);    // dpi in X direction
	long DPIy = pDC->GetDeviceCaps(LOGPIXELSY);    // dpi in Y direction


	CRect Margins = GetDocument()->GetPageMargins();
	CRect ScaledMargins=Margins;
	
	// Convert the hundredths of millimeters (HiMetric) or
	// thousandths of inches (HiEnglish) margin values
	// from the Page Setup dialog to device units.
	// (There are 2540 hundredths of a mm in an inch.)
	char localeInfo[3];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, localeInfo, 3);

	long  Divider = (localeInfo[0] == '0') ? 2540 : 1000; // '0' - Metric system. '1' is US System

	//convert 'uni' points to device coordinates 
	ScaledMargins.left	= ::MulDiv(Margins.left,  DPIx, Divider);
	ScaledMargins.top	= ::MulDiv(Margins.top,   DPIy, Divider);
	ScaledMargins.right	= ::MulDiv(Margins.right, DPIx, Divider);
	ScaledMargins.bottom= ::MulDiv(Margins.bottom,DPIy, Divider);

	// rectMargins now contains the values used to shrink the printable
	// area of the page.
	
	// Convert device coordinates into logical coordinates
	pDC->DPtoLP(&ScaledMargins);

	frPrint.hdc			= pDC->m_hDC;
	frPrint.hdcTarget	= pDC->m_hDC;
	frPrint.rc.left		= pInfo->m_rectDraw.left+ScaledMargins.left;
	frPrint.rc.top		= pInfo->m_rectDraw.top+ScaledMargins.top;
	frPrint.rc.right	= pInfo->m_rectDraw.right-ScaledMargins.right;
	frPrint.rc.bottom	= pInfo->m_rectDraw.bottom-ScaledMargins.bottom;
	frPrint.rcPage.left   = 0;
	frPrint.rcPage.top	  = 0;
	frPrint.rcPage.right  = pInfo->m_rectDraw.right-1;
	frPrint.rcPage.bottom = pInfo->m_rectDraw.bottom-1;

	//Print Header

	::SelectObject(pDC->m_hDC,*pHeaderFont);
	TEXTMETRIC tm;  pDC->GetTextMetrics(&tm);
	long HeaderHeight = pHeaderFont->GetHeight()+tm.tmExternalLeading;
	 
	UINT ta = pDC->SetTextAlign(TA_BOTTOM);
	pDC->SetTextColor(clBlack);
	pDC->SetBkColor(clWhite);

	RECT rcw = {frPrint.rc.left, frPrint.rc.top - HeaderHeight - HeaderHeight / 2,
				frPrint.rc.right, frPrint.rc.top - HeaderHeight / 2};
	rcw.bottom = rcw.top + HeaderHeight;

	//Draw FileName
	string_smart strText = GetDocument()->GetPath().c_str();
	pDC->ExtTextOut(frPrint.rc.left + 5, frPrint.rc.top - HeaderHeight / 2,
					ETO_OPAQUE, &rcw, strText.c_str(), strText.length(), NULL);
	
	//Draw PageNumber
	strText = itos(pInfo->m_nCurPage);
	rcw.left=rcw.right-pDC->GetTextExtent("99999",5).cx;
	pDC->ExtTextOut(frPrint.rc.right-10, frPrint.rc.top - HeaderHeight / 2, 
					ETO_OPAQUE, &rcw, strText.c_str(), strText.length(), NULL);
	
	pDC->SetTextAlign(ta);
	
	//Draw header line
	HPEN pen = ::CreatePen(0, 1, 2);
	HPEN penOld = static_cast<HPEN>(::SelectObject(pDC->m_hDC,pen));
	pDC->MoveTo(frPrint.rc.left, frPrint.rc.top - HeaderHeight / 3);
	pDC->LineTo(frPrint.rc.right, frPrint.rc.top - HeaderHeight / 3);
	::SelectObject(pDC->m_hDC,penOld);
	::DeleteObject(pen);

	//Draw Text
	if (pInfo->m_nCurPage-1>=m_lstPrintPages.size())
		m_lstPrintPages.push_back(0);

	frPrint.chrg.cpMin = m_lstPrintPages[pInfo->m_nCurPage-1];
	frPrint.chrg.cpMax = m_Messenger.GetTextLen();
	frPrint.chrg.cpMin = m_Messenger.Perform(SCI_FORMATRANGE,true,reinterpret_cast<LPARAM>(&frPrint));

	if (pInfo->m_nCurPage>=m_lstPrintPages.size())
		m_lstPrintPages.push_back(frPrint.chrg.cpMin);

	if (pInfo->GetMaxPage()==0xFFFF)
		pInfo->SetMaxPage(pInfo->m_nCurPage);

	//Все время регулируем последнюю страницу, чтобы он не была меньше текущей
	if ((frPrint.chrg.cpMin<frPrint.chrg.cpMax) && (pInfo->GetMaxPage()<=pInfo->m_nCurPage))
		pInfo->SetMaxPage(pInfo->m_nCurPage+1);
}
//------------------------------------------------------------------------
///////////////////////// Commands ///////////////////////////////////////
void CSourceView::cmdOnEditReplace() {
    m_Navigation.Replace();
}
void CSourceView::cmdOnEditFind() {
    m_Navigation.Find(false);
}
void CSourceView::cmdOnEditFindCombo() 
{
	CBCGToolbarComboBoxButton* pSrcCombo = NULL;
	CObList listButtons;
	if (CBCGToolBar::GetCommandButtons (ID_EDIT_FIND_COMBO, listButtons) > 0) {
		for (POSITION posCombo = listButtons.GetHeadPosition (); pSrcCombo == NULL && posCombo != NULL;) {
			CBCGToolbarComboBoxButton* pCombo = DYNAMIC_DOWNCAST (CBCGToolbarComboBoxButton, listButtons.GetNext (posCombo));
			if (pCombo != NULL && CBCGToolBar::IsLastCommandFromButton (pCombo))
				pSrcCombo = pCombo;
		}
	}
	if (pSrcCombo != NULL)
	{
		string_smart s(pSrcCombo->GetText ());
		if (!s.isempty()){
			m_Navigation.SetTextToFind(s.c_str());
			m_Navigation.Find(true);
		}
	}
	else
		m_Navigation.Find(false);
}
void CSourceView::cmdOnEditFindNext() {
	m_Navigation.Find(true);
}
void CSourceView::cmdupOnEditFindNext(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_Navigation.IsFindNextAvailable());
}
void CSourceView::cmdOnEditCopy() {
	m_Messenger.EditCopy();
}
void CSourceView::cmdOnEditCut() {
	m_Messenger.EditCut();
}
void CSourceView::cmdOnEditPaste() {
	m_Messenger.EditPaste();
}
void CSourceView::cmdOnEditSelectAll() {
	m_Messenger.EditSelectAll();
}
void CSourceView::cmdOnEditRedo() {
	m_Messenger.EditRedo();
}
void CSourceView::cmdOnEditUndo() {
	m_Messenger.EditUndo();
}
void CSourceView::cmdOnGotoline() {
    m_Navigation.GotoLine();
}
void CSourceView::cmdOnEditNextBookmark() {
	m_Navigation.GotoNextBookmark();
}
void CSourceView::cmdOnEditPrevBookmark() {
	m_Navigation.GotoPrevBookmark();
}
void CSourceView::cmdOnEditFindInFiles()
{
	m_Navigation.FindInFiles();
}
void CSourceView::cmdOnEditToggleBookmark() {
	int dwCurLine = m_Messenger.GetCurLine();
	(GetBookmark(dwCurLine))
		? ResetBookmark(dwCurLine)
		: SetBookmark(dwCurLine);
}
void CSourceView::cmdOnEditClearBookmarks() {
	m_Messenger.ClearAllMarkers(MARKER_BOOKMARK);
}
void CSourceView::cmdOnToggleBreakpoint()	{
	int dwCurLine = m_Messenger.GetCurLine();
	(GetBreakPoint(dwCurLine,BREAKPOINT_EX(this)))	
		? ResetBreakPoint(dwCurLine)
		: SetBreakPoint(BREAKPOINT_EX(this,dwCurLine));
}
void CSourceView::cmdOnDisableBreakpoint(){
	int dwCurLine = m_Messenger.GetCurLine();
	BREAKPOINT_EX bp(this); GetBreakPoint(dwCurLine,bp);
	EnableBreakpoint(dwCurLine,!bp.IsEnabled());
}

void CSourceView::cmdOnClearAllBreakpoints(){
	m_Messenger.ClearAllMarkers(MARKER_BREAKPOINT);
	m_Messenger.ClearAllMarkers(MARKER_DISABLE_BREAKPOINT);
	m_Breakpoints.clear();
	RaiseChangeEvent(evdChangeSourceDocInfo::changeBreakpoints);	
}
void CSourceView::cmdOnViewWhitespace() {
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);
	pStorage->Editor_ShowWhiteSpaces = !pStorage->Editor_ShowWhiteSpaces;
}
void CSourceView::cmdOnViewIndentGuides() {
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);
	pStorage->Editor_ShowIndentGuides = !pStorage->Editor_ShowIndentGuides;
}
void CSourceView::cmdOnViewFoldMargin() {
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);
	pStorage->Editor_ShowFoldMargin = !pStorage->Editor_ShowFoldMargin;
}
void CSourceView::cmdOnEditFoldOne() {
    m_Editor.ExpandCurrent(!m_Editor.IsCurExpanded());
}
void CSourceView::cmdOnEditFoldAll() {
	m_Editor.ExpandAll(!m_Editor.IsAllExpanded());
}
void CSourceView::cmdOnEditZoomIn() {
	m_Messenger.ZoomIn();
}
void CSourceView::cmdOnEditZoomOut() {
	m_Messenger.ZoomOut();	
}
void CSourceView::cmdOnEditZoomReset() {
	m_Messenger.ZoomReset();	
}
void CSourceView::cmdOnEditTabifySelection(){
	m_Editor.TabifySelection(true);
}
void CSourceView::cmdOnEditUntabifySelection(){
	m_Editor.TabifySelection(false);
}
void CSourceView::cmdOnEditUppercase(){
	m_Editor.Uppercase(true);
}
void CSourceView::cmdOnEditLowecase(){
	m_Editor.Uppercase(false);
}
void CSourceView::cmdOnIncreaseLineIndent(){
	m_Editor.IncreaseLineIndent(true);
}
void CSourceView::cmdOnDecreaseLineIndent(){
	m_Editor.IncreaseLineIndent(false);
}
void CSourceView::cmdOnFindDeclaration(){
	long nStart, nEnd;
	string_smart strText = m_Image.GetExpressionAt(m_Messenger.GetCurPos(),nStart, nEnd);

	if (!strText.isempty()){
		ITEMLOCATION aMetAt(GetDocument()->GetPath().c_str(),nStart,nEnd);
		m_Navigation.FindDeclaration(strText,aMetAt);
	}
}
void CSourceView::cmdOnEditAutoFormat(){
	m_Editor.FormatSelection();
}
void CSourceView::cmdOnEditCompleteWord(){
	m_Editor.AutoComplete();
}
//------------------------------------------------------------------------
///////////////////// Update commands ////////////////////////////////////
void CSourceView::cmdupOnIndicatorPOS(CCmdUI* pCmdUI)
{
	TCHAR str[32];
	long dwLine=m_Messenger.GetCurLine();
	long dwCol=m_Messenger.GetCurPos()-m_Messenger.GetPosFromLine(dwLine);
	if ((dwLine>1000) || (dwCol>1000))
		sprintf(str," Ln %d, Col %d", dwLine+1,dwCol+1);
	else
		sprintf(str," Line %d, Col %d", dwLine+1,dwCol+1);

	pCmdUI->Enable (TRUE);
	pCmdUI->SetText (str);
}
void CSourceView::cmdupOnIndicatorOVR(CCmdUI* pCmdUI){
	pCmdUI->Enable (m_Messenger.IsOvrMode());
}
void CSourceView::cmdupOnEditCopy(CCmdUI* pCmdUI) {
	pCmdUI->Enable (IsActiveWindow() && m_Messenger.IsSelected());
}
void CSourceView::cmdupOnEditCut(CCmdUI* pCmdUI) {
	pCmdUI->Enable (IsActiveWindow() && m_Messenger.IsSelected());
}
void CSourceView::cmdupOnEditPaste(CCmdUI* pCmdUI) {
	pCmdUI->Enable (IsActiveWindow() && m_Messenger.CanEditPaste());
}
void CSourceView::cmdupOnEditRedo(CCmdUI* pCmdUI) {
	pCmdUI->Enable (IsActiveWindow() && m_Messenger.CanEditRedo());
}
void CSourceView::cmdupOnEditUndo(CCmdUI* pCmdUI) {
	pCmdUI->Enable (IsActiveWindow() && m_Messenger.CanEditUndo());
}
void CSourceView::cmdupOnViewWhitespace(CCmdUI* pCmdUI){
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);
	pCmdUI->SetCheck(pStorage->Editor_ShowWhiteSpaces);
}
void CSourceView::cmdupOnViewIndentGuides(CCmdUI* pCmdUI) {
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);
	pCmdUI->SetCheck(pStorage->Editor_ShowIndentGuides);
}
void CSourceView::cmdupOnViewFoldMargin(CCmdUI* pCmdUI) {
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);
	pCmdUI->SetCheck(pStorage->Editor_ShowFoldMargin);
}
void CSourceView::cmdupOnEditFoldAll(CCmdUI* pCmdUI) {
	pCmdUI->SetText((m_Editor.IsAllExpanded()) ? CString(LPCTSTR(ID_EDIT_FOLD_ALL)) : CString(LPCTSTR(ID_EDIT_UNFOLD_ALL)));
}
void CSourceView::cmdupOnEditFoldCur(CCmdUI* pCmdUI) {
	pCmdUI->SetText((m_Editor.IsCurExpanded()) ? CString(LPCTSTR(ID_EDIT_FOLD_CUR)) : CString(LPCTSTR(ID_EDIT_UNFOLD_CUR)));
}
void CSourceView::cmdupOnEditAutoFormat(CCmdUI *pCmdUI){
	pCmdUI->Enable(m_Messenger.IsSelected());
}
void CSourceView::cmdupOnEditTabifySelection(CCmdUI *pCmdUI){
	pCmdUI->Enable(m_Messenger.IsSelected());
}
void CSourceView::cmdupOnEditUntabifySelection(CCmdUI *pCmdUI){
	pCmdUI->Enable(m_Messenger.IsSelected());
}
void CSourceView::cmdupOnEditUppercase(CCmdUI *pCmdUI)	{
	//Nothing
}
void CSourceView::cmdupOnEditLowecase(CCmdUI *pCmdUI) {
	//Nothing
}
void CSourceView::cmdupOnIncreaseLineIndent(CCmdUI *pCmdUI){
	//Nothing
}
void CSourceView::cmdupOnDecreaseLineIndent(CCmdUI *pCmdUI){
	//Nothing
}
void CSourceView::cmdupOnDisableBreakpoint(CCmdUI *pCmdUI)	{
	pCmdUI->Enable(GetBreakPoint(m_Messenger.GetCurLine(),BREAKPOINT_EX(this)));
}
void CSourceView::cmdupOnFindDeclaration(CCmdUI *pCmdUI)	{
	long nStart, nEnd;
	string_smart strText = m_Image.GetExpressionAt(m_Messenger.GetCurPos(),nStart, nEnd);

	pCmdUI->Enable(!strText.isempty());
}
void CSourceView::cmdupOnEditCompleteWord(CCmdUI *pCmdUI){
}
void CSourceView::cmdOnEditSyncExplorer()
{
	IClassTreeLink * pExplorer = i_objects::GetInterface<IClassTreeLink>();
	ASSERT_RETURN_VOID(pExplorer);
	
	long nPos = m_Messenger.GetCurPos();
	string_smart strDocName = GetDocument()->GetPath();

	IDocMngrLink *pDocMngr = i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngr);

	IProjectDocLink * pProject=pDocMngr->FindParentProject(strDocName.c_str());
	AfxGetApp()->DoWaitCursor(1);
	
	LOG_PROTECT_CODE_BEGIN
	//В одном из проектов есть такой файл, все нормально
	if (pProject){
		for (int i=0; (i<100) && (nPos>=0); i++){
			if (pExplorer->SelectSyntaxItem(pProject->GetPath().c_str(),strDocName.c_str(),nPos))
				return;
			//Передвигаемся на слово вверх, в надежде встретить слово, которое подтолкнет нас
			//к решению
			long nRight;
			m_Image.GetNearestLeftWord(nPos,nPos,nRight);
		}
	}
	LOG_PROTECT_CODE_END
	AfxGetApp()->DoWaitCursor(-1);
}
