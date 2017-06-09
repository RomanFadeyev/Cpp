/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   12:57
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\SourceView.h
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	SourceView
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Реализация отображения документа исходного файла (SourceFile)
*********************************************************************/

#pragma once

//#include <NotifyWnd.h>		//Нужно для отображения сообщений
#include <Scintilla.h>

#include "SourceDoc.h"		//Нужно для CSourceView
#include "BaseStructs.h"	//Нужно для vbdFind 
#include "View_B.h"			//Базовый класс
#include "CobjectEx.h"		//Один из родителей

#define MARKER_BOOKMARK				1
#define MARKER_BREAKPOINT			2
#define MARKER_DISABLE_BREAKPOINT	3
#define MARKER_COMMAND_POINTER		4

class CSourceView : public CView_B, public ISourceViewLink
{
	//Чуть расширенный класс точки останова
	struct BREAKPOINT_EX: public BREAKPOINT{
		BREAKPOINT_EX(CSourceView * pOwner): BREAKPOINT(pOwner,pOwner->GetDocument()->GetPath().c_str()){
		}
		BREAKPOINT_EX(CSourceView * pOwner,long dwL, bool bE=true, cstr szC=NULL, long dwHC=0):
			BREAKPOINT(pOwner,pOwner->GetDocument()->GetPath().c_str(),dwL,bE,szC,dwHC){
		}
	};
	
	friend class CSourceDoc;
	//////////////////////////////////////////////////////////////////////
	//Класс для обеспечения непосредственной связи с "Scintilla"
public:
	class CEditorMessenger
	{
		HWND			m_hEditorWnd;
		string_smart	m_strCurrTip;
		LRESULT (*m_pWndProc)(void*,UINT,WPARAM,LPARAM);
		void *m_pContext;
	public:
		CEditorMessenger();

		//Связывем данный класс и окно ркдактора 
		void SetWnd(HWND hEditorWnd);
		HWND GetWnd()	const;
		//Выполнение команд
		LRESULT Perform(UINT Msg, WPARAM wParam=0, LPARAM lParam=0);
		LRESULT Perform(UINT Msg, WPARAM wParam, const char* pParam);

		//Масштаб
		void ZoomIn();
		void ZoomOut();
		void ZoomReset();

		//Управление текстом
		long GetTextLen();//Размер текста
		void GetText(string_smart & strBuffer);	//Весь текст
		void SetText(cstr szBuffer);			//Установить текст в редактор	
		string_smart GetSelText();				//Только выделенный фрагмент
		string_smart GetLineText(long dwLine);	//Текст строки
		long AddText(cstr szText, int iLen);	//Поблочное добавление текста, возвращает новую позицию курсора
		long GetStyleAt(long dwPos);			//Взятие стиля по указанной позиции
		long InsertText(cstr szText, int iPos);//Вставка текста c одновременным перемещением курсора
		
		//Вычисление позиций
		long GetLineCount();					//Кол-во строк
		long GetCurPos ();						//Текущая поз курсора	
		long GetCurLine();						//Текущая строка	
		long GetFirstVisibleLine(); 			//Первая видимая строка
		long GetLinesOnScreen();				//Кол-во строк на экране		
		long GetPosFromLine(long dwLine);		//Первая  позиция по указанной строке
		long GetEndPosOfLine(long dwLine);		//Последняя позиция по указанной строке
		long GetLineFromPos(long dwPos);		//Номер строки по указанной позиции
		long GetLineLength(long dwLine);		//Длина указанной строки
		void GetViewPos(long & Pos, long & FirstLineVisible, long &FirstColVisible);//Получение координат текущей страницы экрана: 
																					//где позиция курсора, где начало страницы, какое смещенние по x

		long OffsetToStartLinePos(long dwPos);	//Вычисляет позицию левее указанной, являющуюся началом строки
		long OffsetToEndLinePos(long dwPos);	//Вычисляет позицию правее указанной, являющуюся концом строки

