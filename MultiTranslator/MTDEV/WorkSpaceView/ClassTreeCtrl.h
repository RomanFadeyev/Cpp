/********************************************************************
	created:	2002/03/08
	created:	8:3:2002   9:50
	filename: 	$(Projects)\Sources\MTDEv\WorkSpaceView\ClassTreeCtrl.h
	file path:	$(Projects)\Sources\MTDEv\WorkSpaceView

	file base:	ClassTreeCtrl
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	���������� ������� "Classes" � WorkSpaceBar
*********************************************************************/

#if !defined(AFX_ClassTreeCtrl_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_)
#define AFX_ClassTreeCtrl_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include <map>
#include <ProjectTree_B.h>
#include <BaseUtils.h>

#include "BaseStructs.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass window
class CProjectTreeClass :	public CProjectTree_B, 
							public IClassTreeLink, 
							public ISourceViewLink::IEventSink
{
public:
	//����� ������ ����������������� �����
	enum EPredefItems	{iidNone, iidProjectRoot,iidRulesFolder, iidFuncsFolder, iidStructsFolder, iidRulesRecentlyFolder, iidRuleItem,iidFuncItem,iidStructItem,iidUserFolder,iidUserItem};
	enum EUserItems		{iuidUserFolder=iidUserFolder,iuidUserItem=iidUserItem};

	//��� ����� ����������� ��������
	enum ESortMode {smText, smPosition};

	//--------------------------------------------------------------------
	//���������� � ������� - ����� ID � �������� ���������, ����� - � iidSourcesFolder  � �.�.
	class CClassTreeItemInfo;
	class CClassTreeSkelInfo: public CProjectTree_B::CSkeletonInfo
	{
	public:
		HTREEITEM m_hRulesFolder;
		HTREEITEM m_hFuncsFolder;
		HTREEITEM m_hStructsFolder;
		HTREEITEM m_hRulesRecentlyFolder;

		//����� ����������������� ���������
		typedef map<string_std, CClassTreeItemInfo*> mapped_items;
		mapped_items m_mapMappedItems;

		CClassTreeSkelInfo(IProjectDocLink & pProjectLink): CSkeletonInfo(pProjectLink){
		}

		//��������� ������� � ������������ ������
		void MapItemToFolder(string_adapter szItemName, CClassTreeItemInfo* pFolder){
			m_mapMappedItems[szItemName.c_str()]=pFolder;
		}

		//������� �����, � �������������, ��������� ��� ����������� �� ���
		void RemoveMapFolder(CClassTreeItemInfo* pFolder){
			for (mapped_items::iterator i = m_mapMappedItems.begin(); i!=m_mapMappedItems.end();)
				if ((*i).second==pFolder){
					m_mapMappedItems.erase(i); i=m_mapMappedItems.begin();
				}
				else
					++i;
		}

		//������� ������������� �����
		void RemoveMapping(cstr szItemName){
			mapped_items::iterator i = m_mapMappedItems.find(szItemName); 
			if (i!=m_mapMappedItems.end())
				m_mapMappedItems.erase(i);
		}
		
		//������� �� ����� �������� ��� �������������� � �����
		CClassTreeItemInfo* FindMappedFolder(cstr szItemName){
			mapped_items::iterator i = m_mapMappedItems.find(szItemName);
			if (i==m_mapMappedItems.end())
				return NULL;
			return (*i).second;
		}

		override CClassTreeSkelInfo & operator = (CClassTreeSkelInfo & Src){
			CSkeletonInfo::operator = (Src);
			m_hRulesFolder	= Src.m_hRulesFolder;
			m_hFuncsFolder	= Src.m_hFuncsFolder;
			m_hStructsFolder= Src.m_hStructsFolder;
			return * this;
		}
	};
	//--------------------------------------------------------------------
	//���������� � ������ ������� ����
	class CClassTreeItemInfo: public CProjectTree_B::CItemInfo
	{
		CClassTreeItemInfo * m_pNextPal; //������ ���� "���������"
		CClassTreeItemInfo * m_pPrevPal; //����� ���������� ��������� ��������� � ����� ������
	public:
		//�����������
		CClassTreeItemInfo(CProjectTreeClass & TreeWnd,HTREEITEM hItem, CButtonState & btnState, ELevelItems LevelType, EPredefItems nType, DWORD nID, cstr szFilePath, CClassTreeSkelInfo*  pSkelInfo):
		  CItemInfo(TreeWnd,hItem, btnState, LevelType, nType, szFilePath, pSkelInfo){
			m_nID=nID;
			m_pNextPal=m_pPrevPal=NULL;
			TreeWnd.m_ItemCache.AddItem(this);
		}
		//����������
		~CClassTreeItemInfo(){
			RemoveFromPals();
			m_pSkelInfo->RemoveMapFolder(this);
			static_cast<CProjectTreeClass*>(m_pTreeWnd)->m_ItemCache.RemoveItem(this);
		}

		//��������
		CClassTreeItemInfo * GetNextPal() {return m_pNextPal;}
		CClassTreeItemInfo * GetPrevPal() {return m_pPrevPal;}
		//������� ���� �� ������� "������"
		void RemoveFromPals(){
			if (m_pNextPal){
				ASSERT(m_pNextPal->m_pPrevPal==this);
				m_pNextPal->m_pPrevPal=m_pPrevPal;
			}
			if (m_pPrevPal){
				ASSERT(m_pPrevPal->m_pNextPal==this);
				m_pPrevPal->m_pNextPal=m_pNextPal;
			}
			m_pPrevPal=m_pNextPal=NULL;
		}
		//�������� ���� � ������� ������. ���� �� ��� ����� �� ������ �������� � �������
		//����������� � ������� ����� ���������� Item
		void IncludeToPals(CClassTreeItemInfo * pItem){
			ASSERT(pItem!=this); RemoveFromPals(); 
			m_pNextPal=pItem;
			m_pPrevPal=pItem->m_pPrevPal;
			if (m_pPrevPal)
				m_pPrevPal->m_pNextPal=this;
			pItem->m_pPrevPal=this;
		}
		//�������, ���� �� � �������� ������
		bool HasPals() const {return m_pNextPal || m_pPrevPal;}
		
		//���������� ������ ������� ������ � �����, � ������, ������� ���� ������
		void GetPalChain(vector<CClassTreeItemInfo*> & vPals){
			//��������� ���� ������ ������ � ������ ���� 
			for (CClassTreeItemInfo * pPalItem=this; pPalItem; pPalItem=pPalItem->GetNextPal())
				vPals.push_back(pPalItem);
			//��������� ���� ������ �����
			for (CClassTreeItemInfo * pPalItem=this->GetPrevPal(); pPalItem; pPalItem=pPalItem->GetPrevPal())
				vPals.push_back(pPalItem);		
		}
		//���������� ���� � �������, ����������� ������� 
		cstr GetProjectPath() const {return __super::m_pSkelInfo->GetPath();}
		
		//��������� ������� � ������ 
		void MapItemToMe(cstr szItemName) {m_pSkelInfo->MapItemToFolder(szItemName,this);}

		__declspec(property(get=get_Skel)) CClassTreeSkelInfo* m_pSkelInfo;
		__declspec(property(get=get_Type)) EPredefItems m_nType;

		CClassTreeSkelInfo* get_Skel() {return dynamic_cast<CClassTreeSkelInfo*>(__super::m_pSkelInfo);}
		EPredefItems get_Type() {return static_cast<EPredefItems>(__super::m_nType);}
	};

	//--------------------------------------------------------------------
	//������������ ������� �� �������� ������ Recently Used Rules
	class CRecentlyRules: public CWnd
	{
		bool				m_bRunning;
		UINT				m_nTimerID;
		CProjectTreeClass *	m_pWnd;
		CProjectTreeClass::CClassTreeItemInfo * m_pCatchedRule;
		CProjectTreeClass::CClassTreeItemInfo * m_pRecentlyAddedRule;
	public:	
		//������ � �������
		void Begin();
		void End();
		
		CClassTreeItemInfo * InsertItemIntoRecently(CClassTreeItemInfo * pItemInfo);

		CRecentlyRules(CProjectTreeClass * pOwnerWnd):	m_pWnd(pOwnerWnd),m_bRunning(false),m_nTimerID(0),m_pCatchedRule(NULL),
														m_pRecentlyAddedRule(NULL){
		}
	public:
		DECLARE_MESSAGE_MAP()
		afx_msg void OnTimer(UINT nIDEvent);
	};
	
	//--------------------------------------------------------------------
	//������������ ������� �� ������������ ������ 
	class CScan
	{
	protected:
		//������������ ��� ���������� ���������� �� ����� ������������. ��� �������
		//�����, ��������� � �������, ��������� �������� ���������
		struct ScanData{
			string_std	strFilePath;		//� ����� ������ ��������
			string_std	strProjectPath;		//������  �������
			FILETIME	ftLastModified;		//��� �������� �� �����
			time_t		tLastModified;		//��� �������� ����������

			ScanData():tLastModified(-1){
				memset(&ftLastModified,0, sizeof(FILETIME));
			}
			ScanData (cstr szProjectPath,cstr szFilePath):tLastModified(-1),strFilePath(szFilePath),strProjectPath(szProjectPath){
				memset(&ftLastModified,0, sizeof(FILETIME));
			}
		};
		//��������� ��� �������� ������ � �����
		struct ScanDataTransfer{
			ScanData*	in_pData;
			bool		in_bForcedRescan;
			CScan*		in_pSelf;
			
			bool		out_bGetIt;
			IParserLink::syntax_info out_SyntaxInfo;

			ScanDataTransfer(ScanData * pData, bool bForced, CScan* pObj):
				in_pData(pData),in_bForcedRescan(bForced),in_pSelf(pObj){
			}
		};

		typedef map<string_smart,ScanData> map_scan;
		string_smart		m_strCurProjectPath;
		string_smart		m_strCurFilePath;
		DWORD				m_nScanNo;		//���������� ����� ��������� ������������
		CProjectTreeClass *	m_pWnd;
		map_scan			m_mapScanData;
		volatile HANDLE		m_hCurrentScanThread; //���������� �����, ������� ��������� ������
		static CCriticalSection	m_CritSect;
		static bool			m_bScanning;

		//���������� ������� � ������
		static DWORD WINAPI ScanThread(CScan * pSelf);
	public:
		CScan(CProjectTreeClass * pOwnerWnd):m_pWnd(pOwnerWnd),m_nScanNo(0),m_hCurrentScanThread(0) {
		}
		
		//������ � �������
		void Begin();
		void End();
		bool IsScanning() const {return m_bScanning;}
		CProjectTreeClass *	GetWnd() const {return m_pWnd;};
		
		//���� �� ����� ����� � ������� ��������� ������ � �����
		ScanData * FindData(cstr szProjectPath, cstr szFilePath);
		//��������� ��������� ������ ��� ���������� ����� �������
		ScanData * AddData(cstr szProjectPath, cstr szFilePath);
		//������� ��������� ������ ��� ���������� �����. ���� ��������� ��� 
		//���������� ����� �� �������, ���������� false;
		bool DeleteData(cstr szProjectPath, cstr szFilePath);

		virtual ~CScan();
		//�������� ��������� �� ����������� ������� 
		bool GetNextScanFile(string_smart & OUT strProjectName, string_smart & OUT strFileName);
		//��������� ������������ ��������� ����. bForcedRescan ���������, ��������� �� ����������� ������������
		void ScanFile(cstr szProjectPath, cstr szFilePath, bool bForcedRescan); 
		//��������� ��� ����� � �������
		void ScanWholeProject(cstr szProjectPath); 
		//������� ��������������� ���������� � �����
		void DeleteFileInfo(cstr szProjectPath, cstr szFilePath); 
	};
	friend class CScan;

	//--------------------------------------------------------------------
	//����� ��� ���������� ��������� �����, ����������� �������������
	class CFolderManager{
	private:
		CProjectTreeClass *	m_pWnd;
		
		string_smart GetDataFileName(CClassTreeSkelInfo * pProjectInfo);
	public:
		CFolderManager(CProjectTreeClass * pOwner): m_pWnd(pOwner){}
		
		//��������� ������ � �����
		bool OpenProject(CClassTreeSkelInfo * pProjectInfo);
		//�������� ��� �������� �������
		void CloseProject(CClassTreeSkelInfo * pProjectInfo);
		//��������� ������ ���������� �������
		bool SaveProject(CClassTreeSkelInfo * pProjectInfo);
	};

	//--------------------------------------------------------------------
	//����� ��� ���������� ������������������ ������ � �������
	class CTreeItemCache{
	private:
		CProjectTreeClass *	m_pWnd;
		typedef list<CClassTreeItemInfo*> list_cache;
		list_cache m_lstCache;
	public:
		typedef list_cache::iterator iterator;

		CTreeItemCache(CProjectTreeClass * pOwner): m_pWnd(pOwner){
		}
		void AddItem(CClassTreeItemInfo * pItemInfo);
		void RemoveItem(CClassTreeItemInfo * pItemInfo);

		iterator FirstPos() {return m_lstCache.begin();}
		iterator EndPos()	{return m_lstCache.end();}

		CClassTreeItemInfo * FindByName(cstr szName);
		CClassTreeItemInfo * FindByLocation(CClassTreeSkelInfo * pProjectInfo, cstr szFilePath, size_t nPosInText);
		CClassTreeItemInfo * Find(ELevelItems nItemLevel, EPredefItems nItemType, cstr szFilePath,CClassTreeSkelInfo * pSkelInfo,cstr szItemName);
	};

public:
	//��������� ������ ������ ��� �������
	override bool AddProjectHeader(IProjectDocLink & ieProjectLink);
	//������� ���� ������
	override bool RemoveProject(LPCTSTR szProjectPath);
	//���������� �������� ������ � ������� �� ��� �����
	override CClassTreeSkelInfo*  GetProjectHeader(LPCTSTR szProjectPath);
//	//����������� ����������� ����� ��� ����, ����� ���������� �������������� �����
//	CClassTreeSkelInfo* GetProjectHeader(cstr szToProject){
//		return dynamic_cast<CClassTreeSkelInfo*> (__super::GetProjectHeader(szToProject));
//	}

	//��� ��������� �����, ��� ��������� ������������ ��� � ����������� �� ������
	string_smart DecorateName(cstr szText, ELevelItems nLevel, bool bModified);
    
	CProjectTreeClass();
    virtual ~CProjectTreeClass();
protected:
	typedef map<string_smart, CClassTreeSkelInfo> map_skel;

	map_skel		m_mapProjSkel;
	ESortMode		m_nSortMode;
	CScan			m_Scaner;
	CRecentlyRules	m_RecentlyRules;
	CFolderManager	m_FolderManager;
	CTreeItemCache	m_ItemCache;
	CImageList		m_Images;

public:
	//����������� ����������� ����� ��� ����, ����� ���������� �������������� �����
	CClassTreeItemInfo * GetItemData(HTREEITEM hItem){
		return dynamic_cast<CClassTreeItemInfo *>(__super::GetItemData(hItem));
	}
	//������ �������� �� ����, ��� ������� ����������������,��� ����� �� ������
	//������� ������ �� ������������ �����-�� ������������ �����, � ������ ��
	//����� ������������, �� � ������ ������������� �������
	CClassTreeItemInfo * FindItemUniq(ELevelItems nItemLevel, EPredefItems nItemType, cstr szFilePath,CClassTreeSkelInfo * pSkelInfo,cstr szItemName){
		return m_ItemCache.Find(nItemLevel, nItemType, szFilePath,pSkelInfo,szItemName);
	}
	//����������� ����������� ����� ��� ����, ����� ���������� �������������� �����
	bool GetCurItemData(CClassTreeItemInfo * & pInfo) const {
		CItemInfo* pI; return __super::GetCurItemData(pI) && ((pInfo = dynamic_cast<CClassTreeItemInfo *>(pI))!=NULL);
	}

	//������������ �������� �������� ��������
	//����������� ������������ �� � �� InsertItem. ������� ������� � ����� � ���� - 
	//����� CClassTreeItemInfo, �������� ��� ������. ����� ����������� ��������� ������������
	//������������ �������� ����� �������� ��������� ������ ��������. ���� ������
	//���������� ������� - ���������� ���. ��� �������� �������� ����� ������������ DeleteItem
	CClassTreeItemInfo * AddItem(cstr szItemName, CButtonState & btnState, HTREEITEM hParent, HTREEITEM hInsertAfter,
								 DWORD nID,ELevelItems LevelType, EPredefItems nType, cstr szFilePath, CClassTreeSkelInfo*  pSkelInfo, bool bCheckOnExist=true);

	//���������� ����� ��� ��������� ��������. ��������� ������� � ��� ������, ���� �������� ��� ����� ����� 
	//���� ����������� iidProjectRoot, szToProject ������������
	//������ ���������� nType==iidUserItem ��� nType==iidUserFolder
	CClassTreeItemInfo * AddPredefinedItem(cstr szName,cstr szToProject, EPredefItems nType, cstr szFilePath, DWORD nID,HTREEITEM hInsertAfter=TVI_LAST);

	//����� ������� ����� AddPredefinedItem ��� ���������� �������������� �������
	CClassTreeItemInfo * AddSyntaxItem(IParserLink::syntax_item & SItem, cstr szToProject, DWORD nID, HTREEITEM nInsertAfter=TVI_LAST);

	//������� ��� ���������� "�������������" �������� (nType==iidUserItem ��� nType==iidUserFolder)
	CClassTreeItemInfo * AddUserItem(cstr szName,EUserItems nType, cstr szFilePath, DWORD nID,HTREEITEM hParent, HTREEITEM hInsertAfter=TVI_LAST);

	//������� ��������, ���� � ���� ID!=���������� � ��� ���� m_strFilePath==����������
	void	SiftSyntaxItems(cstr szProjectPath, cstr szFilePath, DWORD nID);
	//��������� �������� ������ ��������� ��������(��������� �������)
	void	SortItems(HTREEITEM hParent, ESortMode nSortMode);
	//��������� ���� ��������� ������
	void	SortProject(cstr szProjectPath);
	//��������� � CItemInfo �������� hItem ��� ���� ��������� ������� �� ���������� �������������
	void	RefresItemHandles(HTREEITEM hParent=NULL);

	//���������� �� ��������� ������� � �������� ��������� �������������� �������
	//� ������
	CClassTreeItemInfo * FindSyntaxItem(cstr szProjectName, cstr szFileName, long nPos);

	//����������� ������� ��� ��������� ������� Drag&Drop
	override bool OnBeginDrag(POINT ptDragPoint, const TVITEM & ItemData);
	override bool OnDragOver(POINT ptDragPoint, const TVITEM & ItemData, HWND hOverWnd,HTREEITEM hOverItem, bool & bUseStdCursors);
	override void OnDragDrop(POINT ptDragPoint, const TVITEM & ItemData, HWND hOverWnd,HTREEITEM hOverItem);

	//������� ������� ��� ������
	override void Clear();

	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &);//�������� ���������� ��������� �������
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &);//��������� ��������� ��������� �������
	override void IDocumentLink::IEventSink::evOnCloseDocument (evdCloseDocumentInfo &);
	override void ISourceViewLink::IEventSink::evOnChangeCursorPos (ISourceViewLink * m_pSender, size_t m_nPos);
	override void OnCloseAppMainWnd();

	//IClassExplorerLink
	override bool IClassTreeLink::SelectSyntaxItem(cstr szProjectName, cstr szFileName, long nPos);

    // Generated message map functions
    //{{AFX_MSG(CProjectTreeClass)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void cmdOnViewSort();
	afx_msg void cmdupOnViewSort(CCmdUI* pCmdUI);
	afx_msg void cmdOnGotoDefinition();
	afx_msg void cmdOnClwClearRecentlyList();
	afx_msg void cmdupOnClwClearRecentlyList(CCmdUI *pCmdUI);
	afx_msg void cmdOnNewFolder();
	afx_msg void cmdupOnNewFolder(CCmdUI *pCmdUI);
	afx_msg void cmdOnClwRename();
	afx_msg void cmdupOnClwRename(CCmdUI *pCmdUI);
	afx_msg void cmdOnClwDelete();
	afx_msg void cmdupOnClwDelete(CCmdUI *pCmdUI);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ClassTreeCtrl_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_)
