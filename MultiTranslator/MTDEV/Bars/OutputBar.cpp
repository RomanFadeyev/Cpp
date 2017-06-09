/********************************************************************
	created:	2002/05/12
	created:	12:5:2002   15:51
	filename: 	$(Projects)\sources\mtdev\bars\outputbar.cpp
	file path:	$(Projects)\sources\mtdev\bars

	file base:	outputbar
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Docked-bar с страницами для вывода Build, Execute, Find 
*********************************************************************/

#include "stdafx.h"
#include <BaseUtils.h>

#include "Resource.h"
#include "OutputBar.h"

#include "Names.h"
#include "BaseStructs.h"

using namespace i_objects;

#define ADDED_TO_TAIL 1 //Отмечает в CM_REALIGN, что добавлялись записи в конец списка
#define PAGE_BUILD 0
//#define PAGE_OUTPUT_SCREEN 1

//Создание переходника для объекта
static i_objects::object_thunk<CbarOutput> barOutput(__MSG_PREFIX__);

CFont	CbarOutput::COutputList_B::m_Font;
///////////////////////////////////////////////////////////////////////////
// CbarOutput::COutputList_B
///////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
typedef CbarOutput::COutputList_B CBarListOutput_B;
BEGIN_MESSAGE_MAP(CBarListOutput_B, CListBox)
//BEGIN_MESSAGE_MAP(CbarOutput::COutputList_B, CListBox)
	ON_MESSAGE_VOID(CM_ACTIVATE,msgOnActivate)
	ON_MESSAGE(CM_REALIGN,msgOnRealign)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, cmdOnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, cmdupOnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, cmdOnEditClear)
	ON_WM_CREATE()
END_MESSAGE_MAP()