		//Перемещение
		void GotoPos(long dwPos);				//Переход к указанной позиции			
		void GotoPos(long dwLine, long dwCol);	//Переход к указанной позиции (строка:столбец)			
		void GotoLine(long dwLine);				//Переход к указанной строке
		void SetViewPos(long dwPos, long FirstLineVisible,long FirstColVisible);//Установка координат текущей страницы
		void EnsureRangeVisible(long nStartPos, long nEndPos); //Принудительный показ 	указанного региона
		
		//Отступы для отдельной строки
		long GetLineIndentation(long dwLine) ;
		void SetLineIndentation(long dwLine, long dwIndent) ;
		long GetLineIndentPosition(long dwLine) ;
		
		//????
		long GetMarginsWidth();					//Общая ширина полосок слева
		bool IsOvrMode();						//Указывает, включен ли режим замены

		//Выделение
		long GetSelStart();						//Начало выделения
		long GetSelEnd();						//Конец выделения
		void SelectRange(long dwStart,long dwEnd);//Выдедить укуазанный диапазон
		void ReplaceSelection(cstr szNewText);	//Заменить текущее выделение текстом
		bool IsSelected();						//Указывает, есть ли выделение текста
		 
		//Команды редактирования
		void EditCut();
		void EditCopy();
		void EditPaste();
		void EditClear();
		void EditSelectAll();
		void EditUndo();
		void EditRedo();
		bool CanEditPaste();
		bool CanEditUndo();
		bool CanEditRedo();
		
		//Маркеры
		long SetMarker(long dwLine,byte btMarker); //Возвращает дескриптор маркера
		void ClearMarker(long dwLine,byte btMarker);
		void ClearMarker(long dwHandle);
		void ClearAllMarkers(byte btMarker);
		long MarkerNext(long dwLine,byte btMarker);
		long MarkerPrev(int dwLine,byte btMarker);
		bool IsMarkered(long dwLine,byte btMarker);
		long GetMarkerLine(long dwHandle); // -1 = not found

		//Подсказки
		void ShowTip(long nPos, cstr szText);
		void HideTip();
		bool IsTipVisible();
		string_smart GetCurTip();

		//Установки редактора
		void SetKeyWords(size_t nGroup, cstr szList);
		// Set the lexing language of the document based on string name.
		void SetLexerLanguage(string_adapter strLanguage);
		
		bool GetViewWhiteSpace();			// Are white space characters currently visible?
		void SetViewWhiteSpace(bool bVisible);// Make white space characters invisible, always visible or visible outside indentation.
		
		bool GetBufferedDraw();	// Is drawing done first into a buffer or direct to the screen?
		void SetBufferedDraw(bool buffered);	// If drawing is buffered then each line of text is drawn into a bitmap buffer
												// before drawing it to the screen to avoid flicker.

		int GetTabWidth();				// Retrieve the visible size of a tab.
		void SetTabWidth(int tabWidth);	// Change the visible size of a tab to be a multiple of the width of a space character.
			
		int GetCodePage();				// Get the code page used to interpret the bytes of the document as characters.
		void SetCodePage(int codePage);	// Set the code page used to interpret the bytes of the document as characters.
	
		bool GetCaretLineVisible();			// Is the background of the line containing the caret in a different colour?
		void SetCaretLineVisible(bool show);// Display the background of the line containing the caret in a different colour.
	
		COLORREF GetCaretLineBack();			// Get the colour of the background of the line containing the caret.
		void SetCaretLineBack(COLORREF color);	// Set the colour of the background of the line containing the caret.
	
		int GetIndent();					// Retrieve indentation size.
		void SetIndent(int indentSize);		// Set the number of spaces used for one level of indentation.

		void SetUseTabs(bool useTabs);	// Indentation will only use space characters if useTabs is false, otherwise
		bool GetUseTabs();				// Retrieve whether tabs will be used in indentation.
			
