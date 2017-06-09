// IDEProjectDocTemplate.h: interface for the CProjectDocTemplate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDEPROJECTDOCTEMPLATE_H__75CDA3A5_AB88_11D4_9C0A_0000E8D9F1D7__INCLUDED_)
#define AFX_IDEPROJECTDOCTEMPLATE_H__75CDA3A5_AB88_11D4_9C0A_0000E8D9F1D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CObjectEx.h"	//Один из родителей

class CProjectDocTemplate : public CMultiDocTemplate  
{
public:
	//Класс для регистрации "себя" у приложения
	class CRegistrator: public CCObjectEx{
	public:
		CRegistrator(){}
	protected:
		override void OnCreateApp();
	};

    bool TemplateWizard();
    static void OnSaveSource();

    CProjectDocTemplate ( UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass );

    override Confidence MatchDocType(LPCTSTR lpszPathName,CDocument*& rpDocMatch);
    override void       CloseAllDocuments( BOOL bEndSession );
	override void		InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc, BOOL bMakeVisible = TRUE);
	override CFrameWnd*	CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
    override ~CProjectDocTemplate();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileProjectOpen();
};

#endif // !defined(AFX_IDEPROJECTDOCTEMPLATE_H__75CDA3A5_AB88_11D4_9C0A_0000E8D9F1D7__INCLUDED_)
