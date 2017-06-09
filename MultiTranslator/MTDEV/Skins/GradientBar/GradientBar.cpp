// GradientBar.cpp: implementation of the CGradientBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GradientBarSkin.h"
#include "GradientBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CGradientBar, CBCGVisualManagerXP)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGradientBar::CGradientBar()
{
	// TODO: add skin initialization code here
	
}

CGradientBar::~CGradientBar()
{

}

void CGradientBar::OnFillBarBackground (CDC* pDC, CControlBar* pBar,
								CRect rectClient, CRect rectClip,
								BOOL bNCArea)
{
	if (bNCArea || pBar->IsKindOf (RUNTIME_CLASS (CBCGPopupMenuBar)))
	{
		CBCGVisualManagerXP::OnFillBarBackground (pDC, pBar,
									rectClient, rectClip,
									bNCArea);
		return;
	}

	CBCGDrawManager dm (*pDC);
	dm.FillGradient (rectClient,::GetSysColor (COLOR_3DFACE),
								::GetSysColor (COLOR_3DHILIGHT),
								pBar->m_dwStyle & CBRS_ORIENT_HORZ);
}

void CGradientBar::OnDrawBarBorder (CDC* pDC, CControlBar* pBar, CRect& rect)
{
	CBCGVisualManager::OnDrawBarBorder (pDC, pBar, rect);
}
