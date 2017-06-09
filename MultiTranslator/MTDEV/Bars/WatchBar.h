/********************************************************************
	created:	2002/05/11
	created:	11:5:2002   17:25
	filename: 	$(Projects)\sources\mtdev\bars\watchbar.h
	file path:	$(Projects)\sources\mtdev\bars

	file base:	watchbar
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Docked-Bar для отладчика
*********************************************************************/

#if !defined(AFX_WATCHBAR_H__31A1AF6B_7EB0_11D3_95C6_00A0C9289F1B__INCLUDED_)
#define AFX_WATCHBAR_H__31A1AF6B_7EB0_11D3_95C6_00A0C9289F1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <TreeListCtrl.h>
#include <ExtControls.h>
#include "Bar_B.h" 
#include <ExtControls.h>		//Нужно для отображения сообщений

/////////////////////////////////////////////////////////////////////////////
// CbarWatch window

class CbarWatch : public Cbar_B
{
public:
	class CStackList_B: public CListBoxEx, public CCObjectEx, public IWatchListLink
	{
	private:
		Cbar_B &		m_Owner;
		bool LocateToPos(string_smart strFileName, long nLine, long nPos);
	protected:
		CStackList_B(Cbar_B & Owner): m_Owner(Owner) {}

		Cbar_B & GetOwner() const {return m_Owner;}

		//Создание главного окна
		override void OnCreateAppMainWnd()	{
			i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}

		virtual cstr GetTitle() const =0;

		//Интерфейс к нашему объекту
		//Реализация IWatchListLink
		override void StartSession(bool bClear=true){if (bClear) Clear();}
		override void StopSession(bool bClear=true)	{if (bClear) Clear();}
		override void Clear()						{ResetContent();}
		override size_t GetItemCount()				{return GetCount();}
		override WATCHITEM GetItem(size_t nIndex);
		override void SetItem(size_t nIndex, const WATCHITEM & WatchItem);
		override void SetItems(WATCHITEM::vector_items & WatchItems);

		//from MFC
		override BOOL PreCreateWindow(CREATESTRUCT& cs);

		DECLARE_MESSAGE_MAP()
		afx_msg void msgOnRealign();		//Обработчик события изменения размеров
		afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);		
		afx_msg void cmdupOnEditCopy(CCmdUI *pCmdUI);
		afx_msg void cmdOnEditCopy();
		afx_msg void cmdOnEditClear();
	};

	//-- class -----------------------------------------------------------
	class CCallStack: public CStackList_B
	{
	public:
		CCallStack(Cbar_B & Owner):CStackList_B(Owner){};
		
		override cstr GetTitle() const		{return "Call Stack";}				
		override size_t GetObjectID() const	{return IWatchListLink::idWatchCallStack;}
	};

	//---------------------------------------------------------------------
	// Parse window
	class CParsePanel: public CLabel, public CCObjectEx, public IWatchParsePanel
	{
	private:
		//CParseStack window
		class CParseStack: public CStackList_B
		{
		public:
			CParseStack(Cbar_B & Owner):CStackList_B(Owner){}

			override cstr GetTitle() const		{return "Parse Stack";}				
			override size_t GetObjectID()const	{return IWatchListLink::idWatchParseStack;}
		};

		CParseStack m_wndParseStack;
		CEdit		m_wndTextBehind;
		CEdit		m_wndTextAhead;
	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
	public:
		//from IWatchParsePanel
		override void SetTextBehind(cstr szText){
			m_wndTextBehind.SetWindowText(szText);
			if (szText){
				size_t x = strlen(szText);
				m_wndTextBehind.SetSel(x,x,FALSE);
			}
		}
		override void SetTextAhead(cstr szText) {m_wndTextAhead.SetWindowText(szText);}

		CParsePanel(Cbar_B & Owner):m_wndParseStack(Owner){}

		cstr GetTitle() const {return m_wndParseStack.GetTitle();}				
	};

	//---------------------------------------------------------------------
	// CWatchList_B window
	class CWatchList_B : public CTreeListCtrl, public CCObjectEx, public IWatchListLink
	{
	private:
		CImageList m_imageList;
	protected:
		
		//Создание главного окна
		override void OnCreateAppMainWnd()	{
			i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}
		
		void SetItem(HFROW hStart, const WATCHITEM & WatchItem);
	public:
		CWatchList_B();
		virtual ~CWatchList_B();

		//Получение заголовка
		virtual cstr GetTitle() const =0;

		//Реализация IWatchListLink
		override void StartSession(bool bClear=true)	{if (bClear) Clear();}
		override void StopSession(bool bClear=true)		{if (bClear) Clear();}
		override size_t GetItemCount()					{return GetChildFRowCount(FL_ROOT);}
		override void Clear();
		override WATCHITEM GetItem(size_t nIndex);
		override void SetItem(size_t nIndex, const WATCHITEM & WatchItem);
		override void SetItems(WATCHITEM::vector_items & WatchItems);
			
		DECLARE_MESSAGE_MAP()		
		afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnDestroy();
	};

	//AutoWatch
	class CWatchListAuto: public CWatchList_B{
	public:
		CWatchListAuto(){};
		override cstr GetTitle() const		{return _T("Autos");}	
		override size_t GetObjectID()	const	{return IWatchListLink::idWatchListAuto;}
	};

	//LocalWatch
	class CWatchListLocal: public CWatchList_B{
	public:
		CWatchListLocal(){};
		override cstr GetTitle() const		{return _T("Locals");}	
		override size_t GetObjectID()	const	{return IWatchListLink::idWatchListLocal;}
	};

	//Watch1,2,3,4....
	class CWatchListN: public CWatchList_B{
		static long  m_nWatches;
		long		 m_nWatchNo;
	public:
		CWatchListN():m_nWatchNo(m_nWatches){
		};
		override cstr GetTitle() const			{return Format("Watch%d",m_nWatchNo).c_str();}
		override size_t GetObjectID()	const	{return IWatchListLink::idWatchListN;}
	};
protected:
    CBCGTabWnd  m_wndTabs;

    CWatchListAuto	 m_wndWatchAuto;
    CWatchListLocal  m_wndWatchLocal;
    CWatchListN		 m_wndWatchesN[4];
    CCallStack		 m_wndCallStack;
	CParsePanel		 m_wndParsePanel;
    void SetFlatTabs (BOOL bFlat = TRUE, BOOL bRepaint = TRUE){
		m_wndTabs.SetTabBorderSize(FLAT_TABS ? 0 : DEFAULT_TAB_BORDER_SIZE);
		m_wndTabs.SetFlatFrame (bFlat, bRepaint);
	}


	//---------------------------------------------------------------------
	//Перекрытие методов абстрактного базового класса
	override cstr	GetBarName(){return barnWatch;};
	override DWORD	GetBarID()	{return ID_VIEW_WATCH;};
	override DWORD	GetBarDock(){return CBRS_ALIGN_BOTTOM;};
	//---------------------------------------------------------------------
	override CBCGTabWnd* GetTabWnd(){return &m_wndTabs;}
	
	//Создание главного окна
	override void OnCreateAppMainWnd()		{
		i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}

	//From ILink
	override size_t GetObjectID() const {return IBarLink::idBarWatch;}
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void cmdOnViewWatch();
	afx_msg void cmdupOnViewWatch(CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()

public:
    CbarWatch();
    virtual ~CbarWatch();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHBAR_H__31A1AF6B_7EB0_11D3_95C6_00A0C9289F1B__INCLUDED_)
