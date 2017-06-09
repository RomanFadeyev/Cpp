/********************************************************************
	created:	2002/12/15
	created:	15:12:2002   21:12
	filename: 	$(Projects)\sources\mtdev\mtdev.h
	file path:	$(Projects)\sources\mtdev
	file base:	mtdev
	file ext:	h
	author:		Fadeyev
	
	purpose:	Класс приложения
*********************************************************************/

#if !defined(AFX_MTDev_H__F6850849_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
#define AFX_MTDev_H__F6850849_7AA4_11D3_A74B_0090274409AC__INCLUDED_

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "CObjectEx.h" 
#include <BaseStructs.h>

#include "resource.h"       // main symbols
#include "COMApplication.h"	//COM  

using namespace i_objects;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CMTDevApp:
// See MTDev.cpp for the implementation of this class
//
class CMTDevApp : public CWinApp, public CBCGWorkspace, public CCObjectEx, 
				  public IApplicationLink
{
private:
	int					m_nSupressMBCount;
	CRecentFileList	*	m_pRecentProjectList;
	mtdev_lib::ILibraryInfoPtr	m_sdDevLibraryInfo;
	bool				m_bAutoSaveFiles;
	size_t				m_nAutoSaveNMinutes;
	bool				m_bRunEventRaised;
	//COM Support
	COM::Application *	m_pCoClass;	//Текущий CoClass
	friend class COM::Application;

	typedef list<IIdleHandler*>	list_idle_handlers;
	list_idle_handlers	m_lstIdleHandlerList;	//Список всех Idle-обработчиков
protected:	
	void LoadApplicationInfo();
	void SetRegistryKey();
	bool LoadLibraries();
	void LoadMRU();

	//from IApplicationLink
	override ref_cnt_ptr<COM::IApplication> IApplicationLink::GetComInterface();
	override bool IApplicationLink::Quit(bool bForcedClose=false); 
	override void IApplicationLink::SupressMessageBoxes(bool bSupress);
	override registry GetAppVersionBasedRegistryKey();
	override registry GetAppSteadyRegistryKey();

	override bool IncludeToOnIdleMap(IIdleHandler* pHandler);
	override bool ExcludeFromOnIdleMap(IIdleHandler* pHandler);

	//From CWinApp
	override void AddToRecentFileList(LPCTSTR lpszPathName);  
	override int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	override BOOL PumpMessage();
	override BOOL OnIdle(LONG lCount);

	//From CCObjectEx 
	override void OnStorageDataChanged(IStorageLink::EParam nParameterChanged);

	// Overrides
	//{{AFX_VIRTUAL(CMTDevApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	virtual void PreLoadState();
	virtual void LoadCustomState ();
	virtual void SaveCustomState ();
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CMTDevApp)
	afx_msg BOOL cmdOnRecentProject(UINT nID);
	afx_msg BOOL cmdOnNewDoc(UINT nID);
	afx_msg void cmdupOnRecentProjects(CCmdUI* pCmdUI);
	afx_msg void cmdupOnNewDocs(CCmdUI *pCmdUI);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CMTDevApp();
	virtual ~CMTDevApp();
	
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MTDev_H__F6850849_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
