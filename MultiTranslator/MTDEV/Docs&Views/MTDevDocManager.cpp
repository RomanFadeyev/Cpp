/********************************************************************
	created:	2002/02/25
	created:	25:2:2002	12:54
	filename:	$(Projects)\Sources\MTDEv\Docs&Views\MTDevDocManager.cpp
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	MTDevDocManager
	file ext:	cpp
	author:		Fadeyev	R.V.
	
	purpose:	Реализация класса менеджера	документов
*********************************************************************/

#include "stdafx.h"
#include <ModuleNames.h>
#include <Algorithm>
#include <Vector>
#include "FileDialogEx.h"
#include "MTDevDocManager.h"

#include "Resource.h"
#include "BaseStructs.h"
#include "Names.h"

//Создание переходника для объекта
static i_objects::object_thunk<CMTDevDocManager> MTDevDocManager(__MSG_PREFIX__);

//#pragma warning(disable: 4407)// cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CMTDevDocManager, CCmdTarget)
	ON_COMMAND(ID_FILE_SAVE_ALL, cmdOnFileSaveAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALL, cmdupOnFileSaveAll)
	ON_COMMAND(ID_FILE_CLOSE_ALL, cmdOnFileCloseAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_ALL, cmdupOnFileCloseAll)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
///	CMTDevDocManager(IDocMngrLink)
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
CDocument * CMTDevDocManager::GetActiveDocument() const
{
	CFrameWnd * pMainFrame = dynamic_cast<CFrameWnd*> (AfxGetMainWnd());
	ASSERT_RETURN_NULL(pMainFrame);

	CDocument * pDoc = pMainFrame->GetActiveDocument();
	if (!pDoc){
		pMainFrame = pMainFrame->GetActiveFrame();
		if (pMainFrame)
			pDoc = pMainFrame->GetActiveDocument();
	}
	return pDoc;
}
//------------------------------------------------------------------------
CDocManager* CMTDevDocManager::GetObject(){
	return this;
}
//------------------------------------------------------------------------
void CMTDevDocManager::SetActiveProject(IProjectDocLink	* pProjectLink)
{
	static int nRecrsn=0;
	//Если активный	элемент
	if ((pProjectLink==m_pActiveProjectLink) ||	(nRecrsn))
		return;

	nRecrsn++; 
	LOG_PROTECT_CODE_BEGIN
	if (m_pActiveProjectLink)
		m_pActiveProjectLink->SetActive(false);
	LOG_PROTECT_CODE_END
	
	m_pActiveProjectLink=pProjectLink;
	LOG_PROTECT_CODE_BEGIN
	if (m_pActiveProjectLink)
		m_pActiveProjectLink->SetActive(true);
	LOG_PROTECT_CODE_END
	nRecrsn--;
}
//------------------------------------------------------------------------
IProjectDocLink	* CMTDevDocManager::GetActiveProject() const
{
	return m_pActiveProjectLink;
}
//------------------------------------------------------------------------
ISourceDocLink	* CMTDevDocManager::GetActiveSourceDoc() const
{
	return dynamic_cast<ISourceDocLink*> (GetActiveDocument());
}
//------------------------------------------------------------------------
IProjectDocLink* CMTDevDocManager::FindParentProject(cstr szDocPath)
{
	ASSERT_RETURN_NULL(szDocPath);

	IProjectDocLink * pProject=GetActiveProject();
	
	//В активном проекте такого файла нет, ищем дальше
	if ((!pProject) || (!pProject->IsFilePresent(szDocPath))){
		CDocTemplate * pProjectTemplate = FindProjectDocTemplate();
		if (pProjectTemplate){
			POSITION posDoc = pProjectTemplate->GetFirstDocPosition();
			while (posDoc)
			{
				CDocument * pDoc = pProjectTemplate->GetNextDoc(posDoc);
				IProjectDocLink * pDocLink = dynamic_cast<IProjectDocLink*>(pDoc);
				ASSERT(pDocLink!=NULL);	 //Данный класс должен поддерживать данный интерфейс, раз уж он проект
				
				if ((pDocLink) && (pDocLink->IsFilePresent(szDocPath)))
					return pDocLink;
			}
		}
	}
	//В активном проекте есть такой файл, все нормально
	else {
		return pProject;
	}

	return NULL;
}
//------------------------------------------------------------------------
IDocumentLink* CMTDevDocManager::FindDocument(cstr szDocPath)
{
	//Перебираем все документы
	doc_iterator it;
	for	(CDocumentEx * pDoc = GetDocumentFirst(it); pDoc; pDoc=GetDocumentNext(it))
		if (lstrcmpi(static_cast<CDocument*>(pDoc)->GetPathName(),szDocPath)==0)
			return pDoc->GetDefaultInterface();
	return NULL;
}
//------------------------------------------------------------------------
CDocTemplate*  CMTDevDocManager::FindDocTemplate(cstr szExt)
{
	for (POSITION posTmpl = GetFirstDocTemplatePosition();posTmpl;)
	{
		CDocTemplate * pTemplate = GetNextDocTemplate(posTmpl);

		CString strFilterExt;
		if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) && !strFilterExt.IsEmpty())
			if (strFilterExt==szExt)
				return pTemplate;
	}

	return NULL;
}
//------------------------------------------------------------------------
CDocumentEx* CMTDevDocManager::GetDocumentFirst(doc_iterator & it)
{
	POSITION posTmpl = GetFirstDocTemplatePosition(); 
	it.pData1=posTmpl;	//Присваивать надо сразу, т.к. GetNext... меняет указатели
	while (posTmpl)
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(posTmpl);
		POSITION posDoc = pTemplate->GetFirstDocPosition();
		if (posDoc)
		{
			CDocument * pDoc = pTemplate->GetNextDoc(posDoc);
			ASSERT(dynamic_cast<CDocumentEx*>(pDoc)!=NULL);	//Все объекты должны наследоваться от данного класса
			it.pData2=posDoc;
			return dynamic_cast<CDocumentEx*>(pDoc);
		}
	}
	it.pData1=NULL;
	it.pData2=NULL;
	return NULL;
}
//------------------------------------------------------------------------
CDocumentEx* CMTDevDocManager::GetDocumentNext(doc_iterator & it)
{
	LOG_PROTECT_CODE_BEGIN
	POSITION posTmpl = static_cast<POSITION>(it.pData1); 
	if (posTmpl)
	{	
		POSITION posTemp=posTmpl;
		CDocTemplate* pTemplate	= GetNextDocTemplate(posTemp);
		POSITION posDoc = static_cast<POSITION>(it.pData2); 
		do{
			if (posDoc)	{
				CDocument* pDoc	= pTemplate->GetNextDoc(posDoc);
				ASSERT(dynamic_cast<CDocumentEx*>(pDoc)!=NULL);	//Все объекты должны наследоваться от данного класса
				it.pData2=posDoc;
				return dynamic_cast<CDocumentEx*>(pDoc);
			}
			
			//Все документы в шаблоне кончились, берем следующий шаблон
			GetNextDocTemplate(posTmpl);	//Изменение указателя на шаблон
			it.pData1=posTmpl;
			
			if (posTmpl){					//Есть ли следующий шаблон
				posTemp = posTmpl;				//Получение шаблона
				pTemplate = GetNextDocTemplate(posTemp);
				posDoc = pTemplate->GetFirstDocPosition();
			}
		} while (posDoc || posTmpl);
	}
	LOG_PROTECT_CODE_END
	
	it.pData1=NULL;
	it.pData2=NULL;
	return NULL;
}
//------------------------------------------------------------------------
bool CMTDevDocManager::SelectFile(	string_list	 & out_lstFiles,
									cstr in_szInitialDir/*=NULL*/, cstr in_szCaption/*=NULL*/,	
									cstr in_szFilter/*=NULL*/, 
									cstr in_szDefaultExt/* = NULL*/,
									bool in_bOpen/*=true*/,		
									bool in_bUseRegisteredFilters/*=true*/,		
									bool in_bAllowMultipleSelection/*=true*/,	bool in_bFileMustExist/*=true*/,
									bool in_bCreatePrompt/*=true*/)

