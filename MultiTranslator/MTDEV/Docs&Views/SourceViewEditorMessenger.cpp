/********************************************************************
	created:	2004/07/11
	created:	11:7:2004   21:28
	filename: 	X:\MultiTranslator\sources\mtdev\docs&views\sourcevieweditormessenger.cpp
	file path:	X:\MultiTranslator\sources\mtdev\docs&views
	file base:	sourcevieweditormessenger
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реализация функций редактора. Каждая функция 
				представляет собой вызов одной или нескольких команд (Message)
				редактора

*********************************************************************/

#include "stdafx.h"
#include "SourceView.h"


CSourceView::CEditorMessenger::CEditorMessenger(): m_hEditorWnd(0),m_pWndProc(NULL),m_pContext(NULL)
{
}

//Связывем данный класс и окно ркдактора 
void CSourceView::CEditorMessenger::SetWnd(HWND hEditorWnd)
{
	ASSERT(hEditorWnd);
	m_hEditorWnd=hEditorWnd;
	m_pWndProc = (LRESULT(__cdecl *)(void *,UINT,WPARAM,LPARAM))
		::SendMessage(m_hEditorWnd,SCI_GETDIRECTFUNCTION,0,0);
	m_pContext = (void *)
		::SendMessage(m_hEditorWnd,SCI_GETDIRECTPOINTER,0,0);
}

HWND CSourceView::CEditorMessenger::GetWnd() const			
{
	return m_hEditorWnd;
}

//Выполнение команд
LRESULT CSourceView::CEditorMessenger::Perform(UINT Msg, WPARAM wParam/*=0*/, LPARAM lParam/*=0*/) 
{
	if (!(m_pWndProc && m_pContext))
		return FALSE;
	return m_pWndProc(m_pContext,Msg, wParam, lParam);
}

LRESULT CSourceView::CEditorMessenger::Perform(UINT Msg, WPARAM wParam, const char* pParam) 
{
	if (!(m_pWndProc && m_pContext))
		return FALSE;
	return m_pWndProc(m_pContext,Msg, wParam, (LPARAM)(pParam));
}

void CSourceView::CEditorMessenger::ZoomIn()
{
	Perform(SCI_ZOOMIN);
}

void CSourceView::CEditorMessenger::ZoomOut()
{
	Perform(SCI_ZOOMOUT);
}

void CSourceView::CEditorMessenger::ZoomReset()
{
	Perform(SCI_SETZOOM,0);
}

long CSourceView::CEditorMessenger::GetTextLen()
{						//Размер текста
	return Perform(SCI_GETTEXTLENGTH);
} 

void CSourceView::CEditorMessenger::GetText(string_smart & strBuffer)
{	//Весь текст
	strBuffer.reserve(GetTextLen()+2); 
	Perform(SCI_GETTEXT,GetTextLen()+1,strBuffer.buffer());
	strBuffer.invalidate();
}

void CSourceView::CEditorMessenger::SetText(cstr szBuffer)
{	
	Perform(SCI_BEGINUNDOACTION); //Если этого не сделать, будем иметь 2 операции:
	Perform(SCI_SETTEXT,0,szBuffer); //удаление и вставка
	Perform(SCI_ENDUNDOACTION);
}

string_smart CSourceView::CEditorMessenger::GetSelText()
{				//Только выделенный фрагмент
	string_smart strBuffer(GetSelEnd()-GetSelStart()+2);
	Perform(SCI_GETSELTEXT,0,strBuffer.buffer());
	strBuffer.invalidate();
	return strBuffer;
}

string_smart CSourceView::CEditorMessenger::GetLineText(long dwLine)
{//Текст строки
	string_smart strBuffer(GetLineLength(dwLine));
	Perform(SCI_GETCURLINE,strBuffer.buffer_size()-1,strBuffer.buffer());
	return strBuffer;
}

long CSourceView::CEditorMessenger::AddText(cstr szText, int iLen)
{	//Поблочное добавление текста, возвращает новую позицию курсора
	Perform(SCI_ADDTEXT, iLen,szText);
	return GetCurPos();
}

long CSourceView::CEditorMessenger::GetStyleAt(long dwPos)
{		//Взятие стиля по указанной позиции
	return Perform(SCI_GETSTYLEAT,dwPos);
}

