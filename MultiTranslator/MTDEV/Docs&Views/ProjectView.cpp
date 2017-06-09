/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   12:53
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\ProjectView.cpp
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	ProjectView
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реализация отображения документа проекта (Project)
*********************************************************************/

#include "stdafx.h"
#include "ProjectView.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectView
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CProjectView, CView)

BEGIN_MESSAGE_MAP(CProjectView, CView)
END_MESSAGE_MAP()

//------------------------------------------------------------------------
CProjectView::CProjectView(){
}
//------------------------------------------------------------------------
CProjectView::~CProjectView(){
}
//------------------------------------------------------------------------
BOOL CProjectView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}
//------------------------------------------------------------------------
void CProjectView::OnDraw(CDC* pDC)
{
	//CProjectDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CProjectView diagnostics
#ifdef _DEBUG
void CProjectView::AssertValid() const {
	CView::AssertValid();
}
void CProjectView::Dump(CDumpContext& dc) const {
	CView::Dump(dc);
}
#endif //_DEBUG