		void SetHScrollBar(bool show);	// Show or hide the horizontal scroll bar.
		bool GetHScrollBar();			// Is the horizontal scroll bar visible?

		void SetVScrollBar(bool show);	// Show or hide the vertical scroll bar.
		bool GetVScrollBar();			// Is the vertical scroll bar visible?

		void SetIndentationGuides(bool show);// Show or hide indentation guides.
		bool GetIndentationGuides();		// Are the indentation guides visible?

		void SetCaretWidth(int pixelWidth);	// Set the width of the insert mode caret.
		int GetCaretWidth();				// Returns the width of the insert mode caret.
		
		void SetCaretFore(COLORREF fore);	//Set the foreground colour of the caret.
		COLORREF GetCaretFore();			//Get the foreground colour of the caret.

		void SetLexer(int lexer);		// Set the lexing language of the document.
		int GetLexer();					// Retrieve the lexing language of the document.

		void SetSelFore(bool useSetting, COLORREF fore); //Set the foreground colour of the selection and whether to use this setting.
		void SetSelBack(bool useSetting, COLORREF back); //Set the background colour of the selection and whether to use this setting.

		void SetMarginType(int margin, int marginType);	// Set a margin to be either numeric or symbolic.
		int  GetMarginType(int margin);					// Retrieve the type of a margin.
		void SetMarginWidth(int margin, int pixelWidth);// Set the width of a margin to a width expressed in pixels.
		int  GetMarginWidth(int margin);					// Retrieve the width of a margin in pixels.
		void SetMarginMouseSensitive(int margin, bool sensitive);// Make a margin sensitive or insensitive to mouse clicks.
		bool GetMarginMouseSensitive(int margin);			// Retrieve the mouse click sensitivity of a margin.

		// Measure the pixel width of some text in a particular style.
		int GetTextWidth(int style, string_adapter strText);

		void SetProperty(const string_adapter & strKey, bool bValue); // Set up a value that may be used by a lexer for some optional feature.
		void SetProperty(const string_adapter & strKey, const string_adapter & strValue); // Set up a value that may be used by a lexer for some optional feature.

		//Set to read only or read write.
		bool GetReadOnly();
		void SetReadOnly(bool bReadOnly);

		//Set the colours used as a chequerboard pattern in the fold margin
		void SetFoldMarginBackColor(bool useSetting, COLORREF back);
		void SetFoldMarginForeColor(bool useSetting, COLORREF fore);
		
		//Change the way control characters are displayed:
		//If symbol is < 32, keep the drawn way, else, use the given character.
		void SetControlCharSymbol(int symbol);
		//Get the way control characters are displayed.
		int GetControlCharSymbol();

		// Sets whether text is word wrapped.
		void SetWordWrap(bool bWrap);
		//Retrieve whether text is word wrapped.
		bool GetWordWrap();
		