//------------------------------------------------------------------------
void CbarOutput::COutputList_B::StartSession(bool bClear/*=true*/)
{
	if (bClear)
		Clear();
	m_cLastSymbol=0;
	//Убеждаемся, что наш Bar отображается на экране
	//Не вызывать напрямую, только через SendMessage, 
	//так как эта процедура может вызваться из другого
	//потока, а нам нужно вернуться в свой родной
	SendMessage(CM_ACTIVATE,0,0);
}
//------------------------------------------------------------------------
void CbarOutput::COutputList_B::StopSession(bool bClear/*=false*/)
{
	if (bClear)
		Clear();

	//Убеждаемся, что наш Bar отображается на экране
	SendMessage(CM_ACTIVATE,0,0);
	SendMessage(WM_VSCROLL,SB_BOTTOM,0);
}
//-------------------------------------------------------------------------
void CbarOutput::COutputList_B::Print(cstr szText)
{
	ASSERT_RETURN_VOID(szText);
	if (!m_hWnd) return;
	bool bDisableRedraw=false;
	size_t nOrigCount = GetCount();

	//Отключаем smooth
	BOOL bSmoothScroll=false;
	SystemParametersInfo(SPI_GETLISTBOXSMOOTHSCROLLING, 0, &bSmoothScroll, 0);

	if (bSmoothScroll)
		SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, 0, FALSE, 0);
	
	LOG_PROTECT_CODE_BEGIN
	string_smart strText(szText); strText.replace("\t","    ");
	string_list strStrings(strText.c_str());

	for (size_t i=0;i<strStrings.items().count();i++){
		if ((i==0) && (GetCount()>0) && (m_cLastSymbol!='\n')){
			SendMessage(WM_SETREDRAW, false, 0);	
			bDisableRedraw=true;
			CString s; 
			CListBox::GetText(GetCount()-1,s);
			CListBox::DeleteString(GetCount()-1);
			s+=strStrings.items()[i];
			CListBox::AddString(s.TrimRight());
		}
		else
			AddString(strStrings.items().get_at(i).trimright().c_str());
	}
	SendMessage(WM_VSCROLL,SB_BOTTOM,0);	
	if (bDisableRedraw)
		SendMessage(WM_SETREDRAW, true, 0);	

	if (nOrigCount<500)	//Защита: если строк больше, то приложение будет надолго подвисать, да и выравнивание уже неактуально при таком объеме строк
		PostMessage(CM_REALIGN,ADDED_TO_TAIL,nOrigCount);	
										//Отправляем асинхронно, "на потом", 
										//чтобы не ступорить вывод при большой нагрузке и объеме
	
	m_cLastSymbol =(strStrings.isempty()) 
			? 0
			: m_cLastSymbol =strStrings.c_str()[strStrings.length()-1];
	LOG_PROTECT_CODE_END

	//Восстанавливаем анимацию
	if (bSmoothScroll)
		SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, 0, LPVOID(TRUE), 0);
}
//-------------------------------------------------------------------------
string_smart CbarOutput::COutputList_B::GetText()
{
	long nLen=1;
	for (int i =0; i<GetCount();i++)
		nLen+=__super::GetTextLen(i);
	
	string_smart s(nLen+GetCount());
	for (int i =0; i<GetCount();i++){
		CString ss; __super::GetText(i,ss);
		s.cat(ss).cat("\r\n");
	}
	return s;
}
//-------------------------------------------------------------------------
bool CbarOutput::COutputList_B::ExtractPositionMthd1(IN cstr szMessage, OUT string_smart & strFileName, OUT long & nLine, long &nPos)
{
	//Формат строки сообщения N1: 
	//[xxx....]file <имя файла, можно в кавычках>: [xxx....] <Line X, Pos Y>
	//Наличие двоеточия после имени файла обязательно
	//Обрамление позиции в угловые скобки обязательно, наличие ключевых слов 
	//file, Line, Pos (с учетом регистра) обязательно
	//Наличие запятой между строкой и столбцом в позиции обязательно
	char * szLeft, *szRight;
	//- Поиск имени файла --------------
	string_parse strText; strText=szMessage;
	string_smart strTemp;
	if (!strText.extractbetween("file",": ",strFileName, szLeft, szRight))
		return false;
	strFileName.trim().frames_delete('\'','\'').frames_delete('\"','\"');
	
	//- Поиск номера строки ------------
	strText=szRight+1;
	if (!strText.extractbetween("<Line",",",strTemp, szLeft, szRight))
		return false;
	nLine = atol(strTemp.trim().c_str());
	//- Поиск номера столбца ------------
	strText=szRight+1;
	if (!strText.extractbetween("Pos",">",strTemp, szLeft, szRight))
		return false;
	nPos = atol(strTemp.trim().c_str());
	return true;
}
//-------------------------------------------------------------------------
bool CbarOutput::COutputList_B::ExtractPositionMthd2(IN cstr szMessage, OUT string_smart & strFileName, OUT long & nLine, long &nPos)
{
	//Формат строки сообщения N2: 
	//<имя файла, можно в кавычках><<xx,yy>> [....]
	//Если Pos не найдено, все равно возвращается true, лишь бы был номер строки

	//- Поиск имени файла --------------
	string_parse strText; strText.AddSkipRanges("\"","\""); 
	strText=szMessage;
	
	//Ищем строку
	cstr szTemp = strText.find("<");
	if (!szTemp)
		return false;
	
	//Итак, имя файла
	strFileName.assign(strText.c_str(),szTemp-strText.c_str());
	strFileName.trim().frames_delete('\'','\'').frames_delete('\"','\"');
	
	//- Поиск номера строки ------------
	string_smart strTemp;
	for (szTemp=strltrim(szTemp+1);(*szTemp) && isdigit(*szTemp);szTemp++)
		strTemp.cat(*szTemp);
	
	if (strTemp.isempty())
		return false;
	nLine = atoi(strTemp.c_str());
	
	//- Поиск номера столбца ------------
	szTemp=strltrim(szTemp);
	strTemp.erase();
	if (*szTemp==',')
		for (szTemp=strltrim(szTemp+1);(*szTemp) && isdigit(*szTemp);szTemp++)
			strTemp.cat(*szTemp);
	if (!strTemp.isempty())	
		nPos = atoi(strTemp.c_str());
	else
		nPos=1;
	return true;
}
//-------------------------------------------------------------------------
bool CbarOutput::COutputList_B::ExtractPositionMthd3(IN cstr szMessage, OUT string_smart & strFileName, OUT long & nLine, long &nPos)
{
	//Формат строки сообщения N2: 
	//<имя файла, можно в кавычках>:xx,yy [....]
	//Если Pos не найдено, все равно возвращается true, лишь бы был номер строки

	//- Поиск имени файла --------------
	string_parse strText; strText.AddSkipRanges("\"","\""); 
	strText=szMessage;
	
	//Ищем строку
	cstr szTemp = strText.find(":");
	if (!szTemp)
		return false;
	if (szTemp[1]=='\\') //Если мы наткнулись на двоеточие после имени диска
		szTemp = strchr(szTemp+2,':');
	if (!szTemp)
		return false;	

	//Итак, имя файла
	strFileName.assign(strText.c_str(),szTemp-strText.c_str());
	strFileName.trim().frames_delete('\'','\'').frames_delete('\"','\"');
	
	//- Поиск номера строки ------------
	string_smart strTemp;
	for (szTemp=strltrim(szTemp+1);(*szTemp) && isdigit(*szTemp);szTemp++)
		strTemp.cat(*szTemp);
	
	if (strTemp.isempty())
		return false;
	nLine = atoi(strTemp.c_str());
	
	//- Поиск номера столбца ------------
	szTemp=strltrim(szTemp);
	strTemp.erase();
	if (*szTemp==',')
		for (szTemp=strltrim(szTemp+1);(*szTemp) && isdigit(*szTemp);szTemp++)
			strTemp.cat(*szTemp);
	if (!strTemp.isempty())	
		nPos = atoi(strTemp.c_str());
	else
		nPos=1;
	return true;
}
//-------------------------------------------------------------------------
bool CbarOutput::COutputList_B::ExtractPosition(IN cstr szMessage, OUT string_smart & strFileName, OUT long & nLine, long &nPos)
{
	bool bRes = ExtractPositionMthd1(szMessage,strFileName,nLine,nPos) ||
				ExtractPositionMthd2(szMessage,strFileName,nLine,nPos) ||
				ExtractPositionMthd3(szMessage,strFileName,nLine,nPos);
	return bRes;
}
//-------------------------------------------------------------------------
bool CbarOutput::COutputList_B::LocateToPos(cstr szFileName, long nLine, long nPos)
{
	CDocument* pDoc = AfxGetApp()->OpenDocumentFile(szFileName);
	//Приводим к документу исходников (полученный документ должен быть с интерфейсом ISourceDocLink )
	ISourceDocLink * pDocIface = dynamic_cast <ISourceDocLink *>(pDoc);
	if (!pDoc)
		return false;
	pDocIface->GetViewLink()->GetObject()->SetFocus();
	pDocIface->GetViewLink()->GotoPos(nLine,nPos);
	return true;
}
//------------------------------------------------------------------------
BOOL CbarOutput::COutputList_B::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=LBS_NOINTEGRALHEIGHT |  WS_HSCROLL  | WS_VSCROLL;
	return __super::PreCreateWindow(cs);
}
//------------------------------------------------------------------------
int CbarOutput::COutputList_B::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Устанавливаем шрифт
	if (HFONT(m_Font)==NULL)
		m_Font.CreateStockObject (DEFAULT_GUI_FONT);
	
	SetOwner (&m_Owner);
	SetFont(&m_Font);
	return 0;
}
//------------------------------------------------------------------------
void CbarOutput::COutputList_B::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	__super::OnWindowPosChanging(lpwndpos);
	// Hide horizontal scrollbar:
	ShowScrollBar (SB_HORZ, FALSE);
	ModifyStyle (WS_HSCROLL, 0, SWP_DRAWFRAME);
}
//------------------------------------------------------------------------
void CbarOutput::COutputList_B::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	SetFocus(); //Не убирать
	GetWorkspace()->ShowPopupMenu (IDR_POPUP_OUTPUT, point, AfxGetMainWnd ());
}
//------------------------------------------------------------------------	
void CbarOutput::COutputList_B::msgOnActivate()
{
	m_Owner.SendMessage(CM_SHOW,0,0);

	CBCGTabWnd* pTabWnd = DYNAMIC_DOWNCAST(CBCGTabWnd,GetParent());
	ASSERT_RETURN_VOID(pTabWnd);
	if (pTabWnd->GetTabFromHwnd(m_hWnd)!=-1)
		pTabWnd->SetActiveTab(pTabWnd->GetTabFromHwnd(m_hWnd));
}
//------------------------------------------------------------------------
LRESULT CbarOutput::COutputList_B::msgOnRealign(WPARAM wParam, LPARAM lParam)
{
	//Если в очерели сообщений есть еще такие же сообщения, данную обработку опускаем
	//Т.О. мы всегда реагируем на самое последнее сообщение
	MSG Msg;
	if (PeekMessage(&Msg, m_hWnd, CM_REALIGN,CM_REALIGN, PM_NOREMOVE))
		return 0;

	CClientDC dc (&m_Owner);
	CFont* pOldFont = dc.SelectObject (&m_Font);
	
	int cxExtentMax = (wParam=ADDED_TO_TAIL) ? GetHorizontalExtent() : 0 ;
	int cxExtentMaxOrig = cxExtentMax;
	
	for (int i = (wParam=ADDED_TO_TAIL) ? lParam : 0, j=GetCount (); i<j ; i ++)
	{
		CString strItem; CListBox::GetText (i, strItem);
		cxExtentMax = max (cxExtentMax, dc.GetTextExtent (strItem).cx+10);
	}
	if (cxExtentMax!=cxExtentMaxOrig)	
		SetHorizontalExtent (cxExtentMax);
	dc.SelectObject (pOldFont);

	m_Owner.PostMessage(CM_REALIGN,0,0);
	return 0;
}
//------------------------------------------------------------------------
void CbarOutput::COutputList_B::cmdOnEditCopy() 
{
	string_smart strText = GetText();
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
void CbarOutput::COutputList_B::cmdupOnEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(	(::GetFocus()==m_hWnd) &&
					(GetCount()>0) && 
					((GetCount()>1) || __super::GetTextLen(0)>0));
}
//------------------------------------------------------------------------
void CbarOutput::COutputList_B::cmdOnEditClear() 
{
	Clear();
}
//------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// CbarOutput::COutputBuild
///////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
typedef CbarOutput::COutputBuild CBarBuildOutput;
BEGIN_MESSAGE_MAP(CBarBuildOutput, COutputList_B )
//BEGIN_MESSAGE_MAP(CbarOutput::COutputBuild, CListBox)
	ON_COMMAND(ID_BUILD_NEXT_ERROR, cmdOnNextError)
	ON_COMMAND(ID_BUILD_PREV_ERROR, cmdOnPrevError)
	ON_UPDATE_COMMAND_UI(ID_BUILD_NEXT_ERROR, cmdupOnNextError)
	ON_UPDATE_COMMAND_UI(ID_BUILD_PREV_ERROR, cmdupOnPrevError)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarOutput::COutputBuild::cmdOnNextError()
{
	for (int i=GetCurSel()+1;i<GetCount();i++)
	{
		CString Str; 
		SetCurSel(i);CListBox::GetText( GetCurSel(),Str);	
		
		string_smart strFileName;
		long nLine,nPos;
		if (ExtractPosition(Str,strFileName,nLine,nPos)) {
			LocateToPos(strFileName.c_str(),nLine-1,nPos-1);
			break;
		}
	}
}
//-------------------------------------------------------------------------
void CbarOutput::COutputBuild::cmdOnPrevError()
{
	for (int i=GetCurSel()-1;i>=0;i--)
	{
		CString Str; 
		SetCurSel(i);CListBox::GetText( GetCurSel(),Str);	
		
		string_smart strFileName;
		long nLine,nPos;
		if (ExtractPosition(Str,strFileName,nLine,nPos)) {
			LocateToPos(strFileName.c_str(),nLine-1,nPos-1);
			break;
		}
	}
}
//-------------------------------------------------------------------------
void CbarOutput::COutputBuild::cmdupOnNextError(CCmdUI *pCmdUI)
{
	IDocMngrLink * pDocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	pCmdUI->Enable((pDocMngrLink) && (pDocMngrLink->GetActiveProject()));
}
//-------------------------------------------------------------------------
void CbarOutput::COutputBuild::cmdupOnPrevError(CCmdUI *pCmdUI)
{
	IDocMngrLink * pDocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	pCmdUI->Enable((pDocMngrLink) && (pDocMngrLink->GetActiveProject()));
}
//------------------------------------------------------------------------
void CbarOutput::COutputBuild::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);
	if (GetCurSel()==-1)	//Не убирать
		return;				//

	CString Str; 
	CListBox::GetText( GetCurSel(),Str);	

	string_smart strFileName;
	long nLine,nPos;
	if (ExtractPosition(Str,strFileName,nLine,nPos)) 
		LocateToPos(strFileName.c_str(),nLine-1,nPos-1);
}

