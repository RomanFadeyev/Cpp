/********************************************************************
	created:	2003/01/26
	created:	26:1:2003   12:47
	filename: 	$(Projects)\sources\mtdev\comapplication.cpp
	file path:	$(Projects)\sources\mtdev\
	file base:	comapplication
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реалиация COM-интерфейса к объекту приложения

	Примеч: Глюк компилятора - все исходники, которые используют заголовки MIDL, 
	должны лежать в одной общей директории
*********************************************************************/


#include "stdafx.h"
#include <BaseUtils.h>

#include "COMApplication.h"
#include "Names.h"
#include "BaseStructs.h"

#include "MTDev.h"

using namespace COM;
//------------------------------------------------------------------------
// Application
//------------------------------------------------------------------------
void Application::LinkToOwner(CMTDevApp *pOwner)
{
	_ASSERT(pOwner!=NULL);

	m_pOwner = pOwner;
	m_pOwner->m_pCoClass=this;
}
//------------------------------------------------------------------------
HRESULT	Application::FinalConstruct()
{
	_ASSERT(m_pOwner==NULL);
	CMTDevApp* pApp =dynamic_cast<CMTDevApp*>(AfxGetApp());
	ASSERT_RETURN_E_FAIL(pApp);

	LinkToOwner(pApp);
	return S_OK;
}
//------------------------------------------------------------------------
void Application::FinalRelease()
{
	_ASSERT(m_pOwner!=NULL);
	m_pOwner->m_pCoClass = NULL;
	m_pOwner=NULL; 
}
//------------------------------------------------------------------------
STDMETHODIMP Application::Quit (VARIANT_BOOL SaveChanges)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);
	
	if (SaveChanges)
		m_pOwner->SaveAllModified();

	AfxPostQuitMessage(0);	//Посылка предварительного PostQuit позволяет избежать всех MessageBox'ов
	return  (m_pOwner->Quit()) ? S_OK : E_FAIL;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::get_Visible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(AfxGetMainWnd());
	
	*pVal = AfxGetMainWnd()->IsWindowVisible();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(AfxGetMainWnd());

	AfxGetMainWnd()->ShowWindow((newVal) ?SW_SHOW : SW_HIDE);
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::ScreenRefresh(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(AfxGetMainWnd());

	AfxGetMainWnd()->UpdateWindow();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::Resize(LONG Width, LONG Height)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(AfxGetMainWnd());
	
	AfxGetMainWnd()->SetWindowPos(NULL,0,0,Width,Height,SWP_NOACTIVATE | SWP_NOMOVE |SWP_NOZORDER);
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::Activate(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(AfxGetMainWnd());

	AfxGetMainWnd()->SetForegroundWindow();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::get_ProjectCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	
	*pVal=0;
	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_E_FAIL(pDocMngrLink);

	CDocTemplate * pProjectDocTemplate = pDocMngrLink->FindDocTemplate(EXT_PROJECTS);
	ASSERT_RETURN_E_FAIL(pProjectDocTemplate);

	for	(POSITION posDoc = pProjectDocTemplate->GetFirstDocPosition(); posDoc;pProjectDocTemplate->GetNextDoc(posDoc)){
		(*pVal)++;
	}
	
	return S_OK;	
}
//------------------------------------------------------------------------
STDMETHODIMP Application::get_Projects(LONG nIndex, IProjectDocument** pDoc)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_E_FAIL(pDocMngrLink);

	CDocTemplate * pProjectDocTemplate = pDocMngrLink->FindDocTemplate(EXT_PROJECTS);
	ASSERT_RETURN_E_FAIL(pProjectDocTemplate);

	size_t i=0;
	for	(POSITION posDoc = pProjectDocTemplate->GetFirstDocPosition(); posDoc;i++){
		CDocument* pDocObj	= pProjectDocTemplate->GetNextDoc(posDoc);
		if 	(i==nIndex){
			IProjectDocLink * pProjectLink = dynamic_cast<IProjectDocLink*>(pDocObj) ;
			ASSERT_RETURN_E_FAIL(pProjectLink);
			*pDoc = pProjectLink->GetComInterface2().detach();
			return S_OK;
		}
		
	}

	return E_INVALIDARG;	
}
//------------------------------------------------------------------------
STDMETHODIMP Application::get_ActiveProject(IProjectDocument** pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	
	*pVal=NULL;
	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_E_FAIL(pDocMngrLink);

	IProjectDocLink * pProjectDoc = pDocMngrLink->GetActiveProject();
	*pVal = pProjectDoc->GetComInterface2().detach();
	
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::get_MRUDocument(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t strVal("");
	
	LOG_PROTECT_CODE_BEGIN
	if ((m_pOwner->m_pRecentProjectList) && 
		(m_pOwner->m_pRecentProjectList->GetSize()>0)) 
		strVal=(*m_pOwner->m_pRecentProjectList)[0];
	LOG_PROTECT_CODE_END

	*pVal=strVal; strVal.Detach();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::FindDocument(BSTR Path,IDocument** pDoc)
{	
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*pDoc=NULL;	
	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_E_FAIL(pDocMngrLink);

	IDocumentLink * pDocLink = pDocMngrLink->FindDocument(_bstr_t(Path));
	if (pDocLink==NULL)
		return S_OK; //Вернется NULL

	//От полученного внутреннего интерфейса добиваемся COM-интерфейса
	*pDoc = pDocLink->GetComInterface().detach();
	ASSERT_RETURN_E_FAIL(*pDoc);

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::OpenDocument(BSTR Path,IDocument** pDoc)
{	
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*pDoc=NULL;
	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_E_FAIL(pDocMngrLink);

	IDocumentLink * pDocLink = pDocMngrLink->OpenDocument(CString(Path),NULL,true);
	if (pDocLink==NULL)
		return S_OK; //Вернется NIL

	//От полученного внутреннего интерфейса добиваемся COM-интерфейса
	*pDoc = pDocLink->GetComInterface().detach();
	ASSERT_RETURN_E_FAIL(*pDoc);

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::CreateProject(BSTR Path,IProjectDocument** pDoc)
{	
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*pDoc=NULL;
	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_E_FAIL(pDocMngrLink);

	string_smart strProjectPath = Path;
	if (extract_fileext(strProjectPath).isempty())
		strProjectPath.cat(EXT_PROJECTS);

	//Если получилось, нужно убедиться, что он - именно проект
	IProjectDocLink * pProjectLink = dynamic_cast<IProjectDocLink*> (pDocMngrLink->FindDocTemplate(EXT_PROJECTS)->CreateNewDocument());
	if (!pProjectLink)
		return S_OK;	//pDoc = NULL
	
	if (!pProjectLink->SaveAs(strProjectPath.c_str())){
		pProjectLink->Close();
		return S_OK;
	}

	//От полученного внутреннего интерфейса добиваемся COM-интерфейса
	*pDoc = pProjectLink->GetComInterface2().detach();
	ASSERT_RETURN_E_FAIL(*pDoc);

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP Application::get_Parser(IParser** pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	IParserLink * pParserLink = singleton_cache::GetParserLink();
	ASSERT_RETURN_E_FAIL(pParserLink);
	
	*pVal = pParserLink->GetComInterface().detach();
	ASSERT_RETURN_E_FAIL(*pVal);

	return S_OK;
}