		//Set all properties of the style			
		void SetStyle(const ISourceDocStylerLink::style_definition & aStyle);
	};
	//--------------------------------------------------------------------
	//Класс для отображения содержимого редактора
public:
	class CEditorContents
	{
	public:
		//Способ, которым собираются читать данные (с начала вперед, с конца назад, случайно)
		//Это влияет на формирование границ окна образа
		enum ELookupDirection{ldForward,ldBackward,ldRandom};
	private:
		CharacterRange	 m_CurRange;	//Текущее окно
		ELookupDirection m_LookupDir;	//Режим захвата окна
		long			 m_dwCaretPos;	//Текущее положение
		long			 m_dwTextSize;	//Общий объем текста
		CEditorMessenger&m_rMessenger;
		//Размер "окна", в пределах которого идет отображение
		//и смещение чтения окна относительно указанной позиции влево или вправо 
		//в зависимости от ELookupDirection
		enum {dwBufferSize=512, dwSlopSize=dwBufferSize/8};
		char m_aImage[dwBufferSize+1];
		void GetImage(int dwPosition);	//Получение образа для ldForward берется как 
		//Start=dwPosition-dwSlopSize, End=Start+dwBufferSize
		unsigned char GetChar(long nPos){
			ASSERT(nPos>=0);
			if (!IsInCache(nPos))	//Если необходимо, обновляем кеш
				GetImage(nPos);	//
			return m_aImage [nPos-m_CurRange.cpMin];
		}
	public:
		CEditorContents (CSourceView & aOwner); 		
		//Установка приблизительного направления чтения из образа
		void SetLookupDirection(ELookupDirection LookupDir){m_LookupDir=LookupDir;}
		//Принудительное обнаружение прложения курсора
		long DetectCaretPos(){
			return m_dwCaretPos=m_rMessenger.GetCurPos();}
		//Получение позиции курсора либо из внутренних структур 
		//Необходимо пользоваться данной функцией очень острожно, так как автоматического
		//обновления m_dwCaretPos не выполняется. Данную функцию удобно использовать для сравнения 
		//нового и старого положений курсора.
		long GetCaretPos(){
			return m_dwCaretPos; }
		//Принудительный подсчет объема текста
		long DetectTextSize(){
			return m_dwTextSize=m_rMessenger.GetTextLen();}
		//Возвр. общий объем текста
		long GetTextSize(){
			return IsValid() ? m_dwTextSize: DetectTextSize();}
		//Проверка действительности образа
		bool IsValid()	 {
			return m_CurRange.cpMin>=0;}
		//Объявление образа недействительным
		void Invalidate(){
			_RPT0(_CRT_WARN,"Invalidating image...\n");
			m_CurRange.cpMax=m_CurRange.cpMin=-1;}

		//Возвращает слово (от пробела/начала строки/текста до пробела/конца строки/текста)
		string_smart GetWordAt(long dwPos, OUT long & nStart, OUT long & nEnd);

		//Ищет законченное слово слева от указанной позиции, причем
		//если курсор уже стоит на каком-то слове, оно пропускается
		string_smart GetNearestLeftWord(long dwPos,long & nStart, long & nEnd);

		//Возвращает текст в зависимости от условий, может, например вернуть строку в кавычках, или просто слово
		string_smart GetExpressionAt(long dwPos,OUT long & nStart, OUT long & nEnd);

		string_smart GetTextRange(long dwStart, long dwEnd);
		//Проверяет, попадает ли указанный символ в текущий кеш
		inline bool IsInCache(long dwPos){
			return inrange(dwPos,m_CurRange.cpMin,m_CurRange.cpMax-1);
		}
		//Оператор выбора символа по индексу
		inline char operator [](int dwPos){
			return GetChar(dwPos);
		}
	};
	
	//////////////////////////////////////////////////////////////////////
	//Класс для обеспечения работы диалогов поиска, замены, перехода и т.д.
public:
	class CNavigation
	{
	private:
		mtdev_lib::IUtilsPtr			m_spUtils;
		IFindTextLink::vbdFind			m_FindInfo;
		IReplaceTextLink::vbdReplace	m_ReplaceInfo;
		list<long>						m_lstReplacedPositions;

		CEditorMessenger& m_rMessenger;
		CEditorContents	& m_rImage;
	protected:
		void	ShowWarning(const string_smart & strText);
		bool	ReplaceCallback(IReplaceTextLink::vbdReplace::operations lOp,HWND hwnd);
		bool	FindNextRoutine(bool bShowNotFound=true);
		bool	ReplaceNextRoutine(bool bShowNotFound=true);
	public:
		CNavigation(CSourceView & aOwner);
		bool	Find(bool bFindNext); //bool bFindNext - продолжение поиска без диалога
		bool	IsFindNextAvailable(); //Если можно продолжать без диалога
		void    SetTextToFind(cstr szText);
		bool	Replace();
		bool	GotoLine();
		bool	GotoNextBookmark();
		bool	GotoPrevBookmark();
		bool 	FindInFiles();
		bool	FindDeclaration(const string_smart & strText, const ITEMLOCATION & aMetAt);
	};

