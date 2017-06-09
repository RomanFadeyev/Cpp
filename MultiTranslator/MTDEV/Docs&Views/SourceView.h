/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   12:57
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\SourceView.h
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	SourceView
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	���������� ����������� ��������� ��������� ����� (SourceFile)
*********************************************************************/

#pragma once

//#include <NotifyWnd.h>		//����� ��� ����������� ���������
#include <Scintilla.h>

#include "SourceDoc.h"		//����� ��� CSourceView
#include "BaseStructs.h"	//����� ��� vbdFind 
#include "View_B.h"			//������� �����
#include "CobjectEx.h"		//���� �� ���������

#define MARKER_BOOKMARK				1
#define MARKER_BREAKPOINT			2
#define MARKER_DISABLE_BREAKPOINT	3
#define MARKER_COMMAND_POINTER		4

class CSourceView : public CView_B, public ISourceViewLink
{
	//���� ����������� ����� ����� ��������
	struct BREAKPOINT_EX: public BREAKPOINT{
		BREAKPOINT_EX(CSourceView * pOwner): BREAKPOINT(pOwner,pOwner->GetDocument()->GetPath().c_str()){
		}
		BREAKPOINT_EX(CSourceView * pOwner,long dwL, bool bE=true, cstr szC=NULL, long dwHC=0):
			BREAKPOINT(pOwner,pOwner->GetDocument()->GetPath().c_str(),dwL,bE,szC,dwHC){
		}
	};
	
	friend class CSourceDoc;
	//////////////////////////////////////////////////////////////////////
	//����� ��� ����������� ���������������� ����� � "Scintilla"
public:
	class CEditorMessenger
	{
		HWND			m_hEditorWnd;
		string_smart	m_strCurrTip;
		LRESULT (*m_pWndProc)(void*,UINT,WPARAM,LPARAM);
		void *m_pContext;
	public:
		CEditorMessenger();

		//�������� ������ ����� � ���� ��������� 
		void SetWnd(HWND hEditorWnd);
		HWND GetWnd()	const;
		//���������� ������
		LRESULT Perform(UINT Msg, WPARAM wParam=0, LPARAM lParam=0);
		LRESULT Perform(UINT Msg, WPARAM wParam, const char* pParam);

		//�������
		void ZoomIn();
		void ZoomOut();
		void ZoomReset();

		//���������� �������
		long GetTextLen();//������ ������
		void GetText(string_smart & strBuffer);	//���� �����
		void SetText(cstr szBuffer);			//���������� ����� � ��������	
		string_smart GetSelText();				//������ ���������� ��������
		string_smart GetLineText(long dwLine);	//����� ������
		long AddText(cstr szText, int iLen);	//��������� ���������� ������, ���������� ����� ������� �������
		long GetStyleAt(long dwPos);			//������ ����� �� ��������� �������
		long InsertText(cstr szText, int iPos);//������� ������ c ������������� ������������ �������
		
		//���������� �������
		long GetLineCount();					//���-�� �����
		long GetCurPos ();						//������� ��� �������	
		long GetCurLine();						//������� ������	
		long GetFirstVisibleLine(); 			//������ ������� ������
		long GetLinesOnScreen();				//���-�� ����� �� ������		
		long GetPosFromLine(long dwLine);		//������  ������� �� ��������� ������
		long GetEndPosOfLine(long dwLine);		//��������� ������� �� ��������� ������
		long GetLineFromPos(long dwPos);		//����� ������ �� ��������� �������
		long GetLineLength(long dwLine);		//����� ��������� ������
		void GetViewPos(long & Pos, long & FirstLineVisible, long &FirstColVisible);//��������� ��������� ������� �������� ������: 
																					//��� ������� �������, ��� ������ ��������, ����� ��������� �� x

		long OffsetToStartLinePos(long dwPos);	//��������� ������� ����� ���������, ���������� ������� ������
		long OffsetToEndLinePos(long dwPos);	//��������� ������� ������ ���������, ���������� ������ ������

		//�����������
		void GotoPos(long dwPos);				//������� � ��������� �������			
		void GotoPos(long dwLine, long dwCol);	//������� � ��������� ������� (������:�������)			
		void GotoLine(long dwLine);				//������� � ��������� ������
		void SetViewPos(long dwPos, long FirstLineVisible,long FirstColVisible);//��������� ��������� ������� ��������
		void EnsureRangeVisible(long nStartPos, long nEndPos); //�������������� ����� 	���������� �������
		
		//������� ��� ��������� ������
		long GetLineIndentation(long dwLine) ;
		void SetLineIndentation(long dwLine, long dwIndent) ;
		long GetLineIndentPosition(long dwLine) ;
		
		//????
		long GetMarginsWidth();					//����� ������ ������� �����
		bool IsOvrMode();						//���������, ������� �� ����� ������

