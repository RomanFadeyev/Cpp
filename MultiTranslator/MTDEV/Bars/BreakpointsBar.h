/********************************************************************
	created:	2002/10/08
	created:	8:10:2002   22:50
	filename: 	$(Projects)\sources\mtdev\bars\breakpointsbar.h
	file path:	$(Projects)\sources\mtdev\bars
	file base:	breakpointsbar
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Docked-Bar для точек останова
*********************************************************************/

#pragma once
#include "Bar_B.h" 

/////////////////////////////////////////////////////////////////////////////
// CbarBreakpoints window

class CbarBreakpoints : public Cbar_B
{
public:
	// CScreen window - класс с реализацией вывода на экран
	class CBPView : public CListCtrlEx, public CCObjectEx,
					public IDocumentLink::IEventSink
	{
		//Вспомогательный класс для хранения информации о каждой точке 
		//Было решено не хранить непосредственно объекты BREAKPOINT, что 
		//небезопасно при оперировании, а использовать нейстральный класс
		struct BP_INFO{
			string_std		m_strSource;
			long			m_nLine;
			bool			m_bEnabled;
			
			BP_INFO(cstr szSource, long nLine, bool bEnabled): m_strSource(szSource), m_nLine(nLine),m_bEnabled(bEnabled){
			}
			BP_INFO(const BREAKPOINT & bp): m_strSource(bp.GetModuleName().c_str()), m_nLine(bp.GetLine()), m_bEnabled(bp.IsEnabled()){
			}

			BP_INFO & operator = (const BP_INFO & src){
				m_strSource = src.m_strSource.c_str();
				m_nLine = src.m_nLine;
				m_bEnabled = src.m_bEnabled;
				return *this;
			}
		};

		typedef vector<BP_INFO>	bpinfo_vector;
		typedef bpinfo_vector::iterator bpinfo_v_i;
		
		bpinfo_vector			m_Items;	//Список всех элементов
		CBCGSizingControlBar *	m_pParentBar;
	protected:
		//Сохранение настроек
		override void SaveSettings();
		override void LoadSettings();

		IProjectDocLink * GetActiveProjectIface();
		void Refresh();
		//Выдает информацию о точке останова по номеру строки в списке
		BP_INFO GetBpInfo(long nNumber);
		void	AddBpInfo(BP_INFO & bp);

		void GetEnabledDisabledCount(long & nEnabled, long & nDisabled);
		void SetBreakpoints();
		void GotoBreakpoint(BP_INFO & bpinfo);
		
		void RemoveBreakpoint(BP_INFO & bpinfo);
		void RemoveBreakpoints();
		
		void EnableBreakpoint(BP_INFO & bpinfo, bool bEnable);
		void EnableBreakpoints(bool bEnable);

		void SetColumnsWidth();
	
		//Обработчики глобальных событий
		//from IDocumentLink::IEventSink
		override void IDocumentLink::IEventSink::evOnCloseDocument(evdCloseDocumentInfo &);
		override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &);
		override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo&);

		virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);		
		afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void	OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
		afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);		
		afx_msg void	OnSize(UINT nType, int cx, int cy);
		afx_msg LRESULT OnPostedMessage(WPARAM wParam, LPARAM lParam);		
		afx_msg void	OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void	cmdOnBreakpointsGoto();
		afx_msg void	cmdupOnBreakpointsGoto(CCmdUI *pCmdUI);
		afx_msg void	cmdOnBreakpointsDelete();
		afx_msg void	cmdupOnBreakpointsDelete(CCmdUI *pCmdUI);
		afx_msg void	cmdOnBreakpointsDisableAll();
		afx_msg void	cmdupOnBreakpointsDisableAll(CCmdUI *pCmdUI);
		afx_msg void	cmdOnBreakpointsClearAll();
		afx_msg void	cmdupOnBreakpointsClearAll(CCmdUI *pCmdUI);
		afx_msg void	cmdOnBreakpointsEnableAll();
		afx_msg void	cmdupOnBreakpointsEnableAll(CCmdUI *pCmdUI);
		afx_msg void	cmdOnBreakpointsDisable();
		afx_msg void	cmdupOnBreakpointsDisable(CCmdUI *pCmdUI);
		DECLARE_MESSAGE_MAP()			
	public:
		enum {colModule=0, colLine, colStatus};
		CBPView(CBCGSizingControlBar * pBar);
		virtual ~CBPView();
	};
protected:
	CBPView m_wndView;
	void	DoDelegate();
	//---------------------------------------------------------------------
	//Перекрытие методов абстрактного базового класса
	override cstr	GetBarName(){return barnBreakpoints;};
	override DWORD	GetBarID()	{return ID_VIEW_BREAKPOINTS;};
	override DWORD	GetBarDock(){return CBRS_ALIGN_BOTTOM;};
	override void	CreateWnd(CFrameWnd* pParentWnd);
	//---------------------------------------------------------------------
	//Создание главного окна
	override void OnCreateAppMainWnd();
	override void OnCloseAppMainWnd();		

	//Multiple Instance Link
	override size_t GetObjectID()const	{return IBarLink::idBarBreakpoints;}

    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void cmdOnViewBreakpoints();
	afx_msg void cmdupOnViewBreakpoints(CCmdUI *pCmdUI);
	afx_msg void cmdOnIndependentBar();
	afx_msg void cmdupOnIndependentBar(CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()
public:
    CbarBreakpoints();
    virtual ~CbarBreakpoints();
};

