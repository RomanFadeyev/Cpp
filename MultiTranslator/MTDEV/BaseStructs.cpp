/********************************************************************
	created:	2002/03/16
	created:	16:3:2002   15:14
	filename: 	$(Projects)\Sources\MTDEv\BaseStructs.cpp
	file path:	$(Projects)\Sources\MTDEv

	file base:	BaseStructs
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Основные базовые структуры для передачи в глаголах, событиях 
				и т.п. Реализация их методов
*********************************************************************/


#include "stdafx.h"
#include "BaseStructs.h"
#include "Resource.h"

size_t CDocumentEx::m_nCounter =0;

//------------------------------------------------------------------------
BREAKPOINT::BREAKPOINT(IDebugManageLink *pOwner,string_adapter szModuleName): m_pOwner(pOwner),
	m_strModuleName(szModuleName),bEnabled(true), dwLine(0), dwHitCount(0)
{
}
//------------------------------------------------------------------------
BREAKPOINT::BREAKPOINT(IDebugManageLink *pOwner,string_adapter szModuleName,long dwL, bool bE/*=true*/, cstr szC/*=NULL*/, long dwHC/*=0*/): 
		m_pOwner(pOwner),m_strModuleName(szModuleName),bEnabled(bE), dwLine(dwL), dwHitCount(dwHC), strCondition(szC)
{
}

BREAKPOINT::BREAKPOINT(const  BREAKPOINT & src)
{
	operator = (src);
}
//------------------------------------------------------------------------
BREAKPOINT & BREAKPOINT::operator = (const BREAKPOINT & src)
{
	m_pOwner = src.m_pOwner;
	dwHandle = src.dwHandle;
	bEnabled = src.bEnabled;
	dwLine   = src.dwLine;
	dwHitCount = src.dwHitCount;
	m_strModuleName = src.m_strModuleName.c_str();
	strCondition = src.strCondition.c_str();
	return *this;
}
//------------------------------------------------------------------------
void BREAKPOINT::Enable(bool bEnabled)
{
	ASSERT_RETURN_VOID(m_pOwner);
	if (bEnabled==this->bEnabled) return;
	
	this->bEnabled = bEnabled;
	m_pOwner->SetBreakPoint(*this);
}

//////////////////////////////////////////////////////////////////////////
// CStatusProcessMessage
//////////////////////////////////////////////////////////////////////////

CStatusProcessMessage::CStatusProcessMessage (string_adapter strMessageStart)
{
	LOG_PROTECT_CODE_BEGIN
	IMainFrameLink * pMainFrameLink = singleton_cache::GetMainFrameLink();
	if (pMainFrameLink){
		m_strPreviosMessage = pMainFrameLink->GetStatusMessage();
		pMainFrameLink->SetStatusMessage(strMessageStart);
	}
	LOG_PROTECT_CODE_END
}

CStatusProcessMessage::~CStatusProcessMessage()
{
	LOG_PROTECT_CODE_BEGIN
	IMainFrameLink * pMainFrameLink = singleton_cache::GetMainFrameLink();
	if (pMainFrameLink)
		pMainFrameLink->SetStatusMessage(m_strPreviosMessage);
	LOG_PROTECT_CODE_END
}

//////////////////////////////////////////////////////////////////////////
// CDocumentEx
//////////////////////////////////////////////////////////////////////////

