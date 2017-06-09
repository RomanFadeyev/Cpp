/********************************************************************
	created:	2002/08/18
	created:	18:8:2002   14:08
	filename: 	$(Projects)\sources\mtdev\docs&views\view_b.cpp
	file path:	$(Projects)\sources\mtdev\docs&views
	file base:	view_b
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Базовый класс для представлений
*********************************************************************/

#include "stdafx.h"
#include "View_B.h"
#include "Resource.h"


// CView_B

CView_B::CView_B()
{
}

CView_B::~CView_B()
{
}

BEGIN_MESSAGE_MAP(CView_B, CView)
	ON_WM_NCPAINT()
END_MESSAGE_MAP()


//------------------------------------------------------------------------
BOOL CView_B::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_BORDER;
	return __super::PreCreateWindow(cs);
}
//------------------------------------------------------------------------
void CView_B::OnNcPaint()
{
	__super::OnNcPaint();
	DrawBorders(this,true);
}

