/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   12:54
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\ProjectView.h
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	ProjectView
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Реализация отображения документа проекта (Project)
*********************************************************************/
#pragma once

//#include "ProjectDoc.h"
class CProjectView : public CView
{
public:
//	CDocument** GetDocument(){
//		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProjectDoc)));
//		return (CProjectDoc*)m_pDocument;
//	}

	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual ~CProjectView();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CProjectView();
	DECLARE_DYNCREATE(CProjectView)
	DECLARE_MESSAGE_MAP()
};