BOOL CDocumentEx::DoFileSave()
{
	 _time64(&m_tLastSaveAttempt); //Как бы там ни было, записываем последнюю попытку сохранения

	//Еще раз обновим свойтсво ReadOnly
	m_bReadOnly=is_file_readonly(GetPathName());

	//Если только для чтения, то спрашиваем, что делать
	if  (m_bReadOnly)
	{
		string_cstr strFullPath = GetPathName();
		string_cstr strShortPath = extract_filenameext(strFullPath);

		switch(guiMsgYesNoCancel(0,FormatResCSTR(IDS_FILE_IS_READ_ONLY_s,HRESOURCES,strShortPath.c_str())))
		{
			case IDYES:
				if (!SetFileAttributes(strFullPath,GetFileAttributes(strFullPath) & (~FILE_ATTRIBUTE_READONLY))){
					guiMsgError(0,FormatResCSTR(IDS_CANT_REMOVE_WRITE_PROTECTION_s,HRESOURCES,strShortPath.c_str()));
					return FALSE;
				}
				break;
			case IDNO:
				break;
			case IDCANCEL:
				return FALSE;
		}
	}
	
	if (!__super::DoFileSave())
		return FALSE;
	
	m_bReadOnly=is_file_readonly(GetPathName());
	return TRUE;
}
//------------------------------------------------------------------------
void CDocumentEx::SetTitle(LPCTSTR lpszTitle)
{
	__super::SetTitle(lpszTitle);

	//Проверяем на ReadOnly
	if (m_bReadOnly)
		m_strTitle+=_T(" [Read Only]");
}
//------------------------------------------------------------------------
BOOL CDocumentEx::OnNewDocument()
{
	return __super::OnNewDocument();
}
//------------------------------------------------------------------------
BOOL CDocumentEx::OnOpenDocument(LPCTSTR lpszPathName)
{
	_time64(&m_tLastSaveAttempt); //Считаем, что это есть стартовое время

	string_smart strShortPath = extract_filenameext(lpszPathName);
	CStatusProcessMessage msg(FormatResCSTR(IDS_FILE_LOADING_s,HRESOURCES,strShortPath.c_str()));

	if (!__super::OnOpenDocument(lpszPathName))
		return FALSE;

	//Проверяем на ReadOnly
	m_bReadOnly = is_file_readonly(lpszPathName);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CDocumentEx::OnSaveDocument(LPCTSTR lpszPathName)
{
	IDocumentLink * pExistLink = singleton_cache::GetDocMngrLink()->FindDocument(lpszPathName);
	if ((pExistLink!=NULL) && (pExistLink->GetObject()!=this)){
		guiMsgError(0,FormatRes(IDS_CANT_SAVE_DOC_BECAUSE_SAME_EXISTS_s,HRESOURCES,lpszPathName));
		return FALSE;
	}

	_time64(&m_tLastSaveAttempt); //Как бы там ни было, записываем последнюю попытку сохранения

	string_smart strShortPath = extract_filenameext(lpszPathName);
	CStatusProcessMessage msg(FormatResCSTR(IDS_FILE_SAVING_s,HRESOURCES,strShortPath.c_str()));

	BOOL bRes = __super::OnSaveDocument(lpszPathName);
	_time64(&m_tLastSaveAttempt); //Более точно
	return bRes;
}


//////////////////////////////////////////////////////////////////////////
// singleton_cache
//////////////////////////////////////////////////////////////////////////
namespace singleton_cache
{
	IStorageLink *	gp_StorageLink=NULL;
	IApplicationLink * gp_ApplicationLink=NULL;
	IMainFrameLink * gp_MainFrameLink=NULL;
	IDocMngrLink *	gp_DocMngrLink=NULL;
	IBuilderLink *	gp_BuilderLink=NULL;
	IRunnerLink *	gp_RunnerLink=NULL;
	IParserLink *	gp_ParserLink=NULL;
	ISourceDocStylerLink *	gp_SourceDocStylerLink=NULL;

	IStorageLink *	GetStorageLink(){
		return (gp_StorageLink) ? gp_StorageLink : gp_StorageLink = i_objects::GetInterface<IStorageLink>();
	}
	
	IApplicationLink * GetApplicationLink(){
		return (gp_ApplicationLink) ? gp_ApplicationLink : gp_ApplicationLink = i_objects::GetInterface<IApplicationLink>();
	}

	IMainFrameLink * GetMainFrameLink(){
		return (gp_MainFrameLink) ? gp_MainFrameLink : gp_MainFrameLink = i_objects::GetInterface<IMainFrameLink>();
	}
	
	IDocMngrLink *	GetDocMngrLink(){
		return (gp_DocMngrLink) ? gp_DocMngrLink : gp_DocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	}
	
	IBuilderLink *	GetBuilderLink(){
		return (gp_BuilderLink) ? gp_BuilderLink : gp_BuilderLink = i_objects::GetInterface<IBuilderLink>();
	}
	
	IRunnerLink *	GetRunnerLink(){
		return (gp_RunnerLink) ? gp_RunnerLink : gp_RunnerLink = i_objects::GetInterface<IRunnerLink>();
	}
	
	IParserLink *	GetParserLink(){
		return (gp_ParserLink) ? gp_ParserLink : gp_ParserLink = i_objects::GetInterface<IParserLink>();
	}

	ISourceDocStylerLink *	GetSourceDocStylerLink(){
		return (gp_SourceDocStylerLink) ? gp_SourceDocStylerLink : gp_SourceDocStylerLink = i_objects::GetInterface<ISourceDocStylerLink>();
	}

	//Нам необходимо отследить, когда уйдут объекты и соответсвенно обнулить 
	//указатели на них
	class CObjectsWatch : public i_objects::object
	{
	public:
		override void OnExitAnyObject (object &aObject){
			gp_StorageLink=NULL;
			gp_ApplicationLink=NULL;
			gp_MainFrameLink=NULL;
			gp_DocMngrLink=NULL;
			gp_BuilderLink=NULL;
			gp_RunnerLink=NULL;
			gp_ParserLink=NULL;
			gp_SourceDocStylerLink=NULL;
		}
		~CObjectsWatch(){
			OnExitAnyObject(*this);
		}
	};

	CObjectsWatch gpObjectsWatch;
}