{
	DWORD dwFlags=	((in_bAllowMultipleSelection)	? OFN_ALLOWMULTISELECT	: 0) |
					((in_bCreatePrompt)				? OFN_CREATEPROMPT		: 0) |
					((in_bFileMustExist)			? OFN_PATHMUSTEXIST		: 0);

	if (!SelectFileDlg(out_lstFiles,dwFlags,in_szInitialDir,in_szCaption,lst_doc_info(),NULL,in_szDefaultExt, in_bOpen,in_bUseRegisteredFilters))
		return false;

	return true;
}
//------------------------------------------------------------------------
bool CMTDevDocManager::OpenDocument(CDocTemplate * pDocTemplate)
{
	//В	случае пустого шаблона генерируем предупрежедение - самхивает на ошибку
	if (!pDocTemplate)
		TRACE("The empty pDocTemlate has been received in CMTDevDocManager::OpenDocument");

	CString	strFileName;
	//Есть шаблон документа	проекта
	if	(DoPromptFileName(strFileName, AFX_IDS_OPENFILE, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,	TRUE, pDocTemplate))
		return (OpenDocumentFile(strFileName)!=NULL);
	else
		return false;
}
//------------------------------------------------------------------------
IDocumentLink* CMTDevDocManager::OpenDocument(cstr szDocPath, cstr szAllowedExts /*= NULL*/, bool bSilentError/*=false*/)
{
	ASSERT_RETURN_NULL(szDocPath);
	IApplicationLink * pAppLink=singleton_cache::GetApplicationLink();
	ASSERT_RETURN_NULL(pAppLink);

	//Если задано ограничение по расширениям - проверяем
	
	if (szAllowedExts && (*szAllowedExts))
		if (!is_file_of_group(szDocPath,szAllowedExts)) 
			return NULL;

	if (bSilentError)
		pAppLink->SupressMessageBoxes(true);
	
	CDocumentEx * pDoc =NULL;
	LOG_PROTECT_CODE_BEGIN
	pDoc = OpenDocumentFile(szDocPath);
	LOG_PROTECT_CODE_END

	if (bSilentError)
		pAppLink->SupressMessageBoxes(false);

	if (pDoc)
		return pDoc->GetDefaultInterface();
	return NULL;
}
//------------------------------------------------------------------------
CDocumentEx* CMTDevDocManager::OpenDocumentFile(LPCTSTR lpszFileName){
	
	CDocument * pDoc = __super::OpenDocumentFile(lpszFileName);
	if (pDoc){
		ASSERT(dynamic_cast<CDocumentEx*>(pDoc));
		return dynamic_cast<CDocumentEx*>(pDoc);
	}
	return NULL;
}
//------------------------------------------------------------------------
void CMTDevDocManager::OnFileNew()
{
	if (m_templateList.IsEmpty())
	{
		TRACE(traceAppMsg, 0, "Error: no document templates registered with CWinApp.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return;
	}

	LOG_PROTECT_CODE_BEGIN

	CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead();
	if (m_templateList.GetCount() > 1)
	{
		// more than one document template to choose from
		// bring up dialog prompting user

		class CFriendDocTemplate:public CDocTemplate
		{
			friend void CMTDevDocManager::OnFileNew();
		};

		mtdev_lib::INewDocumentPtr spNewDocumentPtr(__uuidof(mtdev_lib::NewDocument));
	
		std::vector<CDocTemplate*> lstDocTypes;
		// fill with document templates in list
		for	(POSITION pos =	GetFirstDocTemplatePosition();pos;)
		{
			CDocTemplate* pTemplate	= GetNextDocTemplate(pos);
		
			HICON hIcon = AfxGetApp()->LoadIcon(((CFriendDocTemplate*)pTemplate)->m_nIDResource);
			if (!hIcon) hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		
			CString strTypeName;
			if (pTemplate->GetDocString(strTypeName, CDocTemplate::fileNewName) && !strTypeName.IsEmpty())
			{
				spNewDocumentPtr->AddDocType(_bstr_t(strTypeName),(OLE_HANDLE)hIcon);
				lstDocTypes.push_back(pTemplate);
			}
		}
		if  (spNewDocumentPtr->Show() && (spNewDocumentPtr->ActiveDocType!=-1))
			pTemplate = lstDocTypes[spNewDocumentPtr->ActiveDocType];
		else
			return;     // none - cancel operation
	}

	ASSERT_RETURN_VOID(pTemplate != NULL);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	pTemplate->OpenDocumentFile(NULL);
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
///	CMTDevDocManager
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4355)
CMTDevDocManager::CMTDevDocManager(): m_pActiveProjectLink(NULL)
{
}
#pragma warning(default:4355)
//------------------------------------------------------------------------
CMTDevDocManager::~CMTDevDocManager()
{

}
//------------------------------------------------------------------------
static DOCUMENT_INFO MakeDocInfo(CDocTemplate* pTemplate)
{
	ASSERT_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate,	pTemplate);
	
	DOCUMENT_INFO doc_info;

	CString	strFilterExt, strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
		!strFilterExt.IsEmpty()	&&
		pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
		!strFilterName.IsEmpty())
	{
		// a file based	document template -	add	to filter list
		ASSERT(strFilterExt[0] == '.');
		doc_info.m_strDocTitle = strFilterName;
		doc_info.m_strDocFilter= string_smart("*",strFilterExt);
	}
	return doc_info;
}
bool comp_doc_info(const DOCUMENT_INFO & info1,const DOCUMENT_INFO & info2){
	return info1.m_strDocTitle<info2.m_strDocTitle;
}
//------------------------------------------------------------------------
bool CMTDevDocManager::SelectFileDlg(string_list & out_lstFiles,DWORD nFlags, 
						cstr szInitialDir, cstr	szCaption, const lst_doc_info & AdditionalDocs,	
						CDocTemplate *pTemplate,cstr szDefExt, bool bOpenMode,	bool bUseRegistered)
{
	ASSERT(!(pTemplate && bUseRegistered));	//Нельзя одновременно сказать "этот	шаблон"	и "шаблоны по умолчанию"
	CFileDialogEx dlgFile(bOpenMode);
	dlgFile.m_ofn.Flags	|= nFlags;

	//---------	Получение фильтров -------------
	lst_doc_info lstDocs;

	//append main filters
	if (pTemplate){
		lstDocs.push_back(MakeDocInfo(pTemplate));
	}
	else if	(bUseRegistered)
	{
		// do for all doc template
		POSITION pos = m_templateList.GetHeadPosition();
		while (pos != NULL)
		{
			CDocTemplate* pTemplate	= (CDocTemplate*)m_templateList.GetNext(pos);
			lstDocs.push_back(MakeDocInfo(pTemplate));
		}
	}

	//append additional filters
	lstDocs.insert(lstDocs.end(),AdditionalDocs.begin(),AdditionalDocs.end());

	//appen aux filters of template
	if	(bUseRegistered)
	{
		lst_doc_info lstAux;
		// do for all doc template
		POSITION pos = m_templateList.GetHeadPosition();
		while (pos != NULL)
		{
			CDocumentTemplateEx* pTemplate	= dynamic_cast<CDocumentTemplateEx*>((CDocTemplate*)m_templateList.GetNext(pos));
			if (pTemplate) {
				lst_doc_info lst = pTemplate->GetAdditionalSupportedDocs();
				lstAux.splice(lstAux.end(), lst,lst.begin(), lst.end());
			}
		}
		vector<DOCUMENT_INFO> x(lstAux.size());
		copy(lstAux.begin(),lstAux.end(),x.begin());
		std::sort(x.begin(),x.end(),comp_doc_info);

		copy(x.begin(),x.end(),inserter(lstDocs,lstDocs.end()));
	}


	// append the "*.*"	all	files filter
	CString s; VERIFY (s.LoadString(AFX_IDS_ALLFILTER));
	lstDocs.push_back(DOCUMENT_INFO(s,"*.*"));

	string_smart strDefExt;
	CString strFilter;
	long	nDefFilter=1;	//Default index (1 based number)
	long	nCount=0;
	//assemble data
	for (lst_doc_info::const_iterator i = lstDocs.begin(); i!=lstDocs.end();++i,++nCount)
	{
		strFilter += (*i).m_strDocTitle.c_str();
		strFilter += (TCHAR)'\0';	// next	string please
		strFilter += (*i).m_strDocFilter.c_str();
		dlgFile.m_ofn.nMaxCustFilter++;
		strFilter += '\0';	// last	string
		if (szDefExt!=NULL){
			cstr szFilter=(*i).m_strDocFilter.c_str();
			if (*szFilter=='*')
				szFilter++;
			if (_tcsicmp(szFilter,szDefExt)==0)
				nDefFilter=nCount+1;	
		}
	}
	strFilter += '\0';	// last	string
	
	if (lstDocs.size()>0){
		strDefExt = lstDocs.front().m_strDocFilter.c_str()+2; //skip *.
		dlgFile.m_ofn.lpstrDefExt	= strDefExt.buffer();
		dlgFile.m_ofn.nFilterIndex = nDefFilter;
	}

	//---------	Заполнение структуры------------
	string_smart strResult;	
	strResult.reserve(_MAX_PATH);
	strResult.buffer()[0]='\0';	

	//Выбираем каталог, в котором надо открыть диалог
	string_smart strInitialDir = GetInitialDir(szInitialDir);
	dlgFile.m_ofn.lpstrInitialDir =	strInitialDir.c_str() ;
	dlgFile.m_ofn.lpstrFilter	= strFilter;
	dlgFile.m_ofn.lpstrTitle	= szCaption;
	dlgFile.m_ofn.lpstrFile		= strResult.buffer();

	if (dlgFile.DoModal()!=IDOK) 
		return false;

	for	(POSITION pos=dlgFile.GetStartPosition(); pos; )
	{
		CString	S=dlgFile.GetNextPathName(pos);
		if (!S.IsEmpty())
			out_lstFiles.items().add(S);
	}
	
	if (out_lstFiles.items().count()>0)
		m_strRecentlyDir = extract_filedir(out_lstFiles.items()[0]);

	return out_lstFiles.items().count()>0;
}
//------------------------------------------------------------------------
BOOL CMTDevDocManager::DoPromptFileName(CString	&fileName, UINT	nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate	*pTemplate)
{
	string_list	lstFileNames;

	CString	strTitle;
	VERIFY(strTitle.LoadString(nIDSTitle));

	if (!SelectFileDlg(lstFileNames,lFlags,NULL,strTitle,lst_doc_info(),pTemplate,NULL,bOpenFileDialog!=0,pTemplate==NULL))
		return false;
 
	fileName=lstFileNames.c_str();
	return true;
}
//------------------------------------------------------------------------
bool CMTDevDocManager::CloseAllDocuments()
{
	LOG_PROTECT_CODE_BEGIN
	//Сначала - проекты 
	CDocTemplate * pProjectDocTemplate = FindProjectDocTemplate();
	ASSERT(pProjectDocTemplate);
	if (pProjectDocTemplate){
		POSITION posDoc = pProjectDocTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			CDocument * pDoc = pProjectDocTemplate->GetNextDoc(posDoc);
			IProjectDocLink * pDocLink = dynamic_cast<IProjectDocLink*>(pDoc);
			ASSERT(pDocLink!=NULL);	 //Данный класс должен поддерживать данный интерфейс, раз уж он проект
			if (!pDocLink->Close())	 //Закрытие было отменено
				return false; 
		}
	}

	//Потом - все что IDocumentLink
	doc_iterator it;
	for (CDocument * pDoc = GetDocumentFirst(it); pDoc; )
	{
		CDocument *pDocOld=pDoc;
		IDocumentLink * pDocLink = dynamic_cast<IDocumentLink*>(pDoc);
		if (!pDocLink->Close()) //Документ не захотел закрываться
			return false;

		pDoc = GetDocumentFirst(it);
		if (pDoc==pDocOld)
			break; //Защита от зацикливания... На всякий случай..
	}

	//Потом - все остальное
	for	(POSITION pos =	GetFirstDocTemplatePosition();pos;)
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(pos);
		if (pTemplate!=pProjectDocTemplate)
			pTemplate->CloseAllDocuments(false);
	}

	return GetDocumentFirst(it)==NULL;
	LOG_PROTECT_CODE_END
	
	return false;
}
//------------------------------------------------------------------------
void CMTDevDocManager::CloseAllDocuments(BOOL bEndSession)
{
	//Этот метод вызывается стандартным framework
	//Мы его перенаправляем на свой
	CloseAllDocuments();
}
//------------------------------------------------------------------------
void CMTDevDocManager::OnCreateApp()
{
	CWinApp	* pApp=AfxGetApp();
	ASSERT_RETURN_VOID(pApp);
	if (pApp->m_pDocManager==this) return;

	if (pApp->m_pDocManager) {
		CMTDevDocManager * m_pOldManager = (CMTDevDocManager*)pApp->m_pDocManager;

		while (!m_pOldManager->m_templateList.IsEmpty())
			this->AddDocTemplate((CDocTemplate*)m_pOldManager->m_templateList.RemoveHead());
		delete pApp->m_pDocManager;
	}
	pApp->m_pDocManager=this;

	m_strRecentlyDir = pApp->GetProfileString(szProfileDocMngr,szParamLastDir,
		string_smart(ApplicationData::APP_DIR,module_names::szFolderDocumentsDefault).c_str());
}
//------------------------------------------------------------------------
void CMTDevDocManager::OnCreateAppMainWnd()
{
	//Получаем указатель на MainFrame-интерфейс
	IMainFrameLink * pMainFrame=i_objects::GetInterface<IMainFrameLink>();
	ASSERT_RETURN_VOID(pMainFrame);
	pMainFrame->IncludeToCmdMsgMap(this);
}
//------------------------------------------------------------------------
void CMTDevDocManager::OnDestroyApp()
{
	CWinApp	* pApp=AfxGetApp();
	ASSERT_RETURN_VOID(pApp);
	if (pApp->m_pDocManager==this) 
		pApp->m_pDocManager=NULL;
	pApp->WriteProfileString(szProfileDocMngr,szParamLastDir,m_strRecentlyDir.c_str());
}
//------------------------------------------------------------------------
BOOL CMTDevDocManager::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
	BOOL res = __super::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);

	//Если команда не обработалась, вызываем все шаблоны для поиска обработчика
	for	(POSITION pos =	GetFirstDocTemplatePosition(); (!res)&&(pos);)
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(pos);
		res=pTemplate->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
	}

	//Если мы не обработали	команду, но	есть активный проект, отдаем ему
	if ((!res)&&(GetActiveProject()))
		res=GetActiveProject()->GetObject()->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
	return res;
}
//------------------------------------------------------------------------
string_smart CMTDevDocManager::GetInitialDir(string_adapter szDefInitialDir/*=""*/)
{
	string_smart strInitialDir=szDefInitialDir;

	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT(pStorage);

	//Если указано, что открывать надо там, где текущий документ
	if (pStorage && pStorage->General_OpenFileUsingActiveDocDir){
		CDocument * pDoc = GetActiveDocument();
		if (pDoc) {
			string_smart s = extract_filedir(pDoc->GetPathName());
			if (is_dir_exist(s))
				strInitialDir=s;
		}
	}

	if (!is_dir_exist(strInitialDir.c_str()))
		strInitialDir.erase();

	//Итак, что за каталог мы получили
	if (strInitialDir.isempty()){
		//Выбираем каталог по умолчанию
		if ((m_strRecentlyDir.isempty()) || (!is_dir_exist(m_strRecentlyDir.c_str())))
			m_strRecentlyDir=string_smart(get_module_filedir().c_str(),module_names::szFolderDocumentsDefault);

		strInitialDir =	m_strRecentlyDir.c_str();
	}

	return strInitialDir;
}
//------------------------------------------------------------------------
CDocTemplate * CMTDevDocManager::FindProjectDocTemplate()
{
	for	(POSITION pos =	GetFirstDocTemplatePosition(); pos;)
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(pos);
		CString	strFilterExt;
		if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) && !strFilterExt.IsEmpty())
			if (is_file_of_group(strFilterExt, EXT_PROJECTS))
				return pTemplate;
	}
	return NULL;
}
//------------------------------------------------------------------------
size_t CMTDevDocManager::GetDocumentsCount()
{
	size_t res=0;
	for	(POSITION posTmpl =	GetFirstDocTemplatePosition(); posTmpl;) 
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(posTmpl);
		for	(POSITION posDoc = pTemplate->GetFirstDocPosition(); posDoc;pTemplate->GetNextDoc(posDoc))
			res++;
	}
	return res;
}
//------------------------------------------------------------------------
void CMTDevDocManager::cmdOnFileSaveAll()
{
	for	(POSITION posTmpl =	GetFirstDocTemplatePosition(); posTmpl;) 
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(posTmpl);
		for	(POSITION posDoc = pTemplate->GetFirstDocPosition(); posDoc;) 
		{
			CDocument* pDoc	= pTemplate->GetNextDoc(posDoc);
			pDoc->DoFileSave();
		}
	}
}
//------------------------------------------------------------------------
void CMTDevDocManager::cmdupOnFileSaveAll(CCmdUI *pCmdUI)
{
	size_t nDocCount=0;
	for	(POSITION posTmpl =	GetFirstDocTemplatePosition(); posTmpl;)
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(posTmpl);
		for	(POSITION posDoc = pTemplate->GetFirstDocPosition(); posDoc;){
			CDocument * pDoc = pTemplate->GetNextDoc(posDoc);
			if (pDoc->IsModified())
				nDocCount++;
		}
	}
	pCmdUI->Enable(nDocCount>0);
}
//------------------------------------------------------------------------
void CMTDevDocManager::cmdOnFileCloseAll()
{
	CloseAllDocuments(false);
}
//------------------------------------------------------------------------
void CMTDevDocManager::cmdupOnFileCloseAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocumentCount());
}