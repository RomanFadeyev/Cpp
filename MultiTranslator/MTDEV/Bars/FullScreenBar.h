/********************************************************************
	created:	2002/08/20
	created:	20:8:2002   8:44
	filename: 	$(Projects)\sources\mtdev\bars\fullscreenbar.h
	file path:	$(Projects)\sources\mtdev\bars
	file base:	fullscreenbar
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	������ ������������ �� ����� ������ - FullScreen
*********************************************************************/
#pragma once


#include "CobjectEx.h"	//���� �� ���������

#include "Names.h"
#include "BaseStructs.h"
#include "Resource.h"

using namespace std;
class CbarFullScreen : public CBCGToolBar,public CCObjectEx, public IBarLink 
{
private:
	typedef  map<int,CControlBar*> cbar_map;	
	cbar_map m_bars;
	bool m_bFullScreen;
	WINDOWPLACEMENT m_wpOrigPlacement;
	//����� true, ���� ���� ������� � ������� ���������
	bool IsVisible();
	//�������-�������. �������� ��������� ��������� ������� �� ���������������. 
	bool IsFullScreen();
	void ToggleFullScreen();
protected:
	//�������� �������� ����
	override void OnCreateAppMainWnd()		{
		i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}
	//�������� �������� ����
	override void OnCloseQueryAppMainWnd(bool & CanClose)	{
		if (IsFullScreen()) ToggleFullScreen();}

	//��������� � ������ �������
	override void CreateWnd(CFrameWnd* pParentWnd);
	override void InitDock();
	override void OnShowMainFrame(bool bVisible){}
	override CControlBar * GetObject();
	
	//Multiple Instance Link
	override size_t GetObjectID()const	{return IBarLink::idBarFullScreen;}
	
	//---------------------------------------------------------------------
	//�������������� ����
	cstr	GetBarName()	{return barnFullScreen;}
	DWORD	GetBarIDR()		{return IDR_FULLSCREEN;}
	DWORD	GetBarID()		{return ID_VIEW_FULLSCREEN;}

	//{{AFX_MSG(CbarInfo)
	afx_msg void msgOnShow();
	afx_msg void msgOnHide();
	//}}AFX_MSG
	DECLARE_DYNAMIC(CbarFullScreen)
	DECLARE_MESSAGE_MAP()
public:
	CbarFullScreen();
	virtual ~CbarFullScreen();
	afx_msg void cmdOnViewFullscreen();
	afx_msg void cmdupOnViewFullscreen(CCmdUI *pCmdUI);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};
