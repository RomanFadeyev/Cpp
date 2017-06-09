/********************************************************************
	created:	2002/05/05
	created:	5:5:2002   18:57
	filename: 	$(Projects)\sources\mtdev\bars\infobar.cpp
	file path:	$(Projects)\sources\mtdev\bars

	file base:	infobar
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Docked-Bar, всегда появляющийся при запуске
*********************************************************************/
#include "stdafx.h"
#include <GuiService.h>
#include "InfoBar.h"

using namespace i_objects;

//Это обработчик события создания объекта
static bool OnCreateObject(){
	return (!ApplicationData::APP_AUTOMATED && !ApplicationData::APP_EMBEDDED);
}

//Создание переходника для объекта
static i_objects::object_thunk<CbarInfo> barInfo(__MSG_PREFIX__,OnCreateObject);

//////////////////////////////////////////////////////////////////////////
/// Cbar_B::IBarLink
//////////////////////////////////////////////////////////////////////////
void CbarInfo::CreateWnd(CFrameWnd* pParentWnd)
{
	__super::CreateWnd(pParentWnd);
	EnableRollUp (BCG_ROLLUP_DYNAMIC_ON);
}
//------------------------------------------------------------------------
void CbarInfo::InitDock()
{
	m_szFloat.cx=200;
	EnableDocking(0);	
	ASSERT(GetParent()->IsKindOf( RUNTIME_CLASS( CFrameWnd) ));
	CFrameWnd* pFrame = (CFrameWnd*)(GetParent());
	pFrame ->FloatControlBar (this, CPoint (GetSystemMetrics(SM_CXFULLSCREEN)-m_szFloat.cx-50,0));

	pFrame = GetParentFrame ();
}

