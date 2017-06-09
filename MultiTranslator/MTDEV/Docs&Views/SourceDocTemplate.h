// IDESourceDocTemplate.h: interface for the CSourceDocTemplate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDESOURCEDOCTEMPLATE_H__75CDA3A5_AB88_11D4_9C0A_0000E8D9F1D7__INCLUDED_)
#define AFX_IDESOURCEDOCTEMPLATE_H__75CDA3A5_AB88_11D4_9C0A_0000E8D9F1D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseStructs.h"
#include "CObjectEx.h"	//Один из родителей

class CSourceDocTemplate : public CDocumentTemplateEx  
{
public:
	//Класс для регистрации "себя" у приложения
	class CRegistrator: public CCObjectEx{
	public:
		CRegistrator(){}
	protected:
		override void OnCreateApp();
	};
	
	override lst_doc_info GetAdditionalSupportedDocs() const;

    CSourceDocTemplate ( UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass );
    virtual ~CSourceDocTemplate();
protected:
};

#endif // !defined(AFX_IDESOURCEDOCTEMPLATE_H__75CDA3A5_AB88_11D4_9C0A_0000E8D9F1D7__INCLUDED_)
