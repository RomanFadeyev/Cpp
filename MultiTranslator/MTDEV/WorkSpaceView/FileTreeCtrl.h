/********************************************************************
	created:	2002/03/08
	created:	8:3:2002   9:52
	filename: 	$(Projects)\Sources\MTDEv\WorkSpaceView\FileTreeCtrl.h
	file path:	$(Projects)\Sources\MTDEv\WorkSpaceView

	file base:	FileTreeCtrl
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	���������� ������� "Files" � WorkSpaceBar
*********************************************************************/
#if !defined(AFX_FILETREECTRL_H__7D0E29C4_26B5_4282_BCA7_A10263D783D7__INCLUDED_)
#define AFX_FILETREECTRL_H__7D0E29C4_26B5_4282_BCA7_A10263D783D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <ProjectTree_B.h>
#include <BaseUtils.h>

#include "BaseStructs.h"
/////////////////////////////////////////////////////////////////////////////
// CProjectTreeFile window

using namespace std;
using namespace base_utils;
 
class CProjectTreeFile : public CProjectTree_B
{
public:
	//����� ������ ����������������� �����
	enum EPredefItems {iidNone, iidProjectRoot,iidSourcesFolder, iidOtherFolder, iidFile};

	//���������� � ������� - ����� ID � �������� ���������, ����� - � iidSourcesFolder  � �.�.
	struct CFileTreeSkelInfo:public CProjectTree_B::CSkeletonInfo{
		HTREEITEM m_hSourcesFolder;
		HTREEITEM m_hOtherFolder;
		
		CFileTreeSkelInfo(IProjectDocLink & pProjectLink): CSkeletonInfo(pProjectLink){
		}

		override CFileTreeSkelInfo & operator = (CFileTreeSkelInfo & Src){
			CSkeletonInfo::operator = (Src);
			m_hSourcesFolder		= Src.m_hSourcesFolder;
			m_hOtherFolder			= Src.m_hOtherFolder;
			return * this;
		}
	};
protected:
	//������������ ������� �� ������������ ������ 
	struct CScan{
		HTREEITEM	hCur;
		DWORD		m_TimerID;
		CProjectTreeFile*	m_pWnd;
		CScan(CProjectTreeFile * pOwnerWnd):m_pWnd(pOwnerWnd),hCur(NULL),m_TimerID(0) {
		}
		void Begin(){
			if (!m_TimerID) m_TimerID=m_pWnd->SetTimer(1, 1000, 0);
		}
		void End()	{
			if (m_TimerID) m_pWnd->KillTimer(m_TimerID);m_TimerID=0;
		}
	public:
		void ScanNext();//�������� ��������� �� ����������� ������� � ��������� ���
		void ScanCur();	//��������� ������� �������
	};
public:
	//��������� ������ ������ ��� �������
	override bool AddProjectHeader(IProjectDocLink & ieProjectLink);
	//������� ���� ������
	override bool RemoveProject(LPCTSTR szProjectPath);
	//���������� �������� ������ � ������� �� ��� �����
	override CSkeletonInfo * GetProjectHeader(LPCTSTR szProjectPath);
	//��������� ����� � ��������� ������ � ��������� �����
	bool AddFileToProject(LPCTSTR szProjectPath, LPCTSTR szFileName);
	//������� ���� �� ���������� �������
	bool RemoveFileFromProject(LPCTSTR szProjectPath, LPCTSTR szFileName);
	//���� ���� � �������� ������ �� ���� �������� ��������. ���������� ������ CItemInfo*
	//���� ������ ���� �� ���� ����, ���������� true
	bool FindFiles(LPCTSTR szFindFileName, vector_item_info & out_vFounded);

	//��� ��������� �����, ��� ��������� ������������ ��� � ����������� �� ������
	string_smart DecorateName(const char * szText, ELevelItems nLevel, bool bModified);
	
	CProjectTreeFile();
	virtual ~CProjectTreeFile();

protected:
	HTREEITEM AddItem(LPCTSTR szName,LPCTSTR szToProject, EPredefItems nType, HTREEITEM hParent, LPCTSTR szFilePath=NULL);
	override void Clear();

	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &);//�������� ���������� ��������� �������
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &);//��������� ��������� ��������� �������
	override void IDocumentLink::IEventSink::evOnCloseDocument (evdCloseDocumentInfo &);

	//string_smart ������������ ������, ��� ��� ���� ����� ��������������� ���������
	typedef map<string_smart, CFileTreeSkelInfo> map_skel;
	map_skel		m_mapProjSkel;
	CScan			m_Scaner;
	CImageList		m_Images;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectTreeFile)
	//}}AFX_VIRTUAL
protected:
	// Generated message map functions

	//{{AFX_MSG(CProjectTreeFile)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void cmdOnFileRemove();
	afx_msg void cmdOnFilePhysicalDelete();
	afx_msg void cmdOnProperties();
	afx_msg void cmdOnFileReCreate();
	afx_msg void cmdupOnFilePhysicalDelete(CCmdUI* pCmdUI);
	afx_msg void cmdupOnFileRemove(CCmdUI* pCmdUI);
	afx_msg void cmdupOnFileReCreate(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETREECTRL_H__7D0E29C4_26B5_4282_BCA7_A10263D783D7__INCLUDED_)