//////////////////////////////////////////////////////////////////////////
// CbarInfo
//////////////////////////////////////////////////////////////////////////
CbarInfo::CbarInfo()
{
}
//------------------------------------------------------------------------
CbarInfo::~CbarInfo()
{
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CbarInfo, CBCGSizingControlBar)
	//{{AFX_MSG_MAP(CbarInfo)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_INFOBAR, cmdOnViewInfo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INFOBAR, cmdupOnViewInfo)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CbarInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectClient; GetClientRect (rectClient);

	dc.FillSolidRect (rectClient, ::GetSysColor (COLOR_3DHILIGHT));
	dc.Draw3dRect (rectClient, ::GetSysColor (COLOR_3DSHADOW),::GetSysColor (COLOR_3DLIGHT));
	

	m_RecentlyMngr.Invalidate(false);
	m_RecentlyHeader.Invalidate(false);
	
	CRect r (10,10+guiGetWindowHeight(&m_RecentlyHeader)-2,rectClient.right-10,0);r.bottom=r.top+3;
	dc.DrawEdge(r,BDR_SUNKENOUTER,BF_TOP);
}
//-------------------------------------------------------------------------
void CbarInfo::cmdOnViewInfo()
{
	ToggleVisible();
}
//-------------------------------------------------------------------------
void CbarInfo::cmdupOnViewInfo(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(IsVisible());
}
//-------------------------------------------------------------------------
int CbarInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_RecentlyMngr.Create(this);
	m_RecentlyMngr.FormRecentlyProjectsList();
	
	m_RecentlyHeader.Create(_T("Recently Projects"),WS_VISIBLE, CRect(10,10,10,10),this);
	m_RecentlyHeader.SetFontName("MS Sans Serif");		m_RecentlyHeader.SetFontSize(8);
	m_RecentlyHeader.SetTransparent(true);				m_RecentlyHeader.SetFontBold(true);
 	m_RecentlyHeader.AutoSize();

 
	return 0;
}
//-------------------------------------------------------------------------
void CbarInfo::OnDestroy()
{
	m_RecentlyMngr.Clear();
	m_RecentlyHeader.DestroyWindow();
	__super::OnDestroy();
}
//-------------------------------------------------------------------------
void CbarInfo::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	m_RecentlyMngr.Move(30,30,cx-20,cy);
}
//-------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////
// CbarInfo::CRecentlyMngr
///////////////////////////////////////////////////////////////////////////
void CbarInfo::CRecentlyMngr::Show(int cmd)
{
	for (int i=0; i<m_nLabelsCount;i++)
		m_aLabels[i].ShowWindow(cmd);
}
//-------------------------------------------------------------------------
void CbarInfo::CRecentlyMngr::Invalidate(bool bErase)
{
	for (int i=0; i<m_nLabelsCount;i++)
		m_aLabels[i].Invalidate(bErase);
}
//-------------------------------------------------------------------------
void CbarInfo::CRecentlyMngr::Move (int left, int top, int w, int h)
{
	for (int i=0; i<m_nLabelsCount;i++)
		m_aLabels[i].MoveWindow(left,top+i*20,w,20);
}
//-------------------------------------------------------------------------
void CbarInfo::CRecentlyMngr::AddString (LPCTSTR szText, LPCTSTR szPath)
{
	if (m_nLabelsCount<arrlength(m_aLabels))
		m_aLabels[m_nLabelsCount++].Create(szText,szPath,m_pParentWnd);
}
//-------------------------------------------------------------------------
void CbarInfo::CRecentlyMngr::Clear()
{
	for (int i=0; i<m_nLabelsCount;i++)
		m_aLabels[i].DestroyWindow();
	m_nLabelsCount=0;
}
//-------------------------------------------------------------------------
int CbarInfo::CRecentlyMngr::FindPath(LPCTSTR szText)
{
	for (int i=0; i<m_nLabelsCount;i++)
		if (m_aLabels[i].m_strPath.CompareNoCase(szText)==0)
			return i;
	return -1;
}
//-------------------------------------------------------------------------
CString& CbarInfo::CRecentlyMngr::operator[](int nIndex)
{
	ASSERT(inrange(nIndex,0,(int)m_nLabelsCount-1));
	return m_aLabels[nIndex].m_strPath;
}
//-------------------------------------------------------------------------
void CbarInfo::CRecentlyMngr::FormRecentlyProjectsList()
{
	TCHAR CurDir[256]; ::GetCurrentDirectory(255,CurDir);
	int CurDirLen=strlen(CurDir);

	CRecentFileList RecentProjectList(0,szProfileRecentProjects ,szParamRecentProjectName ,MAX_MRU_PROJECTS);
	RecentProjectList.ReadList();
	CString Name;
	
	//Проверяем, просто ли добавился элемент в список или требуется полное удаление и заполнение
	for (int i=0;i<RecentProjectList.GetSize();i++){
		if ((i>=GetSize())||(operator[](i).CompareNoCase(RecentProjectList[i])!=0)) 
		{
			Clear();
			for (int j=0;j<RecentProjectList.GetSize();j++)
				if (RecentProjectList.GetDisplayName(Name,j,CurDir, CurDirLen))
					if (!Name.IsEmpty())
						AddString(Name,RecentProjectList[j]);
			break;
		}
	}
}
//-------------------------------------------------------------------------
BOOL CbarInfo::CRecentlyMngr::CRecentlyLabel::Create(LPCTSTR lpszText, LPCTSTR lpszPath, CWnd* pParentWnd)
{
	BOOL res=CLabel::Create(lpszText,WS_VISIBLE,CRect(0,0,0,0),pParentWnd);

	SetFontName("MS Sans Serif");		SetFontSize(8);
	SetTransparent(true);				SetLink(true,true);
	SetTextColor(RGB(0,0,192));			SetFontUnderline(true);
	SetLinkCursor(AfxGetApp()->LoadCursor(IDC_BCGBARRES_HAND));
	m_strPath=lpszPath;
	return res;
}
//-------------------------------------------------------------------------
LRESULT CbarInfo::CRecentlyMngr::CRecentlyLabel::WindowProc(UINT message, WPARAM wP, LPARAM lP)
{
	if (message==WM_LBUTTONDBLCLK)
		AfxGetApp()->OpenDocumentFile(m_strPath);
	return CLabel::WindowProc(message,wP,lP);
}
