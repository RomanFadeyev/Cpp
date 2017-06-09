/********************************************************************
	created:	2002/05/04
	created:	4:5:2002   23:47
	filename: 	$(Projects)\sources\mtdev\bars\bar_b.h
	file path:	$(Projects)\sources\mtdev\bars

	file base:	bar_b
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Базовый класс DockedBar с поддержкой CCObject
*********************************************************************/

#pragma once

#include "CobjectEx.h"	//Один из родителей

#include "Names.h"
#include "BaseStructs.h"

class Cbar_B : public CBCGSizingControlBar, public CCObjectEx, public IBarLink 
{
	HWND m_hTabWnd;
	HWND m_hDelegateWnd;
	bool m_bVisible;	
	int		GetDelegatedPageNo() const;
	CWnd*	GetDelegateControl() const {return CWnd::FromHandle(m_hDelegateWnd);}
	CBCGTabWnd*	GetTabWnd()		 const {return DYNAMIC_DOWNCAST(CBCGTabWnd,CWnd::FromHandle(m_hTabWnd));}
protected:
	//Функция-утилита. Изменяет состояние видимости объекта на противоположное. 
	void ToggleVisible();
	//---------------------------------------------------------------------
	virtual cstr	GetBarName()=0;
	virtual DWORD	GetBarDock()=0; //CBRS_ALIGN_TOP, CBRS_ALIGN_LEFT,CBRS_ALIGN_BOTTOM,CBRS_ALIGN_RIGHT
	virtual DWORD	GetBarID()  =0;
	virtual CSize   GetDefaultSize()	{return CSize(100,100);}

	virtual bool	IsVisibleDefault() const;
	virtual bool	IsDelegated() const;
	virtual	bool	Delegate(CWnd* pWnd, CBCGTabWnd * pTabWnd);
	virtual	bool	UnDelegate();
	
	//Наша задача - чтобы данная функция не была виртуальной, поэтому
	//вводим левый параметр
	bool IsVisible(bool b=false) const;
	//Nолько для IsDelegated=true
	void ShowDelegate(bool bShow);
	//---------------------------------------------------------------------
	//Интерфейс к нашему объекту
	override void CreateWnd(CFrameWnd* pParentWnd);
	override void InitDock();
	override void OnShowMainFrame(bool bVisible);
	override CControlBar * GetObject();
	
	afx_msg void msgOnShow();
	afx_msg void msgOnHide();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

	DECLARE_DYNAMIC(Cbar_B)
	DECLARE_MESSAGE_MAP()
public:
	void Show();
	void Hide();

	Cbar_B();
	virtual ~Cbar_B();
};


