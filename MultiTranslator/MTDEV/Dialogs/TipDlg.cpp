/*************************************************************************
	created:	2001/12/29
	created:	29:12:2001   11:41
	filename: 	$(Projects)\Sources\MTDEv\TipDlg.cpp
	file path:	$(Projects)\Sources\MTDEv
	file base:	TipDlg
	file ext:	cpp
	author:		Фадеев Р.В. (Fadeyev R.V.)
	
	purpose:	Диалог "Tip of the Day". Может появляться при старте программы
				а также вызываться из меню			
*************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "TipDlg.h"
#include <sys\stat.h>
#include "Names.h"

//Создание переходника для объекта
static i_objects::object_thunk< CdlgTip > TipDlg(__MSG_PREFIX__);

/////////////////////////////////////////////////////////////////////////////
// CdlgTip dialog

#define MAX_BUFLEN 1000

static const TCHAR szSection[] = _T("Tip");
static const TCHAR szIntFilePos[] = _T("FilePos");
static const TCHAR szTimeStamp[] = _T("TimeStamp");
static const TCHAR szIntStartup[] = _T("StartUp");

//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CdlgTip, CdlgDialog_B)
	//{{AFX_MSG_MAP(CdlgTip)
	ON_BN_CLICKED(IDC_NEXTTIP, OnNextTip)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
CdlgTip::CdlgTip(CWnd* pParent /*=NULL*/)
	: CdlgDialog_B(IDD_TIP, pParent)
{
	//{{AFX_DATA_INIT(CdlgTip)
	m_bStartup = TRUE;
	//}}AFX_DATA_INIT
	// Now try to open the tips file

	string_res strDefFileName(IDS_TOD_FILENAME,HRESOURCES);
	m_strTipFileName = string_smart(ApplicationData::APP_DIR, strDefFileName.c_str());
#ifdef _DEBUG
	//Если отладочная версия, можно поискать в каталоге выше от нашего каталога 
	if (!is_file_exist(m_strTipFileName.c_str()))
		m_strTipFileName = get_abs_path(_T("..\\"),ApplicationData::APP_DIR).cat(strDefFileName.c_str());
	//... или в текущем каталоге (вообще-то, это опасно, текущий каталог может быть любым)
	if (!is_file_exist(m_strTipFileName.c_str()))
		m_strTipFileName = expand_to_full_path(string_res(IDS_TOD_FILENAME,HRESOURCES).c_str());

#endif
	if (!is_file_exist(m_strTipFileName.c_str()))
		LOG_ERROR(string_res(IDS_TOD_FILE_ABSENT,HRESOURCES).cat(_T(":")).cat(m_strTipFileName.c_str()).c_str());
}
//------------------------------------------------------------------------
CdlgTip::~CdlgTip()
{
}
//------------------------------------------------------------------------
int CdlgTip::DoModal()
{
	//Получим указатель на Application
	CWinApp* pApp = AfxGetApp();
	//Получим указатель записи в файле
	UINT iFilePos = pApp->GetProfileInt(szSection, szIntFilePos, 0);
	// Now try to open the tips file
	_ASSERTE(!m_strTipFileName.isempty());

	m_pStream = fopen(m_strTipFileName.c_str(), "r");
	if (m_pStream == NULL) 
	{
		AfxMessageBox(IDS_TOD_FILE_ABSENT);
		return FALSE;
	} 

	// If the timestamp in the INI file is different from the timestamp of
	// the tips file, then we know that the tips file has been modified
	// Reset the file position to 0 and write the latest timestamp to the
	// ini file
	struct _stat buf;_fstat(_fileno(m_pStream), &buf);
	CString strCurrentTime = ctime(&buf.st_ctime);
	strCurrentTime.TrimRight();
	CString strStoredTime = 
		pApp->GetProfileString(szSection, szTimeStamp, NULL);
	if (strCurrentTime != strStoredTime) 
	{
		iFilePos = 0;
		pApp->WriteProfileString(szSection, szTimeStamp, strCurrentTime);
	}

	if (fseek(m_pStream, iFilePos, SEEK_SET) != 0) 
	{
		AfxMessageBox(IDS_TOD_FILE_CORRUPT);
		return FALSE;
	}
	else 
	{
		GetNextTipString(m_strTip);
	}
	int Result=__super::DoModal();

	// But make sure the tips file existed in the first place....
	if (m_pStream != NULL) 
	{
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(szSection, szIntFilePos, ftell(m_pStream));
		fclose(m_pStream);
	}
	return Result;
}
//------------------------------------------------------------------------
void CdlgTip::DoDataExchange(CDataExchange* pDX)
{
	CdlgDialog_B::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgTip)
	DDX_Control(pDX, IDC_NEXTTIP, m_buNext);
	DDX_Control(pDX, IDOK, m_buOK);
	DDX_Check(pDX, IDC_STARTUP, m_bStartup);
	DDX_Text(pDX, IDC_TIPSTRING, m_strTip);
	//}}AFX_DATA_MAP
}
//------------------------------------------------------------------------
void CdlgTip::OnNextTip()
{
	GetNextTipString(m_strTip);
	UpdateData(FALSE);
}
//------------------------------------------------------------------------
void CdlgTip::GetNextTipString(CString& strNext)
{
	LPTSTR lpsz = strNext.GetBuffer(MAX_BUFLEN);

	// This routine identifies the next string that needs to be
	// read from the tips file
	BOOL bStop = FALSE;
	while (!bStop) 
	{
		if (_fgetts(lpsz, MAX_BUFLEN, m_pStream) == NULL) 
		{
			// We have either reached EOF or enocuntered some problem
			// In both cases reset the pointer to the beginning of the file
			// This behavior is same as VC++ Tips file
			if (fseek(m_pStream, 0, SEEK_SET) != 0) 
				AfxMessageBox(IDS_TOD_FILE_CORRUPT);
		} 
		else 
		{
			if (*lpsz != ' ' && *lpsz != '\t' && 
				*lpsz != '\n' && *lpsz != ';') 
			{
				// There should be no space at the beginning of the tip
				// This behavior is same as VC++ Tips file
				// Comment lines are ignored and they start with a semicolon
				bStop = TRUE;
			}
		}
	}
	strNext.ReleaseBuffer();
}
//------------------------------------------------------------------------
HBRUSH CdlgTip::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd->GetDlgCtrlID() == IDC_TIPSTRING)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CdlgDialog_B::OnCtlColor(pDC, pWnd, nCtlColor);
}
//------------------------------------------------------------------------
void CdlgTip::OnOK()
{
	__super::OnOK();
	
	// Update the startup information stored in the INI file
	SaveSettings();
}
//------------------------------------------------------------------------
BOOL CdlgTip::OnInitDialog()
{
	__super::OnInitDialog();

	// If Tips file does not exist then disable NextTip
	if (m_pStream == NULL)
		GetDlgItem(IDC_NEXTTIP)->EnableWindow(FALSE);
	SET_BUTTON_VIEW(m_buNext);
	SET_BUTTON_VIEW(m_buOK);
	return TRUE;  // return TRUE unless you set the focus to a control
}
//------------------------------------------------------------------------
void CdlgTip::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Get paint area for the big static control
	CRect rect;
	GetDlgItem(IDC_BACKGRND)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Paint the background white.
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	dc.FillRect(rect, &brush);
	rect.DeflateRect(2,2);// border

	// Load bitmap and get dimensions of the bitmap
	CBitmap bmp;
	bmp.LoadBitmap(IDB_LIGHTBULB);
	BITMAP bmpInfo;
	bmp.GetBitmap(&bmpInfo);

	// Draw bitmap in top corner and validate only top portion of window
	CDC dcTmp;
	CRect rectImg(rect);
	dcTmp.CreateCompatibleDC(&dc);
	dcTmp.SelectObject(&bmp);
	rectImg.bottom = bmpInfo.bmHeight + rect.top;
	dc.BitBlt(rectImg.left, rectImg.top, rectImg.Width(), rectImg.Height(), &dcTmp, 0, 0, SRCCOPY);

	// Draw out "Did you know..." message next to the bitmap
	CRect rectDidYouKnow(rectImg);
	CString strMessage;
	strMessage.LoadString(IDS_TOD_DID_YOU_KNOW);	
	rectDidYouKnow.left += bmpInfo.bmWidth;
	dc.DrawText(strMessage, rectDidYouKnow, DT_VCENTER | DT_SINGLELINE);
	//rectDidYouKnow.top+=//dc.GetTextExtent(strMessage).cy;
	dc.MoveTo(rectDidYouKnow.left,rectDidYouKnow.bottom-5);
	dc.LineTo(rectDidYouKnow.right,rectDidYouKnow.bottom-5);
	
	// Вывод текста
	CRect rectText(rect);
	rectText.top =rectDidYouKnow.bottom+15;
	rectText.left=rectDidYouKnow.left ;
	CWnd * pText=GetDlgItem(IDC_TIPSTRING);
	pText->MoveWindow(rectText,false);
}
//------------------------------------------------------------------------
void CdlgTip::LoadSettings()
{
	CWinApp* pApp = AfxGetApp();
	m_bStartup = pApp->GetProfileInt(szSection, szIntStartup, true);
}
//------------------------------------------------------------------------
void CdlgTip::SaveSettings()
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(szSection, szIntStartup, m_bStartup);
}
//------------------------------------------------------------------------
void CdlgTip::OnRunApp()
{
	if ((m_bStartup) && !(ApplicationData::APP_AUTOMATED || ApplicationData::APP_EMBEDDED))
		DoModal();
}

