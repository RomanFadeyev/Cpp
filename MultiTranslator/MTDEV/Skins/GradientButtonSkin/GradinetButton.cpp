// GradinetButton.cpp: implementation of the CGradinetButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GradientButtonSkin.h"
#include "GradinetButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CGradinetButton, CBCGVisualManagerXP)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGradinetButton::CGradinetButton()
{
	// TODO: add skin initialization code here
	
}

CGradinetButton::~CGradinetButton()
{

}

void CGradinetButton::OnDrawButtonBorder (CDC* pDC,
	CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state)
{
	if (!pButton->IsExtraSize () ||
		CBCGToolBar::IsCustomizeMode ())
	{
		CBCGVisualManager::OnDrawButtonBorder (pDC, pButton, rect, state);
		return;
	}

	rect.DeflateRect (1, 1);

	switch (state)
	{
	case ButtonsIsPressed:
		pDC->Draw3dRect (&rect, ::GetSysColor (COLOR_3DDKSHADOW), ::GetSysColor (COLOR_3DHILIGHT));
		return;

	case ButtonsIsHighlighted:
		pDC->Draw3dRect (&rect, ::GetSysColor (COLOR_3DHILIGHT), ::GetSysColor (COLOR_3DDKSHADOW));
		return;
	}
}

void CGradinetButton::OnFillButtonInterior (CDC* pDC,
		CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state)
{
	if (!pButton->IsExtraSize () ||
		CBCGToolBar::IsCustomizeMode () ||
		(pButton->m_nStyle & TBBS_DISABLED))
	{
		CBCGVisualManager::OnFillButtonInterior (pDC, pButton, rect, state);
		return;
	}

	rect.DeflateRect (1, 1);

	CBCGDrawManager dm (*pDC);

	switch (state)
	{
	case ButtonsIsHighlighted:
		dm.FillGradient (rect, 
						::GetSysColor (COLOR_3DLIGHT),
						::GetSysColor (COLOR_WINDOW), 
						FALSE);
		break;

	default:
		dm.FillGradient (rect, 
						::GetSysColor (COLOR_3DHILIGHT),
						::GetSysColor (COLOR_3DFACE), 
						FALSE);
	}
}

void CGradinetButton::OnHighlightMenuItem (CDC *pDC, CBCGToolbarMenuButton* pButton,
		CRect rect, COLORREF& clrText)
{
	ASSERT_VALID (pDC);

	CBCGDrawManager dm (*pDC);
	dm.FillGradient (rect, RGB (0, 119, 158), RGB (141, 207, 244), TRUE);

	clrText = RGB (255, 255, 255);	// White
}
