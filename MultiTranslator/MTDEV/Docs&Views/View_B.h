/********************************************************************
	created:	2002/08/18
	created:	18:8:2002   14:07
	filename: 	$(Projects)\sources\mtdev\docs&views\view_b.h
	file path:	$(Projects)\sources\mtdev\docs&views
	file base:	view_b
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Базовый класс для представлений
*********************************************************************/

#pragma once


// CView_B view
#include <ExtControls.h>
class CView_B : public CView, public CFlatControls
{
	//DECLARE_DYNCREATE(CView_B)

protected:
	CView_B();           // protected constructor used by dynamic creation
	virtual ~CView_B();

public:

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcPaint();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