long CSourceView::CEditorMessenger::InsertText(cstr szText, int iPos)
{//Вставка текста c одновременным перемещением курсора
	Perform(SCI_INSERTTEXT, iPos,szText);
	return GetCurPos();
}

long CSourceView::CEditorMessenger::GetLineCount()
{					//Кол-во строк
	return Perform(SCI_GETLINECOUNT);
}

long CSourceView::CEditorMessenger::GetCurPos ()
{						//Текущая поз курсора	
	return Perform(SCI_GETCURRENTPOS);
} 

long CSourceView::CEditorMessenger::GetCurLine()
{						//Текущая строка	
	return GetLineFromPos(GetCurPos());
}

long CSourceView::CEditorMessenger::GetFirstVisibleLine()
{ 			//Первая видимая строка
	return Perform(SCI_GETFIRSTVISIBLELINE);
}

long CSourceView::CEditorMessenger::GetLinesOnScreen()
{				//Кол-во строк на экране		
	return Perform(SCI_LINESONSCREEN);
}

long CSourceView::CEditorMessenger::GetPosFromLine(long dwLine)
{		//Первая  позиция по указанной строке
	return Perform(SCI_POSITIONFROMLINE,dwLine);
}

long CSourceView::CEditorMessenger::GetEndPosOfLine(long dwLine)
{		//Последняя позиция по указанной строке
	return Perform(SCI_GETLINEENDPOSITION,dwLine);
}

long CSourceView::CEditorMessenger::GetLineFromPos(long dwPos)
{		//Номер строки по указанной позиции
	return Perform(SCI_LINEFROMPOSITION,dwPos);
}

long CSourceView::CEditorMessenger::GetLineLength(long dwLine)
{		//Длина указанной строки
	return Perform(SCI_LINELENGTH,dwLine);
}

long CSourceView::CEditorMessenger::OffsetToStartLinePos(long dwPos)
{	//Вычисляет позицию левее указанной, являющуюся началом строки
	return GetPosFromLine(GetLineFromPos(dwPos));
}

long CSourceView::CEditorMessenger::OffsetToEndLinePos(long dwPos)
{	//Вычисляет позицию правее указанной, являющуюся концом строки
	return OffsetToStartLinePos(dwPos)+GetLineLength(GetLineFromPos(dwPos));
}

long CSourceView::CEditorMessenger::GetLineIndentation(long dwLine) 
{
	return Perform(SCI_GETLINEINDENTATION, dwLine);
}

void CSourceView::CEditorMessenger::SetLineIndentation(long dwLine, long dwIndent) 
{
	if (dwIndent>= 0) Perform(SCI_SETLINEINDENTATION, dwLine, dwIndent);
}
long CSourceView::CEditorMessenger::GetLineIndentPosition(long dwLine) 
{
	return Perform(SCI_GETLINEINDENTPOSITION, dwLine);
}

long CSourceView::CEditorMessenger::GetSelStart()
{						//Начало выделения
	return Perform(SCI_GETSELECTIONSTART);
}	

long CSourceView::CEditorMessenger::GetSelEnd()
{						//Конец выделения
	return Perform(SCI_GETSELECTIONEND);
}

long CSourceView::CEditorMessenger::GetMarginsWidth()
{					//Общая ширина полосок слева
	int i=0,res=0;
	for (; i<3; i++)
		res+=Perform(SCI_GETMARGINWIDTHN,i);
	return res;
}

void CSourceView::CEditorMessenger::SelectRange(long dwStart,long dwEnd)
{//Выдедить укуазанный диапазон
	Perform(SCI_SETSELECTIONSTART,dwStart);
	Perform(SCI_SETSELECTIONEND,dwEnd);
}

void CSourceView::CEditorMessenger::ReplaceSelection(cstr szNewText)
{	//Заменить текущее выделение текстом
	Perform(SCI_REPLACESEL,strlen(szNewText),szNewText);
}

//Переход к указанной позиции			
void CSourceView::CEditorMessenger::GotoPos(long dwPos)
{				
	Perform(SCI_GOTOPOS, dwPos);
}

//Переход к указанной позиции (строка:столбец)			
void CSourceView::CEditorMessenger::GotoPos(long dwLine, long dwCol)
{	
	GotoPos(GetPosFromLine(dwLine)+min(dwCol,GetLineLength(dwLine)));
}

