// GradinetButton.h: interface for the CGradinetButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRADINETBUTTON_H__D25B22DA_500C_49E3_B137_0138263E2588__INCLUDED_)
#define AFX_GRADINETBUTTON_H__D25B22DA_500C_49E3_B137_0138263E2588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGradinetButton : public CBCGVisualManagerXP
{
	DECLARE_DYNCREATE(CGradinetButton)

public:
	CGradinetButton();
	virtual ~CGradinetButton();

	virtual void OnFillButtonInterior (CDC* pDC,
		CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state);
	virtual void OnDrawButtonBorder (CDC* pDC,
		CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state);
	virtual void OnHighlightMenuItem (CDC *pDC, CBCGToolbarMenuButton* pButton,
		CRect rect, COLORREF& clrText);
};

#endif // !defined(AFX_GRADINETBUTTON_H__D25B22DA_500C_49E3_B137_0138263E2588__INCLUDED_)