	//////////////////////////////////////////////////////////////////////
	//Класс для работы с редактором "Scintilla"
public:
	class CEditor: public CCObjectEx, public ISourceDocStylerLink::IEventSink
	{
		/// Codes representing the effect a line has on indentation.
		enum IndentationStatus {
			isNone,			// no effect on indentation
			isBlockStart,	// indentation block begin such as "{" or VB "function"
			isBlockEnd,		// indentation end indicator such as "}" or VB "end"
			isKeyWordStart	// Keywords that cause indentation
		};

		string_cstr    m_strDocFileExt;
		ISourceDocStylerLink::IDocumentStyleDescriptor * m_pDocStyleDescriptor;

		ISourceDocStylerLink::style_and_words m_StatementIndent;
		ISourceDocStylerLink::style_and_words m_StatementEnd;
		ISourceDocStylerLink::style_and_words m_BlockStart;
		ISourceDocStylerLink::style_and_words m_BlockEnd;
		bool	m_bIndentOpening;
		bool	m_bIndentClosing;
		size_t	m_nIndentSize;
			
		CEditorMessenger &m_rMessenger;
		CEditorContents  &m_rImage;
		CSourceView &  m_rOwner;		
	public:
		enum EMargins{mgnLines= 0,mgnBase=1,mgnFold = 2};
	private:
		static HMODULE m_hModule;
		//Кеш наиболее распространенных слов, набираемых пользователем
		list <string_smart> m_lstMostUsedWords;
		typedef list<string_smart>::iterator most_used_it;
		//Список известных слов слов, которые надо показывать всегда
		list <string_smart> m_lstKnownWords;
		typedef list<string_smart>::iterator known_it;

		//хранилище настроек (не реестр)
		ISourceDocStylerLink * GetDocStyler() const;

		//Настройка редактора в зависимости от установленных настроек
		void Tune();
		void SetMargins();		//Это подчасти функции Tune
		void SetViewSettins();	//
		void SetDrawSettings();	//
		void SetReadonlySettings();//
		void SetStyles();

		//Функции подсветки скобок
		bool BraceFindMatching(int &braceAtCaret, int &braceOpposite);
		void BraceMatch();
		
		//Функции свертки
		void FoldUpdate(long dwLine, long dwLevelNow, long dwLevelPrev);
		void FoldExpand(long &dwLine, bool doExpand, bool Force, long dwVisLevels=0, long dwLevel=-1);
		void FoldClick(long dwPosition, long dwModifiers);

		//Элементы автоформатирования
		size_t	GetLinePartsInStyle(long line, int style1, int style2, string_smart sv[], size_t len);
		bool	RangeIsAllWhitespace(long start, long end);
		long	IndentOfBlock(long nLine) ;
		void	SetLineIndentation(long nLine, long nIndent);
		IndentationStatus GetIndentState(long nLine);		
		
		void CharAdded(char ch); //Вызывается при нажатии символа в редакторе		
		
		//From CCObjectEx
		override void OnStorageDataChanged(IStorageLink::EParam nParameterChanged);
		//From ISourceDocStylerLink::IEventSink
		override void ISourceDocStylerLink::IEventSink::evOnChangeData() {LOG_PROTECT_CODE(Tune());}
	public:
		bool	m_bModified;	//Вынесена в public специально	
								//владелец данного класса должен периодически опрашивать 
								//данную переменную на предмет появившихся изменений
								//данная переменная этиим классом никогда не ставится в false, только в true
								//AfterLoad и AfterSave

		CEditor(CSourceView & aOwner);
		~CEditor();
		
