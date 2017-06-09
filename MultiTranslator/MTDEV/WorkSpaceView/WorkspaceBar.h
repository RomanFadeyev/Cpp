/********************************************************************
	created:	2002/05/05
	created:	5:5:2002   21:44
	filename: 	$(Projects)\sources\mtdev\workspaceview\workspacebar.h
	file path:	$(Projects)\sources\mtdev\workspaceview

	file base:	workspacebar
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	WorkSpaceBar c 2 вкладками
*********************************************************************/
#pragma once

#include "Bar_B.h"
#include "Resource.h"
#include "ClassTreeCtrl.h"
#include "FileTreeCtrl.h"

#include "Names.h"
#include "BaseStructs.h"

class CbarWorkspace : public Cbar_B
{
public:
	CbarWorkspace();
	virtual ~CbarWorkspace();

protected:
	CBCGTabWnd	m_wndTabs;

	CProjectTreeClass	m_wndClassView;
	CProjectTreeFile	m_wndFileView;

	void SetFlatTabs (BOOL bFlat = TRUE, BOOL bRepaint = TRUE) {
		m_wndTabs.SetTabBorderSize(FLAT_TABS ? 0 : DEFAULT_TAB_BORDER_SIZE);
		m_wndTabs.SetFlatFrame (bFlat, bRepaint);
	}

	//Создание главного окна
	override void OnCreateAppMainWnd();

	//--------------------------------------------------------------------
	//Перекрытие методов абстрактного базового класса
	override cstr	GetBarName()	{return barnWorkspace;		};
	override DWORD	GetBarID()		{return ID_VIEW_WORKSPACE;	};
	override DWORD	GetBarDock()	{return CBRS_ALIGN_LEFT;	};
	override CSize  GetDefaultSize(){return CSize(200,100);		}
	//--------------------------------------------------------------------

	//Multiple Instance Link
	override size_t GetObjectID()const	{return IBarLink::idBarWorkspace;}

	//From MFC
	override BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
	//{{AFX_MSG(CbarWorkspace)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void cmdupOnViewWorkspace(CCmdUI* pCmdUI);
	afx_msg void cmdOnViewWorkspace();
	afx_msg LRESULT OnChangeActiveTab (WPARAM, LPARAM);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

