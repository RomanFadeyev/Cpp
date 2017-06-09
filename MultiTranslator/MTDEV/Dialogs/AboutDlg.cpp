// AboutDlg.cpp: implementation of the CdlgAbout1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxctl.h>
#include "resource.h"
#include <MiscUtils.h>  // concept objects

#include "AboutDlg.h"

#include "Names.h"

using namespace i_objects;

#define TIMER_RENDERING_ID 1971
#define TIMDER_RENDEING_LATENCY 30
//////////////////////////////////////////////////////////////////////////
//Объект  CdlgAbout::CAboutObject 
//////////////////////////////////////////////////////////////////////////

//Создание переходника для объекта
static i_objects::object_thunk< CdlgAbout> AboutObjectThunk(__MSG_PREFIX__);


//////////////////////////////////////////////////////////////////////
// CdlgAbout
//////////////////////////////////////////////////////////////////////
CdlgAbout::CdlgAbout() : CdlgDialog_B(CdlgAbout::IDD),m_nRenderTimer(0)
, m_strCopyright(_T(""))
{
	//{{AFX_DATA_INIT(CdlgAbout)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CdlgAbout, CdlgDialog_B)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CdlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CdlgDialog_B::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgAbout)
	DDX_Control(pDX, IDC_MAIL, m_btnMail);
	DDX_Control(pDX, IDC_URL, m_btnUrl);
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	DDX_Control(pDX, IDC_LOGO, m_wndLogo);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_COPYRIGHT, m_strCopyright);
}

BOOL CdlgAbout::OnInitDialog() 
{
	CdlgDialog_B::OnInitDialog();

	misc_utils::version_info info(NULL);

	string_smart s(info.get_FileVersion().c_str());
#ifdef _DEBUG
	s.cat(_T(" Debug Version"));
#endif

	m_strVersion=s.c_str();
	m_strCopyright=	info.get_LegalCopyright().c_str();
	UpdateData(false);

	m_btnMail.SetURLPrefix (_T("mailto:"));
	m_btnMail.SetURL (ApplicationData::APP_EMAIL);
	m_btnMail.SetWindowText(ApplicationData::APP_EMAIL);
	m_btnMail.SetTooltip (_T("Send mail to support service"));
	m_btnMail.SizeToContent ();

	m_btnUrl.SetURLPrefix (_T("http://"));
	m_btnUrl.SetURL (ApplicationData::APP_URL);
	m_btnUrl.SetWindowText(ApplicationData::APP_URL);
	m_btnUrl.SetTooltip (_T("Visit us in the web"));
	m_btnUrl.SizeToContent ();

	CRect R1; GetClientRect(R1);
	CRect R2; m_wndLogo.GetWindowRect(R2); ScreenToClient(R2);
	
	long nLeftBorder = R2.left;
	long nRightBorder = R1.right-R2.right;

	if (nLeftBorder!=nRightBorder){
		long nDelta = nRightBorder-nLeftBorder;
		m_wndLogo.SetWindowPos(NULL,R2.left+nDelta/2,R2.top,R2.Width(),R2.Height(),0);
	}

	// Load in our bmp, create our effects etc...
	m_bmpRenderSource.SetBitmap(m_wndLogo.GetBitmap());
	m_bmpRenderTarget.SetBitmap(m_wndLogo.GetBitmap());

	// create water object
	m_myWater.Create(m_bmpRenderTarget.GetWidth(),m_bmpRenderTarget.GetHeight());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//------------------------------------------------------------------------
void CdlgAbout::OnDestroy()
{
	__super::OnDestroy();
	m_bmpRenderSource.DeleteObject();
	m_bmpRenderTarget.DeleteObject();
	if 	(m_nRenderTimer){
		KillTimer(m_nRenderTimer);
		m_nRenderTimer=0;

	}
}
//------------------------------------------------------------------------
void CdlgAbout::OnMouseMove(UINT nFlags, CPoint point)
{
	//Это реализация эффекта воды. Вычисляем, когда пользователь провел мышкой
	//над картинкой
	if (ChildWindowFromPoint(point,CWP_SKIPINVISIBLE)==&m_wndLogo){
		CRect rcPicture;
		m_wndLogo.ScreenToClient((this->ClientToScreen(&point),&point));
		
		CRect ClientRect; m_wndLogo.GetClientRect(&ClientRect);
		if (ClientRect.PtInRect(point)){
			int nIntensity = (MK_LBUTTON & nFlags) ? 60 : 30;
			if (MK_RBUTTON & nFlags) nIntensity+=30;
			int nRadius = (MK_LBUTTON & nFlags) ? 6 : 5;
			m_myWater.HeightBlob(point.x,ClientRect.bottom-point.y,nRadius,nIntensity,m_myWater.m_iHpage);
		}

		if (!m_nRenderTimer)
			m_nRenderTimer=SetTimer(TIMER_RENDERING_ID,TIMDER_RENDEING_LATENCY,NULL);
	}
	__super::OnMouseMove(nFlags, point);
}
//------------------------------------------------------------------------
void CdlgAbout::OnTimer(UINT nIDEvent)
{
	bool bTurnOffTimer =true;	//Ставим в true. Если вылетит - сразу отключим таймер
	
	LOG_PROTECT_CODE_BEGIN
	if(nIDEvent == TIMER_RENDERING_ID)
	{		
		LPVOID pSrcBits=m_bmpRenderSource.GetDIBits();
		LPVOID pDstBits=m_bmpRenderTarget.GetDIBits();

		bTurnOffTimer = (!pSrcBits) || (!pDstBits);	//Для Win98 может венуть NULL

		if (!bTurnOffTimer){
			m_myWater.Render((DWORD*)pSrcBits,(DWORD*)pDstBits);
			m_bmpRenderTarget.Draw(&CClientDC(&m_wndLogo),CPoint(0,0));
			bTurnOffTimer = m_myWater.IsFlattened();
		}
	}
	LOG_PROTECT_CODE_END

	//Если таймер больше не нужен, отключаем
	if (bTurnOffTimer){
		KillTimer(m_nRenderTimer);
		m_nRenderTimer=0;
	}

	__super::OnTimer(nIDEvent);
}

