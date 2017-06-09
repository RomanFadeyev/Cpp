/********************************************************************
    created:    2002/06/15
    created:    15:6:2002   9:27
    filename:   $(Projects)\sources\mtdev\dialogs\finddlg.cpp
    file path:  $(Projects)\sources\mtdev\dialogs

    file base:  finddlg
    file ext:   cpp
    author:     Fadeyev R.V.
    
    purpose:    Диалог поиска текста в текущем документе
*********************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "FindDlg.h"

#include "Names.h"
#include "BaseStructs.h"

//Создание переходника для объекта
static i_objects::object_thunk< CdlgFind > FindObjectThunk(__MSG_PREFIX__);

/////////////////////////////////////////////////////////////////////////////
// CdlgFind dialog

CdlgFind::CdlgFind(CWnd* pParent /*=NULL*/)
    : m_RecentList(szFindRecently,MAX_MRU_ITEMS),
      CdlgDialog_B(CdlgFind::IDD, pParent)
{
    //{{AFX_DATA_INIT(CdlgFind)
	m_bDirDown = 1;
	m_bMatchCase	= FALSE;
	m_bRegexp		= FALSE;
	m_bWholeWord	= FALSE;
	m_bWrapAround	= FALSE;
	m_bInSelection = 1;
	m_strText = _T("");
	//}}AFX_DATA_INIT
}
//------------------------------------------------------------------------
void CdlgFind::DoDataExchange(CDataExchange* pDX)
{
    CdlgDialog_B::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CdlgFind)
	DDX_Radio(pDX, IDC_RADIO_DIR_UP, m_bDirDown);
	DDX_Check(pDX, IDC_CHECK_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_CHECK_REGEXP, m_bRegexp);
	DDX_Check(pDX, IDC_CHECK_WHOLE_WORD, m_bWholeWord);
	DDX_Check(pDX, IDC_CHECK_WRAP_AROUND, m_bWrapAround);
	DDX_Radio(pDX, IDC_RADIO_SCOPE_GLOBAL, m_bInSelection);
	DDX_CBString(pDX, IDC_COMBO_FIND, m_strText);
	//}}AFX_DATA_MAP
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CdlgFind, CdlgDialog_B)
    //{{AFX_MSG_MAP(CdlgFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CdlgFind::OnRunApp()
{
	__super::OnRunApp();
	m_RecentList.ReadList();
}
//------------------------------------------------------------------------
void CdlgFind::OnStopApp()
{
	 m_RecentList.WriteList();
	__super::OnStopApp();
}
//------------------------------------------------------------------------
BOOL CdlgFind::OnInitDialog() 
{
	__super::OnInitDialog();
	
	CComboBox *pComboText = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_FIND));
	for (size_t i=0; i<m_RecentList.Size();i++)
		pComboText->AddString(m_RecentList.GetString(i));

	//Если никакого выделения нет, то и соотв RadioButton тушим 
	CButton *pBtn = static_cast<CButton*>(GetDlgItem(IDC_RADIO_SCOPE_SELECTION));
	pBtn->EnableWindow(m_pCurFindInfo->in_dwInitialSelStart!=m_pCurFindInfo->in_dwInitialSelEnd);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//------------------------------------------------------------------------
void CdlgFind::OnOK() 
{
	UpdateData();

	if (!m_strText.IsEmpty())
		m_RecentList.Add(m_strText);
	
	__super::OnOK();
}
//------------------------------------------------------------------------
bool CdlgFind::Run(vbdFind & Info)
{
	m_pCurFindInfo = &Info;
	
	m_bInSelection = Info.in_bInSelection && (Info.in_dwInitialSelStart!=Info.in_dwInitialSelEnd);
	m_strText = Info.in_strText.c_str();
	if (DoModal()==IDOK)	
	{
		Info.out_bAsRegexp		= (m_bRegexp     == TRUE);
		Info.out_bMatchCase		= (m_bMatchCase  == TRUE);
		Info.out_bWholeWord		= (m_bWholeWord  == TRUE);
		Info.out_bWrapAround	= (m_bWrapAround == TRUE);
		Info.out_bInSelection	= (m_bInSelection== TRUE);
		Info.out_bSearchDown	= (m_bDirDown    == TRUE);
		Info.out_strText		= m_strText;		
		return true;
	}
		
	else
		return false;
}