		//Проверка рабоспособности
		cstr GetEditorClassName();
		//Глобальная инициализация
		void Initialize();                          
		//Перед открытием документа
		void BeforeLoad();
		//После открытия документа
		void AfterLoad();
		//Перед сохр документа
		void BeforeSave();
		//После сохр документа
		void AfterSave();
		//Развертка всех блоков
		void ExpandAll(bool bExpand);
		//Возвращает true, если все блоки раскрыты
		bool IsAllExpanded();
		//Раскрывает/сворачивает текущий блок, если таковой имеется
		void ExpandCurrent(bool bExpand);
		//Возвращает true, если текущий блок раскрыт
		bool IsCurExpanded();
		//Автоформат выделенного фрагмента текста
		void FormatSelection();
		//Замена пробелов табуляциями и наоборот 
		void TabifySelection(bool bTabify);
		//Изменение регистра
		void Uppercase(bool bUpper);
		//Добавление/удаление отступов
		void IncreaseLineIndent(bool bIncrease);	
		//Автоматический подбор отступов при переводе строки
		void AutomaticIndentation(char ch);
		//Подсказка слов
		bool AutoComplete();
		//Всплывающая подсказка слова на основе его начальных букв
		bool AutoTip(string_smart & strLeftPart);

		void OnNotifyMsg(WPARAM wParam, LPARAM lParam);
		void OnCmdMsg(WPARAM wParam, LPARAM lParam);
	};

	//Класс - утилита, позволяющий легко и просто добавлять и удалять состояния документа 
	//Удобно использовать в пределах процедуры. Основной плюс - корректное снятие
	//установленного флага даже при возникновения исключения
	class CAutoState
	{
	private:
		CSourceView & m_View;
		long		m_nState;
	public:
		CAutoState(CSourceView & Doc, long nState):m_View(Doc),m_nState(nState){
			ASSERT(!(m_View.m_nState & m_nState));
			m_View.m_nState|=m_nState;
		}
		void Rollback(){	//Снимает состояние раньше времени.
			ASSERT_VALID(&m_View);
			m_View.m_nState&=~m_nState;
		}
		~CAutoState(){
			Rollback();
		}
	};
	friend class CAutoState;
	
protected:
	typedef vector<BREAKPOINT> breakpoint_vector;
	enum {opChangingBreakpoints=1};
	CEditor 			m_Editor;
	CEditorContents		m_Image;
	CEditorMessenger	m_Messenger;
	CNavigation 		m_Navigation;
	breakpoint_vector	m_Breakpoints;	//Список всех точек останова
	long				m_nState;		//Состояние объекта
	DWORD				m_nTimerID;

	//Printing .....
	//Кол-во символов после каждый страницы при печати
	typedef vector<long> num_vector;
	typedef num_vector::iterator num_vector_it;
	num_vector	m_lstPrintPages;
	CAutoFont*	pHeaderFont;
	//.........
	
	//Для отслеживания текущего указателя команды
	static CSourceView *	pCurComandPointerView;
	static DWORD			nCommandPointerHandle;
	
protected: 
	CSourceView();// create from serialization only
	void	RaiseChangeEvent(int EventType);
	bool	IsActiveWindow();
	
