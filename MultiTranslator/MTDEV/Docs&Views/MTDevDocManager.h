/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   12:55
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\MTDevDocManager.h
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	MTDevDocManager
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Реализация класса менеджера документов
*********************************************************************/

#pragma once

#ifdef new
#undef new
#endif

#include <BaseUtils.h>
#include "CobjectEx.h"	//Один из родителей
#include "MTDevDialog_B.h"	//Один из родителей
#include "Resource.h"

class CMTDevDocManager : public CCmdTarget, public CDocManager/*MFC*/, public CCObjectEx,public IDocMngrLink
{
protected:
	typedef list<DOCUMENT_INFO> lst_doc_info;

	IProjectDocLink*	m_pActiveProjectLink;	//Интерфейс документа проекта
	string_smart		m_strRecentlyDir;
	// Выдает диалог выбора файла
	bool SelectFileDlg(	string_list & out_lstFiles,	DWORD nFlags, 
						cstr szInitialDir, cstr szCaption, const lst_doc_info & AdditionalDocs, 
						CDocTemplate *pTemplate=NULL, cstr szDefExt=NULL, 
						bool OpenMode =true, bool bUseRegistered=true);

	//Сол-во открытых документов
	size_t GetDocumentsCount();

	//Текущий активный документ
	CDocument * GetActiveDocument() const;

	// From CCObjectEx
	override void OnCreateApp();
	override void OnCreateAppMainWnd();
	override void OnDestroyApp();
	// From CDocManager
	override BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);
	override void CloseAllDocuments(BOOL bEndSession); // close documents before exiting
public:
	//Cause: MFC Multiple inheritance
	void* operator new(size_t nSize)  { return CDocManager::operator new(nSize); }
	void operator delete(void* p)     { CDocManager::operator delete(p); }

	//CCmdTarget
	BOOL  OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	//--------------------------------------------------------------------
	//Интерфейс к нашему объекту (IDocMngrLink)
	override CDocManager* GetObject();

	override void IDocMngrLink::SetActiveProject(IProjectDocLink* pProject);	//Назначает активный проект
	override IProjectDocLink * /*IDocMngrLink::*/GetActiveProject() const;		//Возвращает текущий активный проект	
	override ISourceDocLink* /*IDocMngrLink::*/GetActiveSourceDoc() const;		//Дает активный документ исходника

	override CDocTemplate*   IDocMngrLink::FindDocTemplate(cstr szExt);
	override IDocumentLink * IDocMngrLink::FindDocument(cstr szDocPath);

	override IProjectDocLink* IDocMngrLink::FindParentProject(cstr szDocPath);
	override CDocumentEx*	IDocMngrLink::GetDocumentFirst(doc_iterator & it);
	override CDocumentEx*	IDocMngrLink::GetDocumentNext(doc_iterator & it);

	override bool SelectFile(	string_list	 & out_lstFiles,
								cstr in_szInitialDir=NULL, cstr in_szCaption=NULL,	
								cstr in_szFilter=NULL, 
								cstr in_szDefaultExt = NULL,
								bool in_bOpen=true,		
								bool in_bUseRegisteredFilters=true,		
								bool in_bAllowMultipleSelection=true,	bool in_bFileMustExist=true,
								bool in_bCreatePrompt=true);
	override string_smart GetInitialDir(string_adapter szDefInitialDir="");
	override void SetRecentlyDir(string_adapter strDir) {m_strRecentlyDir=strDir;}

	override bool IDocMngrLink::OpenDocument(CDocTemplate * pDocTemplate);
	override IDocumentLink* IDocMngrLink::OpenDocument(cstr szDocPath, cstr szAllowedExts = NULL, bool bSilentError=false);
	override bool IDocMngrLink::CloseAllDocuments();


	//From CDocManager
	override void OnFileNew();
	override CDocumentEx* OpenDocumentFile(LPCTSTR lpszFileName); 
	//--------------------------------------------------------------------
	CDocTemplate * FindProjectDocTemplate();
	CMTDevDocManager();
	virtual ~CMTDevDocManager();

	DECLARE_MESSAGE_MAP()
	afx_msg void cmdOnFileSaveAll();
	afx_msg void cmdupOnFileSaveAll(CCmdUI *pCmdUI);
	afx_msg void cmdOnFileCloseAll();
	afx_msg void cmdupOnFileCloseAll(CCmdUI *pCmdUI);
};
