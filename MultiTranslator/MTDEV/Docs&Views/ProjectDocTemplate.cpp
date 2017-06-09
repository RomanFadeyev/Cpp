/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   13:14
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\ProjectDocTemplate.cpp
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	ProjectDocTemplate
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реализация класса шаблона документа проекта. Унаследован от 
				стандартного CMultiDocTemplate и слегк изменяет его поведение
*********************************************************************/

#include "stdafx.h"
#include <BaseUtils.h>

#include "BaseStructs.h"
#include "Names.h"
#include "Resource.h"

#include "ProjectDocTemplate.h"
#include "ProjectDoc.h"
#include "ProjectView.h"

//////////////////////////////////////////////////////////////////////
// CProjectDocTemplate::CRegistrator class
//////////////////////////////////////////////////////////////////////

static CProjectDocTemplate::CRegistrator DocTemplateRegistrator;

void CProjectDocTemplate::CRegistrator::OnCreateApp(){
	AfxGetApp()->AddDocTemplate(new	CProjectDocTemplate(
		IDR_DOC_PROJECT,
		RUNTIME_CLASS(CProjectDoc),		// document class
		RUNTIME_CLASS(CMDIChildWnd),	// frame class
		RUNTIME_CLASS(CProjectView)		// view	class
	));
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CProjectDocTemplate class
//////////////////////////////////////////////////////////////////////

CProjectDocTemplate::CProjectDocTemplate ( UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass ):
    CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)

{
}

CProjectDocTemplate::~CProjectDocTemplate()
{

}

CFrameWnd*	CProjectDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
	//Заглушка для не MDI-окна
	return new CFrameWnd();
}

void CProjectDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,BOOL bMakeVisible)
{
	//__super :: InitialUpdateFrame(pFrame, pDoc, false);
	delete pFrame;	//Нам больше не нужен фрейм. Он нам и раньше не был нужен но MFC требовала
}

void CProjectDocTemplate::CloseAllDocuments( BOOL bEndSession )
{
    CMultiDocTemplate::CloseAllDocuments(bEndSession);
}

CDocTemplate::Confidence CProjectDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument *&rpDocMatch)
{
	Confidence R= __super :: MatchDocType(lpszPathName,rpDocMatch);
	if (R==yesAttemptForeign)
		R=noAttempt;
	return R;
}

BEGIN_MESSAGE_MAP(CProjectDocTemplate, CMultiDocTemplate)
	ON_COMMAND(ID_FILE_PROJECT_OPEN, OnFileProjectOpen)
END_MESSAGE_MAP()

void CProjectDocTemplate::OnFileProjectOpen()
{ 
	IDocMngrLink * pDocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngrLink);

	pDocMngrLink->OpenDocument(this);
}

