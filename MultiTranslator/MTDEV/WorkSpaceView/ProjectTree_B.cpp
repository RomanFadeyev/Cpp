/********************************************************************
	created:	2002/03/24
	created:	24:3:2002   21:28
	filename: 	$(Projects)\Sources\MTDEv\ProjectTree_B.cpp
	file path:	$(Projects)\Sources\MTDEv

	file base:	ProjectTree_B
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Ѕазовый класс дл€ деревьев, отображающих проект
*********************************************************************/

#include "stdafx.h"

#include "Resource.h"
#include "ProjectTree_B.h"

#include "Names.h"
#include "BaseStructs.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectTree_B
CProjectTree_B::CProjectTree_B()
{
}

CProjectTree_B::~CProjectTree_B()
{
}

BEGIN_MESSAGE_MAP(CProjectTree_B, CTreeCtrlEx)
	//{{AFX_MSG_MAP(CProjectTree_B)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnTvnItemexpanded)
	ON_MESSAGE(CM_POST,OnPostedMessage)		
	ON_COMMAND(ID_PROPERTIES, cmdOnProperties)
	ON_COMMAND(ID_PROJECT_SET_ACTIVE, cmdOnProjectSetActive)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_SET_ACTIVE, cmdupOnProjectSetActive)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
BOOL CProjectTree_B::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=TVS_SHOWSELALWAYS | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;// |TVS_EDITLABELS;
	return __super::PreCreateWindow(cs);
}
//------------------------------------------------------------------------
BOOL CProjectTree_B::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//Ёти команды мы можем обрабатывть только будучи активными
	//¬с€ эта стратеги€ выгл€дит не очень надежно, в будущем нужно 
	//подумать о том, как застаить Document обрабатывать сообщени€ 
	//–аньше других
	if ((::GetFocus()==m_hWnd) && ((nCode == CN_UPDATE_COMMAND_UI) ||  (nCode == CN_COMMAND)))
	{
		CItemInfo * pI;
		if (GetCurItemData(pI)) {
			if (pI->m_pSkelInfo->GetProjectLink()->GetObject()->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
				return true;
		}
	}
	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
//------------------------------------------------------------------------
void CProjectTree_B::DeleteItem(HTREEITEM hItem)
{
    if (!hItem) return;

	CItemInfo * pI=GetItemData(hItem);
	delete pI;

    BeginUpdate();
    for (HTREEITEM child=GetChildItem(hItem);child;child=GetChildItem(hItem))
        DeleteItem (child);
   	EndUpdate();

	__super::DeleteItem(hItem);
}
//------------------------------------------------------------------------
void CProjectTree_B::SetItemBold(HTREEITEM hItem, bool bBold)
{
	SetItemState (hItem, (bBold) ? TVIS_BOLD :0 , TVIS_BOLD);
}
//------------------------------------------------------------------------
CProjectTree_B::CItemInfo * CProjectTree_B::FindItem(HTREEITEM hParentItem, ELevelItems nItemLevel, DWORD nItemType, cstr szFilePath,CSkeletonInfo * pSkelInfo,cstr szItemName, bool bLookInSubFolders)
{
	for (HTREEITEM hItem=GetChildItem(hParentItem);hItem;hItem=GetNextSiblingItem(hItem))
	{
		CItemInfo * pInfo = GetItemData(hItem);
		
		//Ёлементы должны совпадать по имени, по типу, пор имени файла
		if ((pInfo) && 
			(pInfo->IsAppropriate(nItemLevel,nItemType,szFilePath,szItemName,pSkelInfo)))
			return pInfo;

		//ѕросмтатриваем рекурсивно
		if ((bLookInSubFolders) && (ItemHasChildren(hItem))){
			pInfo=FindItem(hItem,nItemLevel,nItemType,szFilePath,pSkelInfo,szItemName,bLookInSubFolders);
			if (pInfo)
				return pInfo;
		}
	}
	
	return NULL;
}
//------------------------------------------------------------------------
void CProjectTree_B::Clear()
{
	for (HTREEITEM hItem=GetRootItem(); hItem; hItem=GetRootItem())
		DeleteItem(hItem);
}
//------------------------------------------------------------------------
void CProjectTree_B::evOnOpenDocument(evdOpenDocumentInfo &Info)
{
	ASSERT(Info.in_strDocumentPath.c_str());
	ASSERT(Info.in_strDocumentName.c_str());
	
	//ћы обрабатываем только событи€ открыти€ проекта. ќстальные пропускаем
	if (Info.in_nDocID!=evdOpenProjectInfo::idProject)
		return;

	//Ёта проверка не дает исключени€
	AddProjectHeader(CAST_DATA(Info.in_DocInterface,IProjectDocLink));
}
//------------------------------------------------------------------------
void CProjectTree_B::evOnCloseDocument(evdCloseDocumentInfo &Info)
{
	ASSERT(Info.in_strDocumentPath.c_str());
	
	//ћы обрабатываем только событи€ открыти€ проекта. ќстальные пропускаем
	if (Info.in_nDocID!=evdCloseProjectInfo::idProject)
		return;

	RemoveProject(Info.in_strDocumentPath.c_str());
}
//------------------------------------------------------------------------
void CProjectTree_B::evOnChangeDocument(evdChangeDocumentInfo	&Info)
{
}
//------------------------------------------------------------------------
LRESULT CProjectTree_B::OnPostedMessage(WPARAM wParam, LPARAM lParam)
{
	//—пец функци€ дл€ обработки дополнительных сообщений
	if (wParam==WM_SETFOCUS) //”становка фокуса на другое окно
		::SetFocus(reinterpret_cast<HWND>(lParam));
		
	return true;
}
//------------------------------------------------------------------------
void CProjectTree_B::cmdOnProjectSetActive() 
{
	CItemInfo * pI;
	if (!GetCurItemData(pI))
		return;
	pI->m_pSkelInfo->GetProjectLink()->SetActive();
}
//------------------------------------------------------------------------
void CProjectTree_B::cmdupOnProjectSetActive(CCmdUI* pCmdUI) {
	CItemInfo * pI;
	if (GetCurItemData(pI))
		pCmdUI->Enable(!pI->m_pSkelInfo->GetProjectLink()->GetActive());
}
//------------------------------------------------------------------------
void CProjectTree_B::cmdOnProperties() 
{
	CItemInfo * pI;
	if (!GetCurItemData(pI))
		return;

	//ќбрабатываем только свойства проекта
	if (pI->m_nLevelType==ilvlProject)
	{
		IProjPropsLink* pProjPropsLink = i_objects::GetInterface<IProjPropsLink>();
		ASSERT_RETURN_VOID(pProjPropsLink);
		
		pProjPropsLink->Run(pI->m_pSkelInfo->GetProjectLink());
	}
	else
		guiMsgAttention(0,string_res(IDS_OBJECT_HAS_NOT_PROPS,HRESOURCES).c_str());	
}

//------------------------------------------------------------------------
void CProjectTree_B::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CItemInfo * pInfo = GetItemData(pNMTreeView->itemNew.hItem);
	if (pInfo){
		SetItemImage(pNMTreeView->itemNew.hItem,
					 pInfo->GetImages().Get(false,pNMTreeView->action==TVE_EXPAND),
					 pInfo->GetImages().Get(true,pNMTreeView->action==TVE_EXPAND));
	}
	*pResult = 0;
}

