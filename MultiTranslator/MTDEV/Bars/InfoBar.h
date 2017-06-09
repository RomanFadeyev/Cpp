/********************************************************************
	created:	2002/05/05
	created:	5:5:2002   10:08
	filename: 	$(Projects)\sources\mtdev\bars\infobar.h
	file path:	$(Projects)\sources\mtdev\bars

	file base:	infobar
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Docked-Bar, всегда появляющийся при запуске
*********************************************************************/
#pragma once

#include <ExtControls.h>
#include "Bar_B.h" 
#include "Resource.h"

#include "Names.h"
#include "BaseStructs.h"

class CbarInfo : public Cbar_B
{
protected:
	//Класс по управлению списком Recently Projects 
	class CRecentlyMngr 
	{
		//------------- Контрол для отображения Recently Project Item-----
		class CRecentlyLabel : public CLabel
		{
		protected:
			override LRESULT WindowProc(UINT message, WPARAM wP, LPARAM lP);
		public:
			CString m_strPath;
			BOOL Create(LPCTSTR lpszText, LPCTSTR lpszPath, CWnd* pParentWnd);
		};
		//----------------------------------------------------------------
	protected:
		CRecentlyLabel	m_aLabels[MAX_MRU_PROJECTS];
		int				m_nLabelsCount;
		CWnd *			m_pParentWnd;
	public:
		CRecentlyMngr():m_nLabelsCount(0),m_pParentWnd(0){}
		virtual ~CRecentlyMngr()		{Clear();}
		
		void FormRecentlyProjectsList();
		CWnd * GetParent()				{return m_pParentWnd;		}
		void Create(CWnd* pParentWnd)	{m_pParentWnd=pParentWnd;	}
		int  GetSize()					{return m_nLabelsCount;		}		
		void Show(int cmd);
		void Invalidate(bool bErase);
		void Move (int left, int top, int w, int h);
		void AddString (LPCTSTR szText, LPCTSTR szPath);
		void Clear();
		int  FindPath(LPCTSTR szText);

		CString& operator[](int nIndex);		
	};
	//---------------------------------------------------------------------
	CLabel				m_RecentlyHeader;
	CRecentlyMngr		m_RecentlyMngr;
	//{{AFX_MSG(CbarInfo)
	afx_msg void OnPaint();
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void cmdOnViewInfo();
	afx_msg void cmdupOnViewInfo(CCmdUI *pCmdUI);
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//---------------------------------------------------------------------
	//Перекрытие методов абстрактного базового класса
	override cstr	GetBarName(){return barnInfo;};
	override DWORD	GetBarID()	{return ID_VIEW_INFOBAR;};
	override DWORD	GetBarDock(){return CBRS_ALIGN_LEFT;};
	//---------------------------------------------------------------------
	//Создание главного окна
	override void OnCreateAppMainWnd()		{i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}
	//Интерфейс к нашему объекту
	override void CreateWnd(CFrameWnd* pParentWnd);
	override void InitDock();

	//Multiple Instance Link
	override size_t GetObjectID()const	{return IBarLink::idBarInfo;}

	CbarInfo();
	virtual ~CbarInfo();
};
