// MacStyleSkin.h: interface for the CMacStyleSkin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MACSTYLESKIN_H__AE245B2C_F22B_4B8C_93F0_2BF2FC81FFCB__INCLUDED_)
#define AFX_MACSTYLESKIN_H__AE245B2C_F22B_4B8C_93F0_2BF2FC81FFCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMacStyleSkin : public CBCGVisualManager
{
	DECLARE_DYNCREATE(CMacStyleSkin)

public:
	CMacStyleSkin();
	virtual ~CMacStyleSkin();

	virtual void OnFillBarBackground (CDC* pDC, CControlBar* pBar,
									CRect rectClient, CRect rectClip,
									BOOL bNCArea = FALSE);
	virtual void OnDrawBarBorder (CDC* pDC, CControlBar* pBar, CRect& rect);
	virtual void OnDrawMenuBorder (CDC* pDC, CBCGPopupMenu* pMenu, CRect rect);
	virtual void OnDrawBarGripper (CDC* pDC, CRect rectGripper, BOOL bHorz, CControlBar* pBar);
	virtual void OnDrawCaptionButton (CDC* pDC, CBCGSCBButton* pButton, BOOL bHorz, BOOL bMaximized, BOOL bDisabled);
	virtual void OnFillButtonInterior (CDC* pDC,
		CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state);
	virtual void OnDrawButtonBorder (CDC* pDC,
		CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state);
	virtual void OnHighlightMenuItem (CDC *pDC, CBCGToolbarMenuButton* pButton,
		CRect rect, COLORREF& clrText);
	virtual void OnHighlightRarelyUsedMenuItems (CDC* pDC, CRect rectRarelyUsed);
	virtual void OnEraseTabsArea (CDC* pDC, CRect rect, const CBCGTabWnd* pTabWnd);
	virtual void OnDrawTab (CDC* pDC, CRect rectTab,
							int iTab, BOOL bIsActive, const CBCGTabWnd* pTabWnd);

	virtual COLORREF GetToolbarButtonTextColor (CBCGToolbarButton* pButton,
												CBCGVisualManager::BCGBUTTON_STATE state);
	virtual COLORREF GetCaptionBarTextColor (CBCGCaptionBar* pBar);
	virtual CSize GetButtonExtraBorder () const
	{
		return CSize (8, 8);
	}

	virtual BOOL IsLook2000Allowed () const
	{
		return FALSE;
	}

	virtual BOOL OnFillOutlookPageButton (	CBCGButton* pButton,
											CDC* pDC, const CRect& rectClient,
											COLORREF& clrText);

	virtual void OnDrawComboDropButton (CDC* pDC, CRect rect,
										BOOL bDisabled,
										BOOL bIsDropped,
										BOOL bIsHighlighted,
										CBCGToolbarComboBoxButton* pButton);
	virtual void OnDrawComboBorder (CDC* pDC, CRect rect,
										BOOL bDisabled,
										BOOL bIsDropped,
										BOOL bIsHighlighted,
										CBCGToolbarComboBoxButton* pButton);
	CBrush	m_brBarBkgnd;
	CBrush	m_brMenuBkgnd;
	CBrush	m_brMenuHighlighted;

	void FillArea (CDC* pDC, CRect rect, const CWnd* pWnd, BOOL bIsFloating, BOOL bNCArea);
};

#endif // !defined(AFX_MACSTYLESKIN_H__AE245B2C_F22B_4B8C_93F0_2BF2FC81FFCB__INCLUDED_)
