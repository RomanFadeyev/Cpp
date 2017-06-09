/********************************************************************
	created:	2002/09/21
	created:	21:9:2002   11:33
	filename: 	$(Projects)\sources\mtdev\workspaceview\projecttree_b.h
	file path:	$(Projects)\sources\mtdev\workspaceview
	file base:	projecttree_b
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma  once 

#include <map>
#include <ExtControls.h>
#include <BaseUtils.h>
#include <MFCUtils.h>
#include <GUIService.h>

#include "Names.h"
#include "BaseStructs.h"
#include "CObjectEx.h" //���� �� ���������

/////////////////////////////////////////////////////////////////////////////
// CProjectTree_B window

using namespace mfc_utils;
using namespace base_utils;

//��������� � �������� ������ ���� �������� �������
#define MENU_ADD_PROJECT_NAME(pCmdUI)\
	if (pCmdUI->m_pMenu){\
		static string_smart strOriginal(CCmdUIEx(pCmdUI).GetMenuItemText());\
		CItemInfo * pI; if (GetCurItemData(pI))\
			pCmdUI->SetText(string_smart(strOriginal.c_str()," <",pI->m_pSkelInfo->GetProjectLink()->GetName().c_str(),">").c_str());\
	}

class CProjectTree_B :	public CTreeCtrlEx,public CCObjectEx,
						public IDocumentLink::IEventSink
{
public:
	enum ELevelItems {ilvlRoot,ilvlProject,ilvlFolder, ilvlItem};

	//���������� � ������� - ����� ID � �������� ���������, ����� - � iidSourcesFolder  � �.�.
	class CItemInfo;
	class CSkeletonInfo{
	protected:
		string_smart m_strProjectPath;
		string_smart m_strProjectTitle;
		IProjectDocLink * m_pProjectLink; 
	public:
		HTREEITEM		  m_hProjectRoot;

		IProjectDocLink * GetProjectLink() const {return m_pProjectLink; }
		cstr GetPath()	const  {return m_strProjectPath.c_str();}
		cstr GetTitle() const {return m_strProjectTitle.c_str();}

		CSkeletonInfo(IProjectDocLink & pProjectLink): m_pProjectLink(&pProjectLink){
			m_strProjectPath =m_pProjectLink->GetPath();
			m_strProjectTitle=m_pProjectLink->GetName();
		}

		virtual CSkeletonInfo & operator = (CSkeletonInfo & Src){
			m_hProjectRoot			= Src.m_hProjectRoot;
			m_pProjectLink			= Src.m_pProjectLink;
			return * this;
		}
	};
	//��������� - ��������� ��� ������� ��������. 
	class CItemInfo{
	protected:
		string_smart    m_strText;
	public:
		CProjectTree_B* m_pTreeWnd;
		HTREEITEM		m_hItem;			//���������� ���������� ��������, ���������� ���� ����������
		ELevelItems		m_nLevelType;
		string_smart	m_strFilePath;
		DWORD			m_nAttr;
		DWORD			m_nID;				//���������� �������������
		DWORD			m_nType;			//��� �������� (����: �������, ���������, ����, ����� � �.�.)
		DWORD			m_nPosStart;		//������� �� � ������	
		DWORD			m_nPosEnd;			//������� �� � ������	
		CSkeletonInfo*  m_pSkelInfo;
		CButtonState	m_BtnState;
		//�����������
		CItemInfo(CProjectTree_B & TreeWnd,HTREEITEM hItem, CButtonState & btnState, ELevelItems LevelType, DWORD nType, cstr szFilePath, CSkeletonInfo* pSkelInfo):
			m_pTreeWnd(&TreeWnd),		m_hItem(hItem),		m_BtnState(btnState),
			m_nLevelType(LevelType),	m_strFilePath(szFilePath),
			m_nAttr(0),			m_pSkelInfo(pSkelInfo),		m_nType(nType), m_nID(0),
			m_nPosStart(0),		m_nPosEnd(0)
		{
			m_strText=LPCTSTR(m_pTreeWnd->GetItemText(hItem));
		}
		//���������� �� m_hItem, m_strFilePath, m_strFilePath, m_nType, m_pSkelInfo
		//����� �� �������� � ������� ������ �������� (GetItemText)
		bool IsAppropriate(ELevelItems nLevelType, DWORD nType, cstr szFilePath,cstr szText,CSkeletonInfo * pSkelInfo){
			return	(m_nType == nType) && 
					(m_nLevelType == nLevelType) &&
					(m_strText.cmp(szText)) &&
					(m_strFilePath.cmpi(szFilePath)) &&
					(m_pSkelInfo==pSkelInfo);

		}
		//���������� �����, ������������ � ������
		cstr GetText() const{
			return m_strText.c_str();
		}
		//���������� �����, ������������ � ������
		void SetText(cstr szText) {
			m_strText=szText;
			m_pTreeWnd->SetItemText(m_hItem,szText);
		}
		//���������� ��� ���� ����� �� ���� ����� � ����� ���������� ��������
		string_smart GetPath() const {
			return LPCTSTR(m_pTreeWnd->GetItemPath(m_hItem));
		}
		//��������
		HTREEITEM GetParent() const {
			return m_pTreeWnd->GetParentItem(m_hItem);
		}
		//��������
		CButtonState GetImages() const {
			return m_BtnState;
		}

		virtual ~CItemInfo(){	//����������� ����������� ��� �������� VMT
		}

	};
	//������ ���������� �� CItemInfo
	typedef vector<CItemInfo*> vector_item_info;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectTree_B)
	//}}AFX_VIRTUAL

// Implementation
public:
	//��������� ������ ������ ��� �������
	virtual bool AddProjectHeader(IProjectDocLink & ieProjectLink)=0;
	//������� ���� ������
	virtual bool RemoveProject(cstr szProjectPath)=0;
	//���������� �������� ������ � ������� �� ��� �����
	virtual CSkeletonInfo * GetProjectHeader(cstr szProjectPath)=0;

	//����������� ����������� ����� ��� ����, ����� ���������� �������������� �����
	CItemInfo * GetItemData(HTREEITEM hItem) const {
		return reinterpret_cast<CItemInfo *>(CTreeCtrlEx::GetItemData(hItem));
	}
	//���������� ��������� ������ ��� �������� ��������. ���� ��� ������, ���������� true
	bool GetCurItemData(CItemInfo * & pInfo) const {
		HTREEITEM hItem=GetSelectedItem();
		if (!hItem) {
			pInfo=NULL;
			return false;
		}
		pInfo=GetItemData(hItem);
		return (pInfo!=NULL);
	}
	//������������� � �������� ������/�������� �����
	void SetItemBold(HTREEITEM hItem, bool bBold);

	//��������� ��������� HTREEITEM �� ��, �������� �� ��� �������� ��� ��������
	//bool IsAppropriateItem(HTREEITEM hItem, ELevelItems nItemLevel, DWORD nItemType, cstr szFilePath,CSkeletonInfo * pSkelInfo,cstr szItemName);

	//���� ������� � ���������� ��������, ������� ������������� 
	//��������� ����������: ������������� �������, ��� ��������, ���� � �����, ��������� �� �����������
	//���� �������� � ��� ��������
	CItemInfo * FindItem(HTREEITEM hParentItem, ELevelItems nItemLevel, DWORD nItemType, cstr szFilePath,CSkeletonInfo * pSkelInfo,cstr szItemName, bool bLookInSubFolders);
	CTreeCtrlEx::FindItem;

	CProjectTree_B();
	virtual ~CProjectTree_B();
protected:
	virtual void DeleteItem(HTREEITEM hItem);
	virtual void Clear();
	
	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &);//�������� ���������� ��������� �������
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &);//��������� ��������� ��������� �������
	override void IDocumentLink::IEventSink::evOnCloseDocument (evdCloseDocumentInfo &);

	//�������� �������� ����
	override void OnCreateAppMainWnd()		{
		i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}

	override BOOL PreCreateWindow(CREATESTRUCT& cs);
	override BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	// Generated message map functions
	//{{AFX_MSG(CProjectTree_B)
	afx_msg void cmdOnProjectSetActive();
	afx_msg void cmdupOnProjectSetActive(CCmdUI* pCmdUI);
	afx_msg void cmdOnProperties();
	afx_msg LRESULT OnPostedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