//Переход к указанной строке
void CSourceView::CEditorMessenger::GotoLine(long dwLine)
{				
	Perform(SCI_ENSUREVISIBLE, dwLine);
	Perform(SCI_GOTOLINE, dwLine);
}	

//Принудительный показ 	указанного региона
void CSourceView::CEditorMessenger::EnsureRangeVisible(long nStartPos, long nEndPos)
{
	long nStartLine = GetLineFromPos(nStartPos);
	long nEndLine = GetLineFromPos(nEndPos);

	RECT clRect; 
	if (!::GetClientRect(m_hEditorWnd,&clRect))
		return;

	long nTopLine   = GetFirstVisibleLine();
	long nBottomLine = GetLineFromPos(Perform(SCI_POSITIONFROMPOINT,clRect.right,clRect.bottom));
	
	//Диапазон уже находится в заданных пределах
	if ((nStartLine>=nTopLine) && (nEndLine<=nBottomLine))
		return;
	
	//Начало указанного диапазона уже лежит в видимой части или где-то ниже
	//Двигаем вверх так, чтобы низ показался на экране, а если экрана не хватает - 
	//то подвигаем диапазон до верхней границы
	if (nStartLine>nTopLine)
		Perform(SCI_LINESCROLL,0,min(nStartLine-nTopLine,nEndLine-nBottomLine+1));
	else 
		Perform(SCI_LINESCROLL,0,-nTopLine+nStartLine);
}

//Получение координат текущей страницы экрана: где позиция курсора, где начало страницы, какое смещенние по x
void CSourceView::CEditorMessenger::GetViewPos(long & Pos, long & FirstLineVisible, long &FirstColVisible)
{
	Pos=GetCurPos();
	FirstLineVisible=GetFirstVisibleLine(); 
	FirstColVisible =Perform(SCI_GETXOFFSET);
}

//Установка координат текущей страницы
void CSourceView::CEditorMessenger::SetViewPos(long dwPos, long FirstLineVisible,long FirstColVisible)
{
	Perform(SCI_LINESCROLL,FirstColVisible,FirstLineVisible);GotoPos(dwPos);
}

bool CSourceView::CEditorMessenger::IsOvrMode()
{		//Указывает, включен ли режим замены
	return Perform(SCI_GETOVERTYPE)==1;
}

bool CSourceView::CEditorMessenger::IsSelected()
{		//Указывает, есть ли выделение текста
	return GetSelEnd()!=GetSelStart();
} 

//Команды редактирования
void CSourceView::CEditorMessenger::EditCut()		
{Perform(SCI_CUT);}

void CSourceView::CEditorMessenger::EditCopy()		
{Perform(SCI_COPY);}

void CSourceView::CEditorMessenger::EditPaste()	
{Perform(SCI_PASTE);}

void CSourceView::CEditorMessenger::EditClear()	
{Perform(SCI_CLEAR);}

void CSourceView::CEditorMessenger::EditSelectAll()
{Perform(SCI_SELECTALL);}

void CSourceView::CEditorMessenger::EditUndo()		
{Perform(SCI_UNDO);}

void CSourceView::CEditorMessenger::EditRedo()		
{Perform(SCI_REDO);}

bool CSourceView::CEditorMessenger::CanEditPaste() 
{return Perform(SCI_CANPASTE)!=0;}

bool CSourceView::CEditorMessenger::CanEditUndo()  
{return Perform(SCI_CANUNDO)!=0;}

bool CSourceView::CEditorMessenger::CanEditRedo()  
{return Perform(SCI_CANREDO)!=0;}

//------ Маркеры ---------------------------------------------------------
long CSourceView::CEditorMessenger::SetMarker(long dwLine,byte btMarker)
{ //Возвращает дескриптор маркера
	return Perform(SCI_MARKERADD, dwLine,btMarker );
}

void CSourceView::CEditorMessenger::ClearMarker(long dwLine,byte btMarker)
{
	Perform(SCI_MARKERDELETE, dwLine, btMarker);
}

void CSourceView::CEditorMessenger::ClearMarker(long dwHandle)
{
	Perform(SCI_MARKERDELETEHANDLE, dwHandle);
}

void CSourceView::CEditorMessenger::ClearAllMarkers(byte btMarker)
{
	Perform(SCI_MARKERDELETEALL, btMarker);
}

long CSourceView::CEditorMessenger::MarkerNext(long dwLine,byte btMarker)
{
	return Perform(SCI_MARKERNEXT, dwLine,1<<btMarker);
}