	DECLARE_DYNCREATE(CSourceView)
		
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSourceView)
	virtual void OnDraw(CDC* pDC);	// overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	
	//{{AFX_MSG(CSourceView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void cmdOnViewFoldAll();
	afx_msg void cmdOnViewFoldOne();
	afx_msg void cmdOnEditReplace();
	afx_msg void cmdOnEditFind();
	afx_msg void cmdOnEditFindCombo();
	afx_msg void cmdOnGotoline();
	afx_msg void cmdupOnIndicatorPOS(CCmdUI* pCmdUI);
	afx_msg void cmdupOnIndicatorOVR(CCmdUI* pCmdUI);
	afx_msg void cmdOnEditFindNext();
	afx_msg void cmdupOnEditFindNext(CCmdUI* pCmdUI);
	afx_msg void cmdOnEditFindInFiles();
	afx_msg void cmdOnEditCopy();
	afx_msg void cmdOnEditCut();
	afx_msg void cmdOnEditPaste();
	afx_msg void cmdOnEditRedo();
	afx_msg void cmdOnEditUndo();
	afx_msg void cmdupOnEditCopy(CCmdUI* pCmdUI);
	afx_msg void cmdupOnEditCut(CCmdUI* pCmdUI);
	afx_msg void cmdupOnEditPaste(CCmdUI* pCmdUI);
	afx_msg void cmdupOnEditRedo(CCmdUI* pCmdUI);
	afx_msg void cmdupOnEditUndo(CCmdUI* pCmdUI);
	afx_msg void cmdOnEditNextBookmark();
	afx_msg void cmdOnEditPrevBookmark();
	afx_msg void cmdOnEditToggleBookmark();
	afx_msg void cmdOnEditClearBookmarks();
	afx_msg void cmdOnEditSelectAll();
	afx_msg void cmdOnViewWhitespace();
	afx_msg void cmdupOnViewWhitespace(CCmdUI* pCmdUI);
	afx_msg void cmdOnViewIndentGuides();
	afx_msg void cmdupOnViewIndentGuides(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void cmdOnEditFoldOne();
	afx_msg void cmdOnEditFoldAll();
	afx_msg void cmdupOnEditFoldAll(CCmdUI* pCmdUI);
	afx_msg void cmdOnViewFoldMargin();
	afx_msg void cmdupOnViewFoldMargin(CCmdUI* pCmdUI);
	afx_msg void cmdOnEditZoomIn();
	afx_msg void cmdOnEditZoomOut();
	afx_msg void cmdupOnEditFoldCur(CCmdUI* pCmdUI);
	afx_msg void cmdOnEditZoomReset();
	afx_msg void cmdOnEditAutoFormat();	
	afx_msg void cmdupOnEditAutoFormat(CCmdUI *pCmdUI);
	afx_msg void cmdOnEditTabifySelection();
	afx_msg void cmdupOnEditTabifySelection(CCmdUI *pCmdUI);
	afx_msg void cmdOnEditUntabifySelection();
	afx_msg void cmdupOnEditUntabifySelection(CCmdUI *pCmdUI);
	afx_msg void cmdOnEditUppercase();
	afx_msg void cmdupOnEditUppercase(CCmdUI *pCmdUI);
	afx_msg void cmdOnEditLowecase();
	afx_msg void cmdupOnEditLowecase(CCmdUI *pCmdUI);
	afx_msg void cmdOnIncreaseLineIndent();
	afx_msg void cmdupOnIncreaseLineIndent(CCmdUI *pCmdUI);
	afx_msg void cmdOnDecreaseLineIndent();
	afx_msg void cmdupOnDecreaseLineIndent(CCmdUI *pCmdUI);
	afx_msg void cmdOnToggleBreakpoint();
	afx_msg void cmdOnClearAllBreakpoints();
	afx_msg void cmdOnDisableBreakpoint();
	afx_msg void cmdupOnDisableBreakpoint(CCmdUI *pCmdUI);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void cmdOnFindDeclaration();
	afx_msg void cmdupOnFindDeclaration(CCmdUI *pCmdUI);
	afx_msg void cmdOnEditCompleteWord();
	afx_msg void cmdupOnEditCompleteWord(CCmdUI *pCmdUI);
	afx_msg void cmdOnEditSyncExplorer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	CSourceDoc* GetDocument();
	
	virtual void OnInitialUpdate();
	virtual ~CSourceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//Внешние операции, наследники от ISourceViewLink. Для внутренних 
	//целей лучше их не использовать, а пользоваться аналогичными функциями
	//m_Messenger для повышения производительности
	
	override CView* GetObject()							{return this;}
	override HWND GetWnd()								{return m_Messenger.GetWnd();}
	override ISourceDocLink* GetDocumentLink() const	{return dynamic_cast<ISourceDocLink*> (CView::GetDocument());}

	override long GetTextLen()							{return m_Messenger.GetTextLen();}
	override void GetText(string_smart & strBuffer)		{m_Messenger.GetText(strBuffer);}
	override void SetText(cstr szBuffer)				{m_Messenger.SetText(szBuffer);}	
	override void GetSelText(string_smart & strBuffer)	{strBuffer=m_Messenger.GetSelText();}
	override void GetLineText(string_smart & strBuffer,long dwLine){strBuffer=m_Messenger.GetLineText(dwLine);}
	override long AddText(cstr szText, size_t nLen)		{return m_Messenger.AddText(szText, nLen);}
	override long InsertText(cstr szText, size_t nPos)	{return m_Messenger.InsertText(szText,nPos);}
	override long GetLineCount()		{return m_Messenger.GetLineCount();}
	override long GetCurPos ()			{return m_Messenger.GetCurPos();}
	override long GetCurLine()			{return m_Messenger.GetCurLine();}
	override long GetFirstVisibleLine()	{return m_Messenger.GetFirstVisibleLine();}
	override long GetLinesOnScreen()	{return m_Messenger.GetLinesOnScreen();}
	override long GetPosFromLine(long dwLine)		{return m_Messenger.GetPosFromLine(dwLine);}
	override long GetEndPosOfLine(long dwLine)		{return m_Messenger.GetEndPosOfLine(dwLine);}
	override long GetLineFromPos(long dwPos)		{return m_Messenger.GetLineFromPos(dwPos);}
	override long GetLineLength(long dwLine)		{return m_Messenger.GetLineLength(dwLine);}
	override long OffsetToStartLinePos(long dwPos)	{return m_Messenger.OffsetToStartLinePos(dwPos);}	
	override long OffsetToEndLinePos(long dwPos)	{return m_Messenger.OffsetToEndLinePos(dwPos);}		
	
	override long GetSelStart()			{return m_Messenger.GetSelStart();}
	override long GetSelEnd()			{return m_Messenger.GetSelEnd();}
	override void SelectRange(long dwStart,long dwEnd)	{m_Messenger.SelectRange(dwStart,dwEnd);}
	override void ReplaceSelection(cstr szNewText)		{m_Messenger.ReplaceSelection(szNewText);}	
	override bool IsSelected()			{return m_Messenger.IsSelected();}

	override void GetViewPos(long & Pos, long & FirstLineVisible, long &FirstColVisible) {m_Messenger.GetViewPos(Pos, FirstLineVisible, FirstColVisible);}
	override void SetViewPos(long Pos, long FirstLineVisible,long FirstColVisible){m_Messenger.SetViewPos(Pos, FirstLineVisible, FirstColVisible);}

	override void GotoPos(long dwPos)							{m_Messenger.GotoPos(dwPos);}	
	override void GotoPos(long dwLine, long dwCol)				{m_Messenger.GotoPos(dwLine, dwCol);}
	override void GotoLine(long dwLine)							{m_Messenger.GotoLine(dwLine);}
	override void EnsureRangeVisible(long nStartPos,long nEndPos){m_Messenger.EnsureRangeVisible(nStartPos,nEndPos);}

//	bool RetrieveBreakpoint(long dwLine, breakpoint_vector::iterator & i);
	override void SetBreakPoint(BREAKPOINT & Breakpoint);
	override bool GetBreakPoint(long dwLine, BREAKPOINT & Breakpoint);	
	override void ResetBreakPoint(long dwLine);
	override void EnableBreakpoint(long dwLine, bool bEnable);	
	override void GetBreakPoints(vector<BREAKPOINT> & Breakpoints);

	override void SetBookmark(long dwLine)						{m_Messenger.SetMarker(dwLine,MARKER_BOOKMARK);}
	override void ResetBookmark(long dwLine)					{m_Messenger.ClearMarker(dwLine,MARKER_BOOKMARK);}
	override bool GetBookmark(long dwLine)						{return m_Messenger.IsMarkered(dwLine,MARKER_BOOKMARK);}
	override void GetBookmarks(vector<long> & bookmarks);
	override void SetCommandMarker(long dwLine);
	override IDebugManageLink* GetDebugManageLink()				{return this;}
	
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
};
