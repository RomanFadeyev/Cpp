/********************************************************************
	created:	2004/08/19
	created:	19:8:2004   22:45
	filename: 	x:\multitranslator\sources\mtdev\com\comprojectdocument.cpp
	file path:	x:\multitranslator\sources\mtdev\com
	file base:	comprojectdocument
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Implementation of ProjectDocument
*********************************************************************/

#include "stdafx.h"
#include "COMProjectDocument.h"
#include "ProjectDoc.h"

#include "Names.h"
#include "BaseStructs.h"

// ProjectDocument

//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::SaveAll(VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	m_pOwner->SaveAll();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::Build(VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	IBuilderLink * pBuilderLink	= m_pOwner->GetBuilderInterface();
	ASSERT_RETURN_E_FAIL(pBuilderLink);

	m_bInBuild = true;
	m_bBuildResult=false;
	m_pOwner->SaveAll();
	m_pOwner->PreBuild();

	pBuilderLink->Build(m_pOwner,IBuilderLink::functor_build(this, BuildWaitToRun));
	//Билдер ничего не делает. Значит, он не смог запуститься или уже выполнился

	if (pBuilderLink->IsBusy())
		while ((m_bInBuild) && (pBuilderLink->IsBusy()))
			guiProcessMessages();
	

	*Result = m_bBuildResult;

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::Run(LONG* ExitCode, VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	*ExitCode = 0;
	*Result=false;

	ASSERT_RETURN_E_FAIL(m_pOwner);
	IRunnerLink	* pRunnerLink	= m_pOwner->GetRunnerInterface();
	ASSERT_RETURN_E_FAIL(pRunnerLink);
	
	HANDLE hProcess = pRunnerLink->Run(m_pOwner);
	*Result = hProcess!=NULL;
	if (*Result){
		*Result = (WaitForSingleObject(hProcess,INFINITE)==WAIT_OBJECT_0);
		DWORD x; GetExitCodeProcess(hProcess,&x);
		*ExitCode = x;
	}

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::Clean(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	m_pOwner->Clean();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::AddFile(BSTR FilePath, VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	*Result = m_pOwner->AddFile(_bstr_t(FilePath));
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::RemoveFile(BSTR FilePath, VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	*Result = m_pOwner->RemoveFile(_bstr_t(FilePath));
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::GetFileFirst(BSTR* FileName, VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t	strPath (m_pOwner->GetFileFirst());
	*Result = strPath.length()>0;
	if (*Result){
		*FileName=strPath; strPath.Detach();
	}
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::GetFileNext(BSTR* FileName, VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t	strPath (m_pOwner->GetFileNext());
	*Result = strPath.length()>0;
	if (*Result){
		*FileName=strPath; strPath.Detach();
	}

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::get_Active(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	*pVal = m_pOwner->GetActive();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::put_Active(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	m_pOwner->SetActive(newVal!=FALSE);
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::get_TargetPath(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t	strValue (m_pOwner->GetExePath().c_str());
	*pVal=strValue; strValue.Detach();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::get_TargetDirectory(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t	strValue (m_pOwner->GetOutputDirectoryFull().c_str());
	*pVal=strValue; strValue.Detach();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::get_Notes(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t	strValue (m_pOwner->GetNotes().c_str());
	*pVal=strValue; strValue.Detach();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::put_Notes(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	m_pOwner->SetNotes(_bstr_t(newVal));
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::get_Params(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t	strValue (m_pOwner->GetParams().c_str());
	*pVal=strValue; strValue.Detach();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::put_Params (BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	m_pOwner->SetParams(_bstr_t(newVal));
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::get_Switches (BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	_bstr_t	strValue (m_pOwner->GetSwitches().c_str());
	*pVal=strValue; strValue.Detach();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::put_Switches (BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	m_pOwner->SetSwitches(_bstr_t(newVal));
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::get_LangGens(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	string_list strList; m_pOwner->GetLangGens(strList);
	string_smart s(strList.c_str()); s.replace("\n",";");
	_bstr_t	strValue (s.c_str());
	*pVal=strValue; strValue.Detach();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::ProjectDocument::put_LangGens(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	ASSERT_RETURN_E_FAIL(m_pOwner);

	string_smart ss = (cstr)_bstr_t(newVal); ss.replace(";","\n");
	string_list strList(ss.c_str()); 
	m_pOwner->SetLangGens(strList);

	return S_OK;
}