long CSourceView::CEditorMessenger::MarkerPrev(int dwLine,byte btMarker)
{
	return Perform(SCI_MARKERPREVIOUS, dwLine,1<<btMarker);
}

bool CSourceView::CEditorMessenger::IsMarkered(long dwLine,byte btMarker)
{
	return ((Perform(SCI_MARKERGET, dwLine) & (1 << btMarker))!=0);
}

long CSourceView::CEditorMessenger::GetMarkerLine(long dwHandle)
{ // -1 = not found
	return Perform(SCI_MARKERLINEFROMHANDLE, dwHandle);
}

//------ Подсказки -------------------------------------------------------
void CSourceView::CEditorMessenger::ShowTip(long nPos, cstr szText)
{
	m_strCurrTip=szText; Perform(SCI_CALLTIPSHOW,GetCurPos(),szText);
}

void CSourceView::CEditorMessenger::HideTip()
{
	m_strCurrTip.erase(); Perform(SCI_CALLTIPCANCEL);
}

bool CSourceView::CEditorMessenger::IsTipVisible()
{
	return Perform(SCI_CALLTIPACTIVE)!=0;
}

string_smart CSourceView::CEditorMessenger::GetCurTip()
{
	return m_strCurrTip.c_str();
}

//------ Установки -------------------------------------------------------
void CSourceView::CEditorMessenger::SetKeyWords(size_t nGroup, cstr szList)
{
	Perform(SCI_SETKEYWORDS, nGroup, szList);
}

// Are white space characters currently visible?
bool CSourceView::CEditorMessenger::GetViewWhiteSpace()
{
	return Perform(SCI_GETVIEWWS)==SCWS_VISIBLEALWAYS;
}