///////////////////////////////////////////////////////////////////////////
// CbarOutput::COutputParse
///////////////////////////////////////////////////////////////////////////
typedef CbarOutput::COutputParse CBarParseOutput;
BEGIN_MESSAGE_MAP(CBarParseOutput, COutputList_B)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CbarOutput::COutputParse::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	const cstr szFrames[][2] = {{"file: \"","\""},	// xxx file: "file_name" xxx
								{"file: \'","\'"},	// xxx file: 'file_name' xxx
								{"file \"",	"\""},	// xxx file "file_name" xxx
								{"file \'",	"\'"},	// xxx file 'file_name' xxx
								{"\"",		"\""},	// xxx "file_name" xxx
								{"\'",		"\'"}	// xxx 'file_name' xxx
								};

	__super::OnLButtonDblClk(nFlags, point);

	if (GetCurSel()==-1)			//???
		return;						//

	CString Str; CListBox::GetText(GetCurSel(),Str);	
	
	string_parse strText = Str;
	string_smart strFileName;

	//Перебираем все стандартные способы указания имени файла
	for (size_t i =0; i<arrlength(szFrames);i++)
		if (strText.extractbetween(szFrames[i][0],szFrames[i][1],strFileName))
		{
			LocateToPos(strFileName.c_str(),0,0);
			break;
		}
}
///////////////////////////////////////////////////////////////////////////
// CbarOutput::COutputFindInFiles
///////////////////////////////////////////////////////////////////////////
typedef CbarOutput::COutputFindInFiles CBarFinFOutput;
BEGIN_MESSAGE_MAP(CBarFinFOutput, COutputList_B)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarOutput::COutputFindInFiles::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);
	if (GetCurSel()==-1)	//Не убирать
		return;				//

	CString Str; 
	CListBox::GetText( GetCurSel(),Str);	

	string_smart strFileName;
	long nLine,nPos;
	if (ExtractPosition(Str,strFileName,nLine,nPos)) 
		LocateToPos(strFileName.c_str(),nLine-1,nPos-1);
}