		//���������
		long GetSelStart();						//������ ���������
		long GetSelEnd();						//����� ���������
		void SelectRange(long dwStart,long dwEnd);//�������� ���������� ��������
		void ReplaceSelection(cstr szNewText);	//�������� ������� ��������� �������
		bool IsSelected();						//���������, ���� �� ��������� ������
		 
		//������� ��������������
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
		
		//�������
		long SetMarker(long dwLine,byte btMarker); //���������� ���������� �������
		void ClearMarker(long dwLine,byte btMarker);
		void ClearMarker(long dwHandle);
		void ClearAllMarkers(byte btMarker);
		long MarkerNext(long dwLine,byte btMarker);
		long MarkerPrev(int dwLine,byte btMarker);
		bool IsMarkered(long dwLine,byte btMarker);
		long GetMarkerLine(long dwHandle); // -1 = not found

		//���������
		void ShowTip(long nPos, cstr szText);
		void HideTip();
		bool IsTipVisible();
		string_smart GetCurTip();

		//��������� ���������
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
	//����� ��� ����������� ����������� ���������
public:
	class CEditorContents
	{
	public:
		//������, ������� ���������� ������ ������ (� ������ ������, � ����� �����, ��������)
		//��� ������ �� ������������ ������ ���� ������
		enum ELookupDirection{ldForward,ldBackward,ldRandom};
	private:
		CharacterRange	 m_CurRange;	//������� ����
		ELookupDirection m_LookupDir;	//����� ������� ����
		long			 m_dwCaretPos;	//������� ���������
		long			 m_dwTextSize;	//����� ����� ������
		CEditorMessenger&m_rMessenger;
		//������ "����", � �������� �������� ���� �����������
		//� �������� ������ ���� ������������ ��������� ������� ����� ��� ������ 
		//� ����������� �� ELookupDirection
		enum {dwBufferSize=512, dwSlopSize=dwBufferSize/8};
		char m_aImage[dwBufferSize+1];
		void GetImage(int dwPosition);	//��������� ������ ��� ldForward ������� ��� 
		//Start=dwPosition-dwSlopSize, End=Start+dwBufferSize
		unsigned char GetChar(long nPos){
			ASSERT(nPos>=0);
			if (!IsInCache(nPos))	//���� ����������, ��������� ���
				GetImage(nPos);	//
			return m_aImage [nPos-m_CurRange.cpMin];
		}
	public:
		CEditorContents (CSourceView & aOwner); 		
		//��������� ���������������� ����������� ������ �� ������
		void SetLookupDirection(ELookupDirection LookupDir){m_LookupDir=LookupDir;}
		//�������������� ����������� ��������� �������
		long DetectCaretPos(){
			return m_dwCaretPos=m_rMessenger.GetCurPos();}
		//��������� ������� ������� ���� �� ���������� �������� 
		//���������� ������������ ������ �������� ����� ��������, ��� ��� ���������������
		//���������� m_dwCaretPos �� �����������. ������ ������� ������ ������������ ��� ��������� 
		//������ � ������� ��������� �������.
		long GetCaretPos(){
			return m_dwCaretPos; }
		//�������������� ������� ������ ������
		long DetectTextSize(){
			return m_dwTextSize=m_rMessenger.GetTextLen();}
		//�����. ����� ����� ������
		long GetTextSize(){
			return IsValid() ? m_dwTextSize: DetectTextSize();}
		//�������� ���������������� ������
		bool IsValid()	 {
			return m_CurRange.cpMin>=0;}
		//���������� ������ ����������������
		void Invalidate(){
			_RPT0(_CRT_WARN,"Invalidating image...\n");
			m_CurRange.cpMax=m_CurRange.cpMin=-1;}

		//���������� ����� (�� �������/������ ������/������ �� �������/����� ������/������)
		string_smart GetWordAt(long dwPos, OUT long & nStart, OUT long & nEnd);

		//���� ����������� ����� ����� �� ��������� �������, ������
		//���� ������ ��� ����� �� �����-�� �����, ��� ������������
		string_smart GetNearestLeftWord(long dwPos,long & nStart, long & nEnd);

		//���������� ����� � ����������� �� �������, �����, �������� ������� ������ � ��������, ��� ������ �����
		string_smart GetExpressionAt(long dwPos,OUT long & nStart, OUT long & nEnd);

		string_smart GetTextRange(long dwStart, long dwEnd);
		//���������, �������� �� ��������� ������ � ������� ���
		inline bool IsInCache(long dwPos){
			return inrange(dwPos,m_CurRange.cpMin,m_CurRange.cpMax-1);
		}
		//�������� ������ ������� �� �������
		inline char operator [](int dwPos){
			return GetChar(dwPos);
		}
	};
	
	//////////////////////////////////////////////////////////////////////
	//����� ��� ����������� ������ �������� ������, ������, �������� � �.�.
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
		bool	Find(bool bFindNext); //bool bFindNext - ����������� ������ ��� �������
		bool	IsFindNextAvailable(); //���� ����� ���������� ��� �������
		void    SetTextToFind(cstr szText);
		bool	Replace();
		bool	GotoLine();
		bool	GotoNextBookmark();
		bool	GotoPrevBookmark();
		bool 	FindInFiles();
		bool	FindDeclaration(const string_smart & strText, const ITEMLOCATION & aMetAt);
	};

