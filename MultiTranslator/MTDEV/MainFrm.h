// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F685084D_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
#define AFX_MAINFRM_H__F685084D_7AA4_11D3_A74B_0090274409AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CMDIFrameWnd CBCGMDIFrameWnd
#include <list>
#include "CObjectEx.h" //Один из родителей
#include "BaseStructs.h"
#include <NotifyWnd.h>		//Нужно для отображения сообщений

class CMainFrame : public CMDIFrameWnd, public CCObjectEx, public IMainFrameLink
{
	//Класс для инкапсуляции управления MDITabs
	struct CMDITabs{
		BOOL					m_bMDITabs;
		BOOL					m_bMDITabIcons;
		CBCGTabWnd::Location	m_MDITabLocation;
		CMDITabs():m_MDITabLocation(CBCGTabWnd::LOCATION_BOTTOM),m_bMDITabIcons(true),m_bMDITabs(true){}
	};
	
protected:  // control bar embedded members
	struct cache_data{
		CCmdTarget* m_pTarget;
		clock_t		m_nLastUpdate;
		cache_data(CCmdTarget* pTarget,clock_t nLastUpdate):m_pTarget(pTarget),m_nLastUpdate(nLastUpdate) {
		}
		cache_data() : m_pTarget(NULL),m_nLastUpdate(0){
		}
	};
	typedef list<CCmdTarget*>		list_targets;
	typedef map<__int64,cache_data>	map_targets;
	list_targets		m_TargetList;	//Список всех сторонних Bar-объектов 	
	map_targets			m_TargetMap;

	CBCGMenuBar			m_wndMenuBar;
	CStatusBar			m_wndStatusBar;
	CBCGToolBar			m_wndToolBar;
	CBCGToolBar			m_wndToolbarBuild;
	CBCGToolBar			m_wndToolbarEdit;
	CBCGToolBar			m_wndToolbarDebug;	

	CBCGToolBarImages	m_UserImages;

	CFont				m_fontVertCaption;
	
	HWND				m_hwndClickedTabWnd;
	CMDITabs			m_MDITabs;
	CNotifyWnd			m_NotifyWnd;

	DECLARE_DYNAMIC(CMainFrame)
	void LoadCustomState();
	void SaveCustomState();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnShowPopupMenu (CBCGPopupMenu* pMenuPopup);
	virtual void OnDrawMenuLogo (CDC* pDC, CBCGPopupMenu* pMenu, const CRect& rectLogo);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//From CCObjectEx
	override void CMainFrame::OnStorageDataChanged(IStorageLink::EParam nParameterChanged);

	//---------------------------------------------------------------------
	//Интерфейс IMainFrameLink к нашему объекту 
	override bool IncludeToCmdMsgMap(CCmdTarget* pHandler);
	override bool ExcludeFromCmdMsgMap(CCmdTarget* pHandler);

	//Запуск указанной функции с синхронизацией по очереди сообщений
	override void SyncInvoke(functors::IFunctorRemote & func);
	//Позволяет показывать строку подсказки в позиции StatusBar
	override void ShowTip(cstr szTipText);
	//Дать текущее активное окно CView документа
	override CView * GetActiveView() const;
	//Установить набор индикаторов в StatusBar
	override void SetStatusBarIndicators(const UINT* lpIDArray, int nIDCount, bool bInitiallyDisabled);
	//Установить в статусной строке текст, типа "Ready" и проч
	override void SetStatusMessage(string_adapter strText);
	override string_smart GetStatusMessage();
	//{{AFX_MSG(CMainFrame)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void cmdOnViewBuildToolbar();
	afx_msg void cmdupOnViewBuildToolbar(CCmdUI* pCmdUI);
	afx_msg void cmdOnViewDebugToolbar();
	afx_msg void cmdupOnViewDebugToolbar(CCmdUI *pCmdUI);
	afx_msg void cmdOnViewMdiTabs();
	afx_msg void cmdupOnViewMdiTabs(CCmdUI* pCmdUI);
	afx_msg void OnMditabsIcons();
	afx_msg void OnUpdateMditabsIcons(CCmdUI* pCmdUI);
	afx_msg void OnMditabsTop();
	afx_msg void OnUpdateMditabsTop(CCmdUI* pCmdUI);
	afx_msg void OnMditabsBottom();
	afx_msg void OnUpdateMditabsBottom(CCmdUI* pCmdUI);
	afx_msg void cmdOnMditabsClose();
	afx_msg void cmdOnViewEditToolbar();
	afx_msg void cmdupOnViewEditToolbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMditabsClose(CCmdUI* pCmdUI);
	afx_msg LRESULT OnSyncInvoke(WPARAM, LPARAM);
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM);
	afx_msg LRESULT OnHelpCustomizeToolbars(WPARAM wp, LPARAM lp);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	void OnToolsViewUserToolbar (UINT id);
	void OnUpdateToolsViewUserToolbar (CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CMainFrame();
	virtual ~CMainFrame();
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F685084D_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