//------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////
// CbarOutput
///////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4355) //using "this"in member intialization list
CbarOutput::CbarOutput():	m_wndOutputBuild(*this),
							m_wndOutputParse(*this),
							m_wndOutputFindInFiles(*this)
{
}
//------------------------------------------------------------------------
CbarOutput::~CbarOutput()
{}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CbarOutput, Cbar_B)
	//{{AFX_MSG_MAP(CbarOutput)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_OUTPUT, cmdOnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, cmdupOnViewOutput)
	ON_MESSAGE(CM_REALIGN,msgOnRealign)
	//}}AFX_MSG_MAP 
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarOutput::evOnCloseDocument(evdCloseDocumentInfo &Info)
{
	//Смотрим, если не осталось ни одного открытого проекта, очищаем 
	//все экраны
	IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();	
	ASSERT_RETURN_VOID(pDocMngrLink);

	if (pDocMngrLink->GetObject()->GetOpenDocumentCount()==1) //Последний документ
	{
		m_wndOutputBuild.Clear();
		m_wndOutputParse.Clear();
	}
}
//------------------------------------------------------------------------
int CbarOutput::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	//m_Font.CreateStockObject (DEFAULT_GUI_FONT);
	CRect rectClient (0, 0, lpCreateStruct->cx, lpCreateStruct->cy);

	// Create tabs window:
	if (!m_wndTabs.Create (CBCGTabWnd::STYLE_FLAT_SHARED_HORZ_SCROLL, rectClient, this, 101))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle =  WS_CHILD|WS_VISIBLE;

	m_wndOutputBuild.Create (dwStyle , rectClient, &m_wndTabs, 1);
	m_wndOutputParse.Create  (dwStyle, rectClient, &m_wndTabs, 2);
	m_wndOutputFindInFiles.Create (dwStyle, rectClient, &m_wndTabs, 3);

	// Attach views to tab:
	m_wndTabs.AddTab (&m_wndOutputBuild, m_wndOutputBuild.GetTitle(), -1);
	m_wndTabs.AddTab (&m_wndOutputParse, m_wndOutputParse.GetTitle(), -1);
	m_wndTabs.AddTab (&m_wndOutputFindInFiles, m_wndOutputFindInFiles.GetTitle(), -1);
	
	//Tune the look of the bar
	m_wndTabs.SetTabBorderSize(FLAT_TABS ? 0 : DEFAULT_TAB_BORDER_SIZE);
	m_wndTabs.SetFlatFrame (FLAT_TABS);
	
	return 0;
}
//------------------------------------------------------------------------
void CbarOutput::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);

    CRect rc; GetClientRect(rc);

	m_wndTabs.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height (),SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	for (int i=0;i<m_wndTabs.GetTabsNum();i++)	
		m_wndTabs.GetTabWnd(i)->SendMessage(CM_REALIGN,0,0);
}
//------------------------------------------------------------------------
LRESULT CbarOutput::msgOnRealign(WPARAM wParam, LPARAM lParam)
{
	//Если в очерели сообщений есть еще такие же сообщения, данную обработку опускаем
	//Т.О. мы всегда реагируем на самое последнее сообщение
	MSG Msg;
	if (PeekMessage(&Msg, m_hWnd, CM_REALIGN,CM_REALIGN, PM_NOREMOVE))
		return 0;

	m_wndTabs.SynchronizeScrollBar();
	return 0;
}
//------------------------------------------------------------------------
void CbarOutput::cmdOnViewOutput()
{
	ToggleVisible();
}
//------------------------------------------------------------------------
void CbarOutput::cmdupOnViewOutput(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(IsVisible());
}
//------------------------------------------------------------------------