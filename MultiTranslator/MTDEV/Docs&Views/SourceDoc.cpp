/********************************************************************
	created:	2002/02/25
	created:	25:2:2002	12:56
	filename:	$(Projects)\Sources\MTDEv\Docs&Views\SourceDoc.cpp
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	SourceDoc
	file ext:	cpp
	author:		Fadeyev	R.V.
	
	purpose:	Реализация документа исходного файла (Source File)
*********************************************************************/

#include "stdafx.h"
#include "Resource.h" 
#include "SourceDoc.h"

#include <MFCUtils.h>
#include <GuiService.h>


//Добавляем	к названию пункта меню название	проекта
#define	MENU_ADD_PROJECT_NAME(pCmdUI)\
	if (pCmdUI->m_pMenu){\
		static string_smart	strOriginal(mfc_utils::CCmdUIEx(pCmdUI).GetMenuItemText());\
		pCmdUI->SetText(string_smart(strOriginal.c_str(),_T(" "),GetName().c_str()).c_str());\
	}

/////////////////////////////////////////////////////////////////////////////
// CSourceDoc

IMPLEMENT_DYNCREATE(CSourceDoc,	CDocumentEx)

BEGIN_MESSAGE_MAP(CSourceDoc, CDocumentEx)
	ON_COMMAND(ID_BUILD_COMPILE_SOURCE,	cmdOnCompile)
	ON_UPDATE_COMMAND_UI(ID_BUILD_COMPILE_SOURCE, cmdupOnCompile)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, cmdupOnFileSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//	CSourceDoc::CSourceDocLink
/////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
ref_cnt_ptr<COM::ISourceDocument> CSourceDoc::GetComInterface2()
{
	if (!m_pCoClass)
	{
		m_pCoClass = new CComObject<COM::SourceDocument>;
		m_pCoClass->LinkToOwner(this);
	}
	return m_pCoClass;
}
//------------------------------------------------------------------------
ISourceViewLink	* CSourceDoc::GetViewLink(){
	if (this==NULL)	return NULL;
	ISourceViewLink	* pLink	= dynamic_cast<ISourceViewLink*> (GetView());
	ASSERT(pLink);
	return pLink;
}
//------------------------------------------------------------------------
string_smart CSourceDoc::GetPath() const
{
	return (LPCTSTR)CDocumentEx::GetPathName();
}
//------------------------------------------------------------------------
string_smart CSourceDoc::GetExt() const
{
	if (m_strPathName.IsEmpty()){
		CString str;
		GetDocTemplate()->GetDocString(str,CDocTemplate::filterExt);
		return (LPCTSTR) str;
	}
	return extract_fileext(m_strPathName);
}
//------------------------------------------------------------------------
string_smart CSourceDoc::GetName() const
{
	return (LPCTSTR)m_strTitle;
}
//------------------------------------------------------------------------
bool CSourceDoc::IsReadOnly() const
{
	return m_bReadOnly;
}
//------------------------------------------------------------------------
bool CSourceDoc::Close()
{
	if (!SaveModified()) //Закрытия	не произошло
		return false;
	OnCloseDocument();
	return true;
}
//------------------------------------------------------------------------
bool CSourceDoc::Save()
{
	return DoFileSave()!=FALSE;
}
//------------------------------------------------------------------------
bool CSourceDoc::SaveAs(cstr szNewFileName)
{
	return DoSave(szNewFileName)!=FALSE;
}
//------------------------------------------------------------------------
clock_t	CSourceDoc::GetTimeOfLastChange(){
	return m_tLastModify;
}



/////////////////////////////////////////////////////////////////////////////
// CSourceDoc 
/////////////////////////////////////////////////////////////////////////////
#pragma	warning(disable:4355)
CSourceDoc::CSourceDoc(): m_pCoClass(NULL), m_tLastModify(0),m_PageMargins(1500,1500,1000,1500)
{
}
#pragma	warning(default:4355)
CSourceDoc::~CSourceDoc()
{
	LOG_PROTECT_CODE_BEGIN
	StopWatchDog();
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
CView *	CSourceDoc::GetView()
{
	return (CView*)m_viewList.GetHead();
}
//------------------------------------------------------------------------
void CSourceDoc::StartWatchDog()
{
	LOG_PROTECT_CODE_BEGIN
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	//Если в Storage написано, что нужно отслеживать изменения (или Storage нет вообще, чего быть не может)
	//То включаем отслеживание
	
	if (!pStorage || pStorage->General_DetectChangesOutside) {
		if (!GetPath().isempty()) //Новый файл не имеет пути
			m_WatchDog.set(GetPathName(),misc_utils::file_watch::call_back_t(this,OnChangeDocumentOutside));	
	}
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceDoc::StopWatchDog()
{
	LOG_PROTECT_CODE_BEGIN
	m_WatchDog.reset();
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceDoc::OnStorageDataChanged(IStorageLink::EParam nParameterChanged)
{
	if (nParameterChanged == IStorageLink::pmGeneral_DetectChangesOutside)
	{
		StopWatchDog();
		StartWatchDog();
	}
}
//------------------------------------------------------------------------
void CSourceDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		GetView()->Serialize(ar);
	}
	else	
	{
		GetView()->Serialize(ar);
	}
}
//------------------------------------------------------------------------
void CSourceDoc::SetModifiedFlag(BOOL bModified/* =	TRUE*/)
{
	bool bJustModified	= IsModified()!=bModified;
	__super::SetModifiedFlag(bModified);

	//Устанавливаем	время последнего изменения
	m_tLastModify=clock();

	//В	процессе загрузки не считается.	Еще	не было	события	открытия
	if ((!bJustModified) ||	(m_nOpState	== opOpening))
		return;
	
	evdChangeDocumentInfo Info(extract_filename(GetPathName()).c_str(),GetPathName(),*this);
	if (bModified)
		Info.in_nChangeType=evdChangeDocumentInfo::changeModify;
	else
		Info.in_nChangeType=evdChangeDocumentInfo::changeSave;

	i_objects::event<ARGS_1(evdChangeDocumentInfo &)>(IDocumentLink::IEventSink::evOnChangeDocument,Info);
}
//------------------------------------------------------------------------
#ifdef _DEBUG
void CSourceDoc::AssertValid() const{
	CDocument::AssertValid();
}
void CSourceDoc::Dump(CDumpContext&	dc)	const{
	CDocument::Dump(dc);
}
#endif //_DEBUG
//------------------------------------------------------------------------
void CSourceDoc::OnChangeDocumentOutside(misc_utils::file_watch &,misc_utils::file_watch::change_type ct)
{
	LOG_PROTECT_CODE_BEGIN

	if (ct!=misc_utils::file_watch::fwcChange)
		return;

	friend class re_read_f;

	//класс функтор для синхроницации и перехода в контекст главного потока
	class re_read_f: public functors::IFunctorRemote
	{
	private:
		CSourceDoc* m_pDoc;
		override void invoke() {
			m_pDoc->SetModifiedFlag(false);
			//Генерируем событие закрытия, пусть все отвяжутся
			evdCloseDocumentInfo Info(extract_filename(m_pDoc->GetPathName()).c_str(),
									  m_pDoc->GetPathName(),*m_pDoc);
			i_objects::event<ARGS_1(evdCloseDocumentInfo &)>(IDocumentLink::IEventSink::evOnCloseDocument,Info);
			m_pDoc->OnOpenDocument(m_pDoc->GetPathName());
		}
	public:
		re_read_f(CSourceDoc * pDoc): m_pDoc(pDoc) {
			i_objects::GetInterface<IMainFrameLink>()->SyncInvoke(*this);}
	};

	if (guiMsgConfirm(0,FormatRes(IDS_FILE_CHANGED_OUTSIDE_s,HRESOURCES,(LPCTSTR)GetPathName()).c_str()))
	{
		//Ничего нельзя делать из чужого потока!!! Переходим в главный поток
		re_read_f f(this); 
	}

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
BOOL CSourceDoc::OnNewDocument()
{
	if (!__super::OnNewDocument())
		return FALSE;
	GetView()->SetWindowText(NULL);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CSourceDoc::OnOpenDocument(LPCTSTR	lpszPathName) 
{
	m_nOpState = opOpening;
	LOG_PROTECT_CODE_BEGIN

	if (!__super::OnOpenDocument(lpszPathName)){
		m_nOpState = opNone;	
		return FALSE;
	}

	m_nOpState = opNone;	

	//Событие открытия
	SetPathName(lpszPathName); //надо чуть раньше запомнить	имя	(оно еще не	запомнено),	чтобы имя воспользоваться

	evdOpenDocumentInfo	Info(extract_filename(GetPathName()).c_str(),GetPathName(),*this);
	i_objects::event<ARGS_1(evdOpenDocumentInfo&)>(IDocumentLink::IEventSink::evOnOpenDocument,Info);
	
	StartWatchDog();	
	return TRUE;
	LOG_PROTECT_CODE_END
	
	m_nOpState = opNone;	
	return FALSE;
}
//------------------------------------------------------------------------
void CSourceDoc::OnCloseDocument() 
{
	StopWatchDog();
	//Событие закрытия 
	evdCloseDocumentInfo Info(extract_filename(GetPathName()).c_str(),GetPathName(),*this);
	i_objects::event<ARGS_1(evdCloseDocumentInfo &)>(IDocumentLink::IEventSink::evOnCloseDocument,Info);
	
	__super::OnCloseDocument();
}
//------------------------------------------------------------------------
BOOL CSourceDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	StopWatchDog();
	
	//Если нужно создавать bak-файлы, то делаем это
	LOG_PROTECT_CODE_BEGIN
	if (singleton_cache::GetStorageLink()->Editor_CreateBackupFiles)
		if (is_file_exist(lpszPathName)){
			//Добавляем к расширению ~
			string_smart strNewExtension = extract_fileext(lpszPathName);
			if (!strNewExtension.isempty() && (strNewExtension[0]=='.'))
				strNewExtension.insert('~',1);
			else
				strNewExtension.insert('~',0);

			string_smart strBackupFileName = change_fileext(lpszPathName,strNewExtension);
			MoveFile(lpszPathName,strBackupFileName.c_str());
		}
	LOG_PROTECT_CODE_END

	BOOL res = __super::OnSaveDocument(lpszPathName);
	StartWatchDog();	
	return res;
}
//------------------------------------------------------------------------
BOOL CSourceDoc::SaveModified()	
{
	if (!IsModified())
		return TRUE;		// ok to continue

	string_smart strText=
		Format(	string_res(IDS_SOURCE_WAS_CHANGED_s,HRESOURCES).c_str(),
		(GetPathName()!="") 
			? extract_filenameext(GetPathName()).c_str()  //Указываем вместе	с расширением. Так понятней
			: (LPCTSTR)GetTitle()); 

	switch (guiMsgYesNoCancel(0,strText.c_str()))
	{
	case IDYES:
		return DoFileSave();
	case IDNO:
		return true;
	case IDCANCEL:
		return false;
	default:
		ASSERT(false);
	}

	return TRUE;	// keep	going
}
//------------------------------------------------------------------------
void CSourceDoc::cmdOnCompile()
{
}
//------------------------------------------------------------------------
void CSourceDoc::cmdupOnCompile(CCmdUI *pCmdUI)
{
	MENU_ADD_PROJECT_NAME(pCmdUI);
}
//------------------------------------------------------------------------
void CSourceDoc::cmdupOnFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bModified);
}