	//////////////////////////////////////////////////////////////////////
	//����� ��� ������ � ���������� "Scintilla"
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
		//��� �������� ���������������� ����, ���������� �������������
		list <string_smart> m_lstMostUsedWords;
		typedef list<string_smart>::iterator most_used_it;
		//������ ��������� ���� ����, ������� ���� ���������� ������
		list <string_smart> m_lstKnownWords;
		typedef list<string_smart>::iterator known_it;

		//��������� �������� (�� ������)
		ISourceDocStylerLink * GetDocStyler() const;

		//��������� ��������� � ����������� �� ������������� ��������
		void Tune();
		void SetMargins();		//��� �������� ������� Tune
		void SetViewSettins();	//
		void SetDrawSettings();	//
		void SetReadonlySettings();//
		void SetStyles();

		//������� ��������� ������
		bool BraceFindMatching(int &braceAtCaret, int &braceOpposite);
		void BraceMatch();
		
		//������� �������
		void FoldUpdate(long dwLine, long dwLevelNow, long dwLevelPrev);
		void FoldExpand(long &dwLine, bool doExpand, bool Force, long dwVisLevels=0, long dwLevel=-1);
		void FoldClick(long dwPosition, long dwModifiers);

		//�������� ������������������
		size_t	GetLinePartsInStyle(long line, int style1, int style2, string_smart sv[], size_t len);
		bool	RangeIsAllWhitespace(long start, long end);
		long	IndentOfBlock(long nLine) ;
		void	SetLineIndentation(long nLine, long nIndent);
		IndentationStatus GetIndentState(long nLine);		
		
		void CharAdded(char ch); //���������� ��� ������� ������� � ���������		
		
		//From CCObjectEx
		override void OnStorageDataChanged(IStorageLink::EParam nParameterChanged);
		//From ISourceDocStylerLink::IEventSink
		override void ISourceDocStylerLink::IEventSink::evOnChangeData() {LOG_PROTECT_CODE(Tune());}
	public:
		bool	m_bModified;	//�������� � public ����������	
								//�������� ������� ������ ������ ������������ ���������� 
								//������ ���������� �� ������� ����������� ���������
								//������ ���������� ����� ������� ������� �� �������� � false, ������ � true
								//AfterLoad � AfterSave

		CEditor(CSourceView & aOwner);
		~CEditor();
		
		//�������� ���������������
		cstr GetEditorClassName();
		//���������� �������������
		void Initialize();                          
		//����� ��������� ���������
		void BeforeLoad();
		//����� �������� ���������
		void AfterLoad();
		//����� ���� ���������
		void BeforeSave();
		//����� ���� ���������
		void AfterSave();
		//��������� ���� ������
		void ExpandAll(bool bExpand);
		//���������� true, ���� ��� ����� ��������
		bool IsAllExpanded();
		//����������/����������� ������� ����, ���� ������� �������
		void ExpandCurrent(bool bExpand);
		//���������� true, ���� ������� ���� �������
		bool IsCurExpanded();
		//���������� ����������� ��������� ������
		void FormatSelection();
		//������ �������� ����������� � �������� 
		void TabifySelection(bool bTabify);
		//��������� ��������
		void Uppercase(bool bUpper);
		//����������/�������� ��������
		void IncreaseLineIndent(bool bIncrease);	
		//�������������� ������ �������� ��� �������� ������
		void AutomaticIndentation(char ch);
		//��������� ����
		bool AutoComplete();
		//����������� ��������� ����� �� ������ ��� ��������� ����
		bool AutoTip(string_smart & strLeftPart);

		void OnNotifyMsg(WPARAM wParam, LPARAM lParam);
		void OnCmdMsg(WPARAM wParam, LPARAM lParam);
	};

	//����� - �������, ����������� ����� � ������ ��������� � ������� ��������� ��������� 
	//������ ������������ � �������� ���������. �������� ���� - ���������� ������
	//�������������� ����� ���� ��� ������������� ����������
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
		void Rollback(){	//������� ��������� ������ �������.
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
	breakpoint_vector	m_Breakpoints;	//������ ���� ����� ��������
	long				m_nState;		//��������� �������
	DWORD				m_nTimerID;

	//Printing .....
	//���-�� �������� ����� ������ �������� ��� ������
	typedef vector<long> num_vector;
	typedef num_vector::iterator num_vector_it;
	num_vector	m_lstPrintPages;
	CAutoFont*	pHeaderFont;
	//.........
	
	//��� ������������ �������� ��������� �������
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

	//������� ��������, ���������� �� ISourceViewLink. ��� ���������� 
	//����� ����� �� �� ������������, � ������������ ������������ ���������
	//m_Messenger ��� ��������� ������������������
	
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
