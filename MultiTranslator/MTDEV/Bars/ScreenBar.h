#pragma once
#include "Resource.h"
#include "Bar_B.h"
#include "Names.h"
#include "BaseStructs.h"

// CbarScreen

class CbarScreen : public Cbar_B
{ 
public:
	// CScreen window - класс с реализацией вывода на экран
	class COutputScreen : public CEdit, public CCObjectEx, public IOutputLink
	{
		COLORREF	m_clrText,m_clrBkgnd;
		CBrush		m_brBkgnd;
		CFont		m_Font;
		CBCGSizingControlBar *m_pParentBar;
		//--
		bool    m_bIsAutoClose;
		bool    m_bIsAutoClear;
		bool    m_bIsVisible;
		bool    m_bIsOEMConvert;    // OEM to ANSI convert
		bool    m_bIsAltConversion;
	protected:
		//Устанавливаем цвет шрифта
		void SetTextColor(COLORREF clTextColor) {m_clrText=clTextColor; if (m_hWnd) Invalidate(false);}
		//Устанавливаем цвет фона
		void SetBkColor(COLORREF clBkColour)	{m_clrBkgnd=clBkColour; m_brBkgnd.DeleteObject();m_brBkgnd.CreateSolidBrush( m_clrBkgnd );if (m_hWnd) Invalidate(false);};
		//Создание главного окна
		override void OnCreateAppMainWnd()		{
			i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}
		//Сохранение настроек
		override void SaveSettings();
		//Загрузку настроек
		override void LoadSettings();
		//---------------------------------------------------------------------
		//Интерфейс к нашему объекту
		override void StartSession(bool bClear=true);
		override void StopSession(bool bClear=false);
		override void Print(cstr szText);
		override string_smart GetText();			
		override void Clear();

		//Multiple Instance Link
		override size_t GetObjectID()const	{return IOutputLink::idOutputScreen;}
	protected:
		afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void	OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
		afx_msg HBRUSH	CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);			
		afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
		afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);		
		afx_msg void	msgOnActivate();		//Обработчик события, активизации

		afx_msg void cmdOnAltConversion();
		afx_msg void cmdupOnAltConversion(CCmdUI *pCmdUI);
		afx_msg void cmdOnAutoClear();
		afx_msg void cmdupOnAutoClear(CCmdUI *pCmdUI);
		afx_msg void cmdOnAutoClose();
		afx_msg void cmdupOnAutoClose(CCmdUI *pCmdUI);
		afx_msg void cmdOnBackColor();
		afx_msg void cmdOnClear();
		afx_msg void cmdOnOEM();
		afx_msg void cmdupOnOEM(CCmdUI *pCmdUI);
		afx_msg void cmdOnSizeToContent();
		afx_msg void cmdOnTextColor();
		afx_msg void cmdupOnSizeToContent(CCmdUI *pCmdUI);
		afx_msg void cmdOnEditCopy();
		afx_msg void cmdupOnEditCopy(CCmdUI *pCmdUI);
		afx_msg void cmdOnFileSave();
		afx_msg void cmdupOnFileSave(CCmdUI *pCmdUI);
		afx_msg void cmdOnEditSelectAll();
		afx_msg void cmdupOnEditSelectAll(CCmdUI *pCmdUI);
		DECLARE_MESSAGE_MAP()			
	public:
		COutputScreen(CBCGSizingControlBar * pBar);
		virtual ~COutputScreen();
	};

	CbarScreen();
	virtual ~CbarScreen();
	//Работа с TabControl
	bool IsIndepended();
	int	 GetOurPageNum();
	CBCGTabWnd * GetTabControl();
protected:
	COutputScreen m_wndOutputScreen;
	void	DoDelegate();	
	//Создание главного окна
	override void OnCreateAppMainWnd();
	override void OnCloseAppMainWnd();		

	//---------------------------------------------------------------------
	//Перекрытие методов абстрактного базового класса
	override cstr	GetBarName(){return barnScreen;};
	override DWORD	GetBarID()	{return ID_VIEW_SCREEN;};
	override DWORD	GetBarDock(){return CBRS_ALIGN_RIGHT;};
	override CSize  GetDefaultSize(){return CSize(200,100);		}	
	//---------------------------------------------------------------------
	//Интерфейс к нашему объекту
	override void CreateWnd(CFrameWnd* pParentWnd);
	override void InitDock();
	override bool IsVisibleDefault() const;

	//Multiple Instance Link
	override size_t GetObjectID()const	{return IBarLink::idBarScreen;}

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void cmdOnViewScreen();
	afx_msg void cmdupOnUpdateViewScreen(CCmdUI *pCmdUI);
	afx_msg void cmdOnIndependentBar();
	afx_msg void cmdupOnIndependentBar(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
};