// Make white space characters invisible, always visible or visible outside indentation.
void CSourceView::CEditorMessenger::SetViewWhiteSpace(bool bVisible)
{
	Perform(SCI_SETVIEWWS, bVisible ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
}

// Is drawing done first into a buffer or direct to the screen?
bool CSourceView::CEditorMessenger::GetBufferedDraw()
{
	return Perform(SCI_GETBUFFEREDDRAW)!=0;
}

// If drawing is buffered then each line of text is drawn into a bitmap buffer
// before drawing it to the screen to avoid flicker.
void CSourceView::CEditorMessenger::SetBufferedDraw(bool buffered)
{
	Perform(SCI_SETBUFFEREDDRAW,buffered);
}

// Retrieve the visible size of a tab.
int CSourceView::CEditorMessenger::GetTabWidth()
{
	return Perform(SCI_GETTABWIDTH);
}

// Change the visible size of a tab to be a multiple of the width of a space character.
void CSourceView::CEditorMessenger::SetTabWidth(int tabWidth)
{
	Perform(SCI_SETTABWIDTH,tabWidth);
}

// Get the code page used to interpret the bytes of the document as characters.
int CSourceView::CEditorMessenger::GetCodePage()
{
	return Perform(SCI_GETCODEPAGE);
}

// Set the code page used to interpret the bytes of the document as characters.
void CSourceView::CEditorMessenger::SetCodePage(int codePage)
{
	Perform(SCI_SETCODEPAGE,codePage);
}

// Is the background of the line containing the caret in a different colour?
bool CSourceView::CEditorMessenger::GetCaretLineVisible()
{
	return Perform(SCI_GETCARETLINEVISIBLE)!=0;
}

// Display the background of the line containing the caret in a different colour.
void CSourceView::CEditorMessenger::SetCaretLineVisible(bool show)
{
	Perform(SCI_SETCARETLINEVISIBLE,show);
}

// Get the colour of the background of the line containing the caret.
COLORREF CSourceView::CEditorMessenger::GetCaretLineBack()
{
	return Perform(SCI_GETCARETLINEBACK);
}

// Set the colour of the background of the line containing the caret.
void CSourceView::CEditorMessenger::SetCaretLineBack(COLORREF color)
{
	Perform(SCI_SETCARETLINEBACK,color);
}

// Retrieve indentation size.
int CSourceView::CEditorMessenger::GetIndent()
{
	return Perform(SCI_GETINDENT);
}

// Set the number of spaces used for one level of indentation.
void CSourceView::CEditorMessenger::SetIndent(int indentSize)
{
	Perform(SCI_SETINDENT,indentSize);
}

// Indentation will only use space characters if useTabs is false, otherwise
// it will use a combination of tabs and spaces.
void CSourceView::CEditorMessenger::SetUseTabs(bool useTabs)
{
	Perform(SCI_SETUSETABS,useTabs);
}

// Retrieve whether tabs will be used in indentation.
bool CSourceView::CEditorMessenger::GetUseTabs()
{
	return Perform(SCI_GETUSETABS)!=0;
}

// Show or hide the horizontal scroll bar.
void CSourceView::CEditorMessenger::SetHScrollBar(bool show)
{
	Perform(SCI_SETHSCROLLBAR,show);
}

// Is the horizontal scroll bar visible?
bool CSourceView::CEditorMessenger::GetHScrollBar()
{
	return Perform(SCI_GETHSCROLLBAR)!=0;
}

// Show or hide the vertical scroll bar.
void CSourceView::CEditorMessenger::SetVScrollBar(bool show)
{
	Perform(SCI_SETVSCROLLBAR,show);
}

// Is the vertical scroll bar visible?
bool CSourceView::CEditorMessenger::GetVScrollBar()
{
	return Perform(SCI_GETVSCROLLBAR)!=0;
}


// Show or hide indentation guides.
void CSourceView::CEditorMessenger::SetIndentationGuides(bool show)
{
	Perform(SCI_SETINDENTATIONGUIDES,show);
}

// Are the indentation guides visible?
bool CSourceView::CEditorMessenger::GetIndentationGuides()
{
	return Perform(SCI_GETINDENTATIONGUIDES)!=0;
}

// Set the width of the insert mode caret.
void CSourceView::CEditorMessenger::SetCaretWidth(int pixelWidth)
{
	Perform(SCI_SETCARETWIDTH,pixelWidth);
}

// Returns the width of the insert mode caret.
int CSourceView::CEditorMessenger::GetCaretWidth()
{
	return Perform(SCI_GETCARETWIDTH);
}

// Set the lexing language of the document.
void CSourceView::CEditorMessenger::SetLexer(int lexer)
{
	Perform(SCI_SETLEXER,lexer);
}

// Retrieve the lexing language of the document.
int CSourceView::CEditorMessenger::GetLexer()
{
	return Perform(SCI_GETLEXER);
}

// Set the lexing language of the document based on string name.
void CSourceView::CEditorMessenger::SetLexerLanguage(string_adapter strLanguage)
{
	Perform(SCI_SETLEXERLANGUAGE,0,strLanguage.c_str());
}

//Set the foreground colour of the caret.
void CSourceView::CEditorMessenger::SetCaretFore(COLORREF fore)
{
	Perform(SCI_SETCARETFORE,fore);
}

//Get the foreground colour of the caret.
COLORREF CSourceView::CEditorMessenger::GetCaretFore()
{
	return Perform(SCI_GETCARETFORE);
}

//Set the foreground colour of the selection and whether to use this setting.
void CSourceView::CEditorMessenger::SetSelFore(bool useSetting, COLORREF fore)
{
	Perform(SCI_SETSELFORE,useSetting,fore);
}

//Set the background colour of the selection and whether to use this setting.
void CSourceView::CEditorMessenger::SetSelBack(bool useSetting, COLORREF back)
{
	Perform(SCI_SETSELBACK,useSetting,back);
}

// Set a margin to be either numeric or symbolic.
void CSourceView::CEditorMessenger::SetMarginType(int margin, int marginType)
{
	Perform(SCI_SETMARGINTYPEN,margin,marginType);
}

// Retrieve the type of a margin.
int CSourceView::CEditorMessenger::GetMarginType(int margin)
{
	return Perform(SCI_GETMARGINTYPEN,margin);
}

// Set the width of a margin to a width expressed in pixels.
void CSourceView::CEditorMessenger::SetMarginWidth(int margin, int pixelWidth)
{
	Perform(SCI_SETMARGINWIDTHN,margin, pixelWidth);
}

// Retrieve the width of a margin in pixels.
int CSourceView::CEditorMessenger::GetMarginWidth(int margin)
{
	return Perform(SCI_GETMARGINWIDTHN,margin);
}

// Make a margin sensitive or insensitive to mouse clicks.
void CSourceView::CEditorMessenger::SetMarginMouseSensitive(int margin, bool sensitive)
{
	Perform(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

// Retrieve the mouse click sensitivity of a margin.
bool CSourceView::CEditorMessenger::GetMarginMouseSensitive(int margin)
{
	return Perform(SCI_GETMARGINSENSITIVEN,margin)!=0;
}
// Set up a value that may be used by a lexer for some optional feature.
void CSourceView::CEditorMessenger::SetProperty(const string_adapter & strKey, const string_adapter & strValue)
{
	Perform(SCI_SETPROPERTY,reinterpret_cast<WPARAM>(strKey.c_str()),strValue.c_str());
}
// Set up a value that may be used by a lexer for some optional feature.
void CSourceView::CEditorMessenger::SetProperty(const string_adapter & strKey, bool bValue)
{
	if (bValue)
		Perform(SCI_SETPROPERTY,reinterpret_cast<WPARAM>(strKey.c_str()),"1");
	else
		Perform(SCI_SETPROPERTY,reinterpret_cast<WPARAM>(strKey.c_str()),"0");

}
// Measure the pixel width of some text in a particular style.
int CSourceView::CEditorMessenger::GetTextWidth(int style, string_adapter strText)
{
	return Perform(SCI_TEXTWIDTH,style,strText.c_str());
}
//Read-only
bool CSourceView::CEditorMessenger::GetReadOnly()
{
	return Perform(SCI_GETREADONLY)!=FALSE;
}
//Set to read only or read write.
void CSourceView::CEditorMessenger::SetReadOnly(bool bReadOnly)
{
	Perform(SCI_SETREADONLY,bReadOnly);
}

//Set the colours used as a chequerboard pattern in the fold margin
void CSourceView::CEditorMessenger::SetFoldMarginBackColor(bool useSetting, COLORREF back)
{
	Perform(SCI_SETFOLDMARGINCOLOUR,useSetting,back);
}

void CSourceView::CEditorMessenger::SetFoldMarginForeColor(bool useSetting, COLORREF fore)
{
	Perform(SCI_SETFOLDMARGINHICOLOUR,useSetting,fore);
}

//Change the way control characters are displayed:
//If symbol is < 32, keep the drawn way, else, use the given character.
void CSourceView::CEditorMessenger::SetControlCharSymbol(int symbol)
{
	Perform(SCI_SETCONTROLCHARSYMBOL,symbol);
}

//Get the way control characters are displayed.
int CSourceView::CEditorMessenger::GetControlCharSymbol()
{
	return Perform(SCI_GETCONTROLCHARSYMBOL);
}

// Sets whether text is word wrapped.
void CSourceView::CEditorMessenger::SetWordWrap(bool bWrap)
{
	Perform(SCI_SETWRAPMODE, (bWrap) ? SC_WRAP_WORD : SC_WRAP_NONE);
}

//Retrieve whether text is word wrapped.
bool CSourceView::CEditorMessenger::GetWordWrap()
{
	return (Perform(SCI_GETWRAPMODE)==SC_WRAP_WORD) ;
}

//Set all properties of the style
void CSourceView::CEditorMessenger::SetStyle(const ISourceDocStylerLink::style_definition & aStyle)
{
	Perform(SCI_STYLESETITALIC,		aStyle.m_nStyleID, aStyle.m_bItalics);
	Perform(SCI_STYLESETBOLD,		aStyle.m_nStyleID, aStyle.m_bBold);
	Perform(SCI_STYLESETFONT,		aStyle.m_nStyleID, aStyle.m_strFont.c_str());
	Perform(SCI_STYLESETFORE,		aStyle.m_nStyleID, color_to_rgb(aStyle.m_clFore));
	Perform(SCI_STYLESETBACK,		aStyle.m_nStyleID, color_to_rgb(aStyle.m_clBack));
	Perform(SCI_STYLESETSIZE,		aStyle.m_nStyleID, aStyle.m_iSize);
	Perform(SCI_STYLESETEOLFILLED,	aStyle.m_nStyleID, aStyle.m_bEolFilled);
	Perform(SCI_STYLESETUNDERLINE,	aStyle.m_nStyleID, aStyle.m_bUnderlined);
	Perform(SCI_STYLESETCASE,		aStyle.m_nStyleID, aStyle.m_iCaseForce);
	Perform(SCI_STYLESETCHARACTERSET,aStyle.m_nStyleID,aStyle.m_nCharset);
}
