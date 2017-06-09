// FileTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include <GUIService.h>

#include "Resource.h"
#include "FileTreeCtrl.h"
#include "Names.h"
#include "BaseStructs.h"

static int Images[][4]= {
	// Unselected.Collapsed UnSelected.Expanded Selected.Collapsed Selected.Expanded
	{3,3,3,3},	//Project Root	
	{0,1,0,1},	//SourceFolder
	{0,1,0,1},	//OtherFolder
	{5,6,5,6}};	//File Unselected.Normal Unselected.Lost .....

#define UC 0
#define UE 1
#define SC 2
#define SE 3

#define ITEM_IMAGE_ROOT 				0
#define ITEM_IMAGE_SRC_FOLDER			1
#define ITEM_IMAGE_OTHER_FOLDER			2
#define ITEM_IMAGE_SOURCE_FILE			3


#define ITEM_ATTR_NOT_PRESENT			1

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeFile:CScan
//------------------------------------------------------------------------
void CProjectTreeFile::CScan::ScanNext()
{
	//Проверка на достоверность дескриптора текущего элемента
	int n1,n2;
	if ((!hCur) || (!m_pWnd->IsItemValid(hCur)))
		hCur=m_pWnd->GetRootItem();
	if (m_pWnd->GetItemImage(hCur,n1,n2))
	{
		CItemInfo * pI=NULL;
		do{
			hCur = m_pWnd->GetNextItemAny(hCur);
			if (!hCur){
				pI=NULL;
				break;
			}
			pI=m_pWnd->GetItemData(hCur);
		} while ((!pI) || (pI->m_nLevelType!=ilvlItem));
		if ((hCur) && (pI))
			ScanCur();
	}
}
//------------------------------------------------------------------------
void CProjectTreeFile::CScan::ScanCur()
{
	if (!hCur) return;
	CItemInfo * pI=m_pWnd->GetItemData(hCur);
	if (!pI) return;
	
	if (!is_file_exist(pI->m_strFilePath.c_str())){
		pI->m_nAttr|=ITEM_ATTR_NOT_PRESENT;
		m_pWnd->SetItemImage(hCur,Images[ITEM_IMAGE_SOURCE_FILE][UE],Images[ITEM_IMAGE_SOURCE_FILE][SE]);
	}
	else{
		pI->m_nAttr&=~ITEM_ATTR_NOT_PRESENT;
		m_pWnd->SetItemImage(hCur,Images[ITEM_IMAGE_SOURCE_FILE][UC],Images[ITEM_IMAGE_SOURCE_FILE][SC]);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeFile
//------------------------------------------------------------------------
#pragma warning(disable:4355)
CProjectTreeFile::CProjectTreeFile(): m_Scaner(this)
{
}
#pragma warning(default:4355)
//------------------------------------------------------------------------
CProjectTreeFile::~CProjectTreeFile()
{
	m_Scaner.End();
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CProjectTreeFile, CProjectTree_B)
	//{{AFX_MSG_MAP(CProjectTreeFile)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDblclk)
	ON_COMMAND(ID_PROPERTIES, cmdOnProperties)
	ON_COMMAND(ID_SOURCE_FILE_REMOVE, cmdOnFileRemove)
	ON_COMMAND(ID_SOURCE_FILE_PHYS_DELETE, cmdOnFilePhysicalDelete)
	ON_COMMAND(ID_SOURCE_FILE_RECREATE, cmdOnFileReCreate)
	ON_UPDATE_COMMAND_UI(ID_SOURCE_FILE_PHYS_DELETE, cmdupOnFilePhysicalDelete)
	ON_UPDATE_COMMAND_UI(ID_SOURCE_FILE_REMOVE, cmdupOnFileRemove)
	ON_UPDATE_COMMAND_UI(ID_SOURCE_FILE_RECREATE, cmdupOnFileReCreate)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

//------------------------------------------------------------------------
HTREEITEM CProjectTreeFile::AddItem(LPCTSTR szName,LPCTSTR szToProject, EPredefItems nType, HTREEITEM hParent, LPCTSTR szFilePath)
{
	//Получаем привязку с проекту
	map_skel::iterator i = m_mapProjSkel.find(szToProject);
	ASSERT(i!=m_mapProjSkel.end());
	CSkeletonInfo *pSkelInfo= &(*i).second;

	//Добавляем 
	HTREEITEM hItem=NULL;
	switch (nType){
	case iidProjectRoot:	
		hItem = InsertItem(szName, Images[ITEM_IMAGE_ROOT][UC], Images[ITEM_IMAGE_ROOT][SC]);
		SetItemData(hItem,(DWORD)new CItemInfo(*this,hItem,CButtonState(Images[ITEM_IMAGE_ROOT]), ilvlProject,nType,szFilePath,pSkelInfo));
		break;
	case iidSourcesFolder:	
		hItem = InsertItem(szName, Images[ITEM_IMAGE_SRC_FOLDER][UC], Images[ITEM_IMAGE_SRC_FOLDER][SC],hParent);
		SetItemData(hItem,(DWORD)new CItemInfo(*this,hItem, CButtonState(Images[ITEM_IMAGE_SRC_FOLDER]), ilvlFolder,nType,NULL,pSkelInfo));
		break;
	case iidOtherFolder:	
		hItem = InsertItem(szName, Images[ITEM_IMAGE_OTHER_FOLDER][UC], Images[ITEM_IMAGE_OTHER_FOLDER][SC],hParent);
		SetItemData(hItem,(DWORD)new CItemInfo(*this,hItem,CButtonState(Images[ITEM_IMAGE_OTHER_FOLDER]), ilvlFolder,nType,NULL,pSkelInfo));
		break;
	case iidFile:			
		hItem = InsertItem(szName, Images[ITEM_IMAGE_SOURCE_FILE][UC], Images[ITEM_IMAGE_SOURCE_FILE][SC],hParent);
		SetItemData(hItem,(DWORD)new CItemInfo(*this,hItem,CButtonState(Images[ITEM_IMAGE_SOURCE_FILE]),ilvlItem,nType,szFilePath,pSkelInfo));
		break;
	}
	return hItem;
}
//------------------------------------------------------------------------
void CProjectTreeFile::Clear()
{
	m_mapProjSkel.clear();

	__super::Clear();
}
//------------------------------------------------------------------------
bool CProjectTreeFile::RemoveProject(LPCTSTR szProjectPath){
	CSkeletonInfo *pSkelInfo=GetProjectHeader(szProjectPath);
	ASSERT(pSkelInfo);
	if (!pSkelInfo)
		return false;

	map_skel::iterator i =m_mapProjSkel.find(szProjectPath);
	ASSERT(i!=m_mapProjSkel.end());

	DeleteItem(pSkelInfo->m_hProjectRoot);
	
	if (i==m_mapProjSkel.end())
		return false;

	m_mapProjSkel.erase(i);
	return true;
}
//------------------------------------------------------------------------
bool CProjectTreeFile::AddProjectHeader(IProjectDocLink & ieProjectLink)
{
	ASSERT(GetProjectHeader(ieProjectLink.GetPath().c_str())==NULL);
	//Записываем в карту пустой CSkeletonInfo, а потом будем заполнять через итератор
	map_skel::iterator i = m_mapProjSkel.insert(map_skel::value_type(ieProjectLink.GetPath().c_str(),CFileTreeSkelInfo(ieProjectLink))).first;
	
	CFileTreeSkelInfo & I=(*i).second;
	
	I.m_hProjectRoot	= AddItem(	DecorateName(I.GetTitle(),ilvlProject,false).c_str(), 
									I.GetPath(), iidProjectRoot, NULL,I.GetPath());

	I.m_hSourcesFolder	= AddItem(	DecorateName(CString(LPCTSTR(IDS_FILE_TREE_SOURCE_FOLDER)),ilvlFolder,false).c_str(),
									I.GetPath(), iidSourcesFolder,I.m_hProjectRoot);
	I.m_hOtherFolder	= AddItem(	DecorateName(CString(LPCTSTR(IDS_FILE_TREE_OTHER_DOC_FOLDER)),ilvlFolder,false).c_str(), 
									I.GetPath(), iidOtherFolder,I.m_hProjectRoot);

	Expand (I.m_hProjectRoot, TVE_EXPAND);

	//Включаем сканирование файлов
	m_Scaner.Begin();

	return true;
}
//------------------------------------------------------------------------
CProjectTreeFile::CSkeletonInfo * CProjectTreeFile::GetProjectHeader(LPCTSTR szProjectPath)
{
	map_skel::iterator i = m_mapProjSkel.find(szProjectPath);
	return  (i==m_mapProjSkel.end())
				? NULL
				: &(*i).second;
}
//------------------------------------------------------------------------
string_smart CProjectTreeFile::DecorateName(const char * szText, ELevelItems nLevel,bool bModified)
{	
	const char *szProjectTitles[2]= {"%s files","%s files*"};
	const char *szFileTitles[2]= {"%s","%s*"};

	string_smart S;
	switch (nLevel)
	{
	case ilvlRoot:		S=szText; break;
	case ilvlProject:	S=Format(szProjectTitles[bModified],szText); break;
	case ilvlFolder:	S=szText; break;	
	case ilvlItem:		S=Format(szFileTitles[bModified],extract_filenameext(szText).c_str()) ; break;
	default:
		S=szText;
	}
	return S;
}
//------------------------------------------------------------------------
bool CProjectTreeFile::AddFileToProject(LPCTSTR szProjectPath, LPCTSTR szFileName)
{
	map_skel::iterator i =m_mapProjSkel.find(szProjectPath);
	ASSERT(i!=m_mapProjSkel.end());
	if (i==m_mapProjSkel.end()) 
		return false;

	CFileTreeSkelInfo & I=(*i).second;
	string_smart szName(extract_filenameext(szFileName));

	EPredefItems nWhere=(I.GetProjectLink()->GetFileType(szFileName)==IProjectDocLink::fileSource)
					?iidSourcesFolder
					:iidOtherFolder;

	HTREEITEM hItem=NULL;
	switch (nWhere)
	{
//	case iidProjectRoot:	hItem=AddItem(szName.c_str(), szProjectPath, iidFile, I.m_hProjectRoot,szFileName);  break;
	case iidSourcesFolder:	hItem=AddItem(szName.c_str(), szProjectPath, iidFile, I.m_hSourcesFolder,szFileName);break;
	case iidOtherFolder:	hItem=AddItem(szName.c_str(), szProjectPath, iidFile, I.m_hOtherFolder,szFileName);  break;
	}
	return hItem!=NULL;
}
//------------------------------------------------------------------------
bool CProjectTreeFile::RemoveFileFromProject(LPCTSTR szProjectPath, LPCTSTR szFileName)
{
	map_skel::iterator i =m_mapProjSkel.find(szProjectPath);
	ASSERT_RETURN_FALSE(i!=m_mapProjSkel.end());
	CFileTreeSkelInfo *pSkelInfo= &(*i).second;

	//Определяем, в какой же папке может находится файл
	EPredefItems nWhere=(pSkelInfo->GetProjectLink()->GetFileType(szFileName)==IProjectDocLink::fileSource)
		?iidSourcesFolder
		:iidOtherFolder;

	//Находим папку
	HTREEITEM hFolderItem = (nWhere==iidSourcesFolder) 
		? pSkelInfo->m_hSourcesFolder
		: pSkelInfo->m_hOtherFolder;

	for (HTREEITEM hItem=GetChildItem(hFolderItem);hItem;hItem=GetNextItem(hItem,TVGN_NEXT)){
		CItemInfo * pI=GetItemData(hItem);
		if ((pI) && (pI->m_nType==iidFile) && (pI->m_strFilePath.cmpi(szFileName))){
			DeleteItem(pI->m_hItem);
			return true;
		}
	};
	return false;
}
//------------------------------------------------------------------------
bool CProjectTreeFile::FindFiles(LPCTSTR szFindFileName, vector_item_info & out_vFounded)
{
	out_vFounded.clear();
	for (HTREEITEM hItem=GetRootItem();hItem;hItem= GetNextItemAny(hItem))
	{
		CItemInfo * pI=GetItemData(hItem);
		if ((pI) && (pI->m_nLevelType==ilvlItem) && 
			(lstrcmpi(pI->m_strFilePath.c_str(),szFindFileName)==0))
			out_vFounded.push_back(pI);
	};
	return out_vFounded.size()>0;
}
//------------------------------------------------------------------------
void CProjectTreeFile::evOnOpenDocument(evdOpenDocumentInfo &Info)
{
	__super::evOnOpenDocument(Info);

	//Мы обрабатываем только события открытия проекта. Остальные пропускаем
	if (Info.in_nDocID!=evdOpenProjectInfo::idProject)
		return;

	//Работаем с информацией, как с информацией о проекте
	IProjectDocLink & Link	= CAST_DATA(Info.in_DocInterface,IProjectDocLink);
	for (cstr szFileName = Link.GetFileFirst();szFileName;szFileName=Link.GetFileNext())
		AddFileToProject(Info.in_strDocumentPath.c_str(),szFileName);
}
//------------------------------------------------------------------------
void CProjectTreeFile::evOnCloseDocument(evdCloseDocumentInfo &Info)
{
	ASSERT(Info.in_strDocumentPath.c_str());

	//Это базовый документ, он же документ исходных текстов (Source || Other)
	if (Info.in_nDocID==evdCloseDocumentInfo::idDocument)
	{
		CProjectTreeFile::vector_item_info vFoundedFiles;
		//Находим все элементы, которые ссылаются на указанный файл, и 
		//убираем у них *
		if (FindFiles(Info.in_strDocumentPath.c_str(),vFoundedFiles)){
			for (CProjectTreeFile::vector_item_info::iterator i = vFoundedFiles.begin();i!=vFoundedFiles.end();++i)
				SetItemText((*i)->m_hItem,DecorateName((*i)->m_strFilePath.c_str(),(*i)->m_nLevelType,false).c_str());
		}
	}

	__super::evOnCloseDocument(Info);
}
//------------------------------------------------------------------------
void CProjectTreeFile::evOnChangeDocument(evdChangeDocumentInfo &Info)
{
	__super::evOnChangeDocument(Info);

	//Это был проект
	if (Info.in_nDocID==evdChangeProjectInfo::idProject)
	{
		//Эта проверка не дает исключения
		ASSERT(dynamic_cast<evdChangeProjectInfo*>(&Info));
		//Работаем с информацией, как с информацией о проекте
		evdChangeProjectInfo &I=dynamic_cast<evdChangeProjectInfo&>(Info);

		CProjectTreeFile::CSkeletonInfo * pSkelInfo = GetProjectHeader(Info.in_strDocumentPath.c_str());
		ASSERT(pSkelInfo);
		if (pSkelInfo)
		{
			switch (I.in_nChangeType) 
			{
			case evdChangeProjectInfo::changeModify: 
			case evdChangeProjectInfo::changeSave:	
				SetItemText(
					pSkelInfo->m_hProjectRoot,DecorateName(
							pSkelInfo->GetTitle(),	
							ilvlProject,
							I.in_nChangeType==evdChangeProjectInfo::changeModify).c_str());
				break;
			case evdChangeProjectInfo::changeAddFile:
				ASSERT(!I.in_strIncludedFile.isempty());
				AddFileToProject(I.in_strDocumentPath.c_str(),I.in_strIncludedFile.c_str());
				break;
			case evdChangeProjectInfo::changeRemoveFile:
				ASSERT(!I.in_strIncludedFile.isempty());
				RemoveFileFromProject(I.in_strDocumentPath.c_str(),I.in_strIncludedFile.c_str());
				break;
			case evdChangeProjectInfo::changeActivate:
				SetItemBold(pSkelInfo->m_hProjectRoot,true);
				break;
			case evdChangeProjectInfo::changeDeactivate:
				SetItemBold(pSkelInfo->m_hProjectRoot,false);
				break;
			}
		}
	}
	//Это базовый документ, он же документ исходных текстов (Source || Other)
	else if (Info.in_nDocID==evdChangeDocumentInfo::idDocument)
	{
		evdChangeDocumentInfo &I = Info;
		CProjectTreeFile::vector_item_info vFoundedFiles;
		switch (I.in_nChangeType) 
		{
		case evdChangeDocumentInfo::changeClose:	
		case evdChangeDocumentInfo::changeModify: 
		case evdChangeDocumentInfo::changeSave:	
			//Находим все элементы, которые ссылаются на указанный файл, и 
			//указываем им *
			if (FindFiles(I.in_strDocumentPath.c_str(),vFoundedFiles)){
				for (CProjectTreeFile::vector_item_info::iterator i = vFoundedFiles.begin();i!=vFoundedFiles.end();++i)
					SetItemText((*i)->m_hItem,
						 DecorateName(
							(*i)->m_strFilePath.c_str(),
							(*i)->m_nLevelType,I.in_nChangeType==evdChangeProjectInfo::changeModify).c_str());
			}
			break;
		break;
		}
	}
}
//------------------------------------------------------------------------
int CProjectTreeFile::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CProjectTree_B::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Load view images:
	m_Images.Create (IDB_FILE_VIEW, 16, 0, RGB (255, 255, 255));
	SetImageList (&m_Images, TVSIL_NORMAL);	
	return 0;
}
//------------------------------------------------------------------------
void CProjectTreeFile::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==m_Scaner.m_TimerID)
		m_Scaner.ScanNext();
	__super::OnTimer(nIDEvent);
}
//------------------------------------------------------------------------
void CProjectTreeFile::OnDestroy() 
{
	m_Scaner.End();
	Clear();
	__super::OnDestroy();
}
//------------------------------------------------------------------------
void CProjectTreeFile::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//Select Item
	if (point != CPoint (-1, -1))
	{
		CPoint ptTree = point; ScreenToClient (&ptTree);
		HTREEITEM hTreeItem = HitTest (ptTree);
		if (hTreeItem != NULL)
			SelectItem (hTreeItem);
	}

	CItemInfo * pI;
	if (!GetCurItemData(pI))
	{
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_FLW_BASE, point, AfxGetMainWnd ());		
		return;
	}
	
	switch (pI->m_nLevelType)
	{
	case ilvlProject:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_FLW_PROJECT, point, AfxGetMainWnd ());
		break;
	case ilvlFolder:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_FLW_SOURCES, point, AfxGetMainWnd ());
		break;
	case ilvlItem:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_FLW_SOURCE, point, AfxGetMainWnd ());
		break;
	}
}
//------------------------------------------------------------------------
void CProjectTreeFile::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	
	HTREEITEM hItem  = pNMTreeView->itemNew.hItem;
	bool	  bExpand= (pNMTreeView->itemNew.state & TVIS_EXPANDED)!=0;
	CItemInfo * pI  = GetItemData(hItem);
	if (!pI) return;

	switch  (pI->m_nLevelType){
	case iidProjectRoot:	
		SetItemImage(hItem,Images[ITEM_IMAGE_ROOT][bExpand], Images[ITEM_IMAGE_ROOT][SC+bExpand]);
		break;
	case iidSourcesFolder:	
		SetItemImage(hItem,Images[ITEM_IMAGE_SRC_FOLDER][bExpand], Images[ITEM_IMAGE_SRC_FOLDER][SC+bExpand]);
		break;
	case iidOtherFolder:	
		SetItemImage(hItem,Images[ITEM_IMAGE_OTHER_FOLDER][bExpand], Images[ITEM_IMAGE_OTHER_FOLDER][SC+bExpand]);
		break;
	case iidFile:			
		SetItemImage(hItem,Images[ITEM_IMAGE_SOURCE_FILE][bExpand], Images[ITEM_IMAGE_SOURCE_FILE][SC+bExpand]);
		break;
	}
}
//------------------------------------------------------------------------
void CProjectTreeFile::OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	
	CItemInfo * pI;
	if (!GetCurItemData(pI) || (pI->m_nLevelType!=ilvlItem))
		return;

	AfxGetApp()->OpenDocumentFile(pI->m_strFilePath.c_str());
	//Дополнительно посылаем сообщения в асинхронном режиме - Были проблемы с DblClick
	//Переводим фокус
	PostMessage(CM_POST,WM_SETFOCUS,reinterpret_cast<LPARAM>(MAIN_HWND()));
}
//------------------------------------------------------------------------
void CProjectTreeFile::cmdOnFileRemove() 
{
	CItemInfo * pI;
	if (!GetCurItemData(pI))
		return;

	pI->m_pSkelInfo->GetProjectLink()->RemoveFile(pI->m_strFilePath.c_str());
}
//------------------------------------------------------------------------
void CProjectTreeFile::cmdOnFilePhysicalDelete() 
{
	CItemInfo * pI;
	if (!GetCurItemData(pI))
		return;
	//Проверка на существование такого же файла
	if (!guiMsgConfirm(0,
			Format(	string_res(IDS_DO_YOU_WANT_DELETE_FILE_s,HRESOURCES).c_str(),
					pI->m_strFilePath.c_str()).c_str()))
		return;
	if (remove(pI->m_strFilePath.c_str())==-1)
		guiMsgAttention(0,
			Format(	string_res(IDS_CANT_DELETE_FILE_s,HRESOURCES).c_str(),
					pI->m_strFilePath.c_str()).c_str());
}
//------------------------------------------------------------------------
void CProjectTreeFile::cmdOnProperties() 
{
	CItemInfo * pI;
	if (!GetCurItemData(pI))
		return;

	switch  (pI->m_nLevelType){
	case ilvlProject:	
		__super::cmdOnProperties();
		break;
	case ilvlFolder:	
		break;
	case ilvlItem:			
		//Файл - исходник
//		if (is_file_of_group(InfoProject.in_Path.c_str(),EXT_SOURCES)){
		
		ISourcePropsLink* pSourcePropsLink = i_objects::GetInterface<ISourcePropsLink>();
		ASSERT_RETURN_VOID(pSourcePropsLink);
		pSourcePropsLink->Run(pI->m_strFilePath.c_str());
		
		break;
	}
}
//------------------------------------------------------------------------
void CProjectTreeFile::cmdOnFileReCreate() 
{
	CItemInfo * pI;
	if (!GetCurItemData(pI))
		return; 
	//Создаем файл заново
	FILE *stream;
	if((stream=fopen( pI->m_strFilePath.c_str(), "w" )) == NULL)
		guiMsgAttention(0,
			Format(	string_res(IDS_CANT_CREATE_FILE_s,HRESOURCES).c_str(),
					pI->m_strFilePath.c_str()).c_str());
	else
		fclose(stream);
	m_Scaner.hCur=GetSelectedItem();
	m_Scaner.ScanCur();
}
//------------------------------------------------------------------------
void CProjectTreeFile::cmdupOnFilePhysicalDelete(CCmdUI* pCmdUI) 
{
	CItemInfo * pI;
	pCmdUI->Enable(GetCurItemData(pI) && (!(pI->m_nAttr & ITEM_ATTR_NOT_PRESENT)));
}
//------------------------------------------------------------------------
void CProjectTreeFile::cmdupOnFileRemove(CCmdUI* pCmdUI) 
{
}
//------------------------------------------------------------------------
void CProjectTreeFile::cmdupOnFileReCreate(CCmdUI* pCmdUI) 
{
	CItemInfo * pI;
	pCmdUI->Enable(GetCurItemData(pI) && ((pI->m_nAttr & ITEM_ATTR_NOT_PRESENT)));
}
