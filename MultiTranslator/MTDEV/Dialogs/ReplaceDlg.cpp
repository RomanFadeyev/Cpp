// ReplaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ReplaceDlg.h"
#include "Names.h"

//Создание переходника для объекта
static i_objects::object_thunk< CdlgReplace > ReplaceObject(__MSG_PREFIX__);

BEGIN_MESSAGE_MAP(CdlgReplace, CdlgDialog_B)
    //{{AFX_MSG_MAP(CdlgReplace)
	ON_BN_CLICKED(IDC_FIND, cmdOnFindNext)
	ON_BN_CLICKED(IDC_REPLACE, cmdOnReplace)
	ON_BN_CLICKED(IDC_REPLACE_ALL, cmdOnReplaceAll)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO_SCOPE_GLOBAL, OnBnClickedRadioScope)
	ON_BN_CLICKED(IDC_RADIO_SCOPE_SELECTION, OnBnClickedRadioScope)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgReplace dialog
CdlgReplace::CdlgReplace(CWnd* pParent /*=NULL*/)
    : m_FindMRU(szReplaceRecentlyFind,MAX_MRU_ITEMS),
      m_ReplaceMRU(szReplaceRecentlyReplace,MAX_MRU_ITEMS),
      CdlgDialog_B(CdlgReplace::IDD, pParent)
{
	m_pCurReplaceInfo=NULL;
    //{{AFX_DATA_INIT(CdlgReplace)
	m_strFindText = _T("");
	m_strReplaceText = _T("");
	m_bMatchCase = FALSE;
	m_bRegexp = FALSE;
	m_bInSelection = -1;
	m_bWholeWord = FALSE;
	m_bWrapAround = FALSE;
	//}}AFX_DATA_INIT
}
//------------------------------------------------------------------------
void CdlgReplace::DoDataExchange(CDataExchange* pDX)
{
    CdlgDialog_B::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CdlgReplace)
	DDX_CBString(pDX, IDC_COMBO_FIND, m_strFindText);
	DDX_CBString(pDX, IDC_COMBO_REPLACE, m_strReplaceText);
	DDX_Check(pDX, IDC_CHECK_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_CHECK_REGEXP, m_bRegexp);
	DDX_Radio(pDX, IDC_RADIO_SCOPE_GLOBAL, m_bInSelection);
	DDX_Check(pDX, IDC_CHECK_WHOLE_WORD, m_bWholeWord);
	DDX_Check(pDX, IDC_CHECK_WRAP_AROUND, m_bWrapAround);
	//}}AFX_DATA_MAP
}
//------------------------------------------------------------------------
BOOL CdlgReplace::OnInitDialog()
{
	__super::OnInitDialog();

	CComboBox *pComboText = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_FIND));
	for (size_t i=0; i<m_FindMRU.Size();i++)
		pComboText->AddString(m_FindMRU.GetString(i));

	pComboText = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_REPLACE));
	for (size_t i=0; i<m_ReplaceMRU.Size();i++)
		pComboText->AddString(m_ReplaceMRU.GetString(i));
	

	//Если никакого выделения нет, то и соотв RadioButton тушим 
	CButton *pBtn = static_cast<CButton*>(GetDlgItem(IDC_RADIO_SCOPE_SELECTION));
	pBtn->EnableWindow(m_pCurReplaceInfo->in_dwInitialSelStart!=m_pCurReplaceInfo->in_dwInitialSelEnd);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
//------------------------------------------------------------------------
void CdlgReplace::OnRunApp()
{
	__super::OnRunApp();
	m_FindMRU.ReadList();
	m_ReplaceMRU.ReadList();
}
//------------------------------------------------------------------------
void CdlgReplace::OnStopApp()
{
	 m_FindMRU.WriteList();
	 m_ReplaceMRU.WriteList();
	__super::OnStopApp();
}
//------------------------------------------------------------------------
bool CdlgReplace::Run(vbdReplace & Data)
{
	m_pCurReplaceInfo=&Data;
	_ASSERTE(m_pCurReplaceInfo);
	
	m_bInSelection = (m_pCurReplaceInfo->in_bInSelection) && (m_pCurReplaceInfo->in_dwInitialSelStart!=m_pCurReplaceInfo->in_dwInitialSelEnd);
	m_strFindText = m_pCurReplaceInfo->in_strText.c_str();

	ASSERT(m_pCurReplaceInfo->in_pfCallback.assigned());
	return DoModal()==IDOK;
}
//------------------------------------------------------------------------
void CdlgReplace::PushFindValue()
{
	if (!m_strFindText.IsEmpty())
		m_FindMRU.Add(m_strFindText);
}
//------------------------------------------------------------------------
void CdlgReplace::PushReplaceValue()
{
	if (!m_strReplaceText.IsEmpty())
		m_ReplaceMRU.Add(m_strReplaceText);
}
//------------------------------------------------------------------------
void CdlgReplace::PerformCallback(IReplaceTextLink::vbdReplace::operations lOp)
{
	ASSERT(m_pCurReplaceInfo);
	UpdateData();
	PushFindValue();
	PushReplaceValue();
	LOG_PROTECT_CODE_BEGIN
		m_pCurReplaceInfo->out_strText		= m_strFindText;
		m_pCurReplaceInfo->out_strReplaceText=m_strReplaceText;	
		m_pCurReplaceInfo->out_bAsRegexp	= (m_bRegexp     == TRUE);
		m_pCurReplaceInfo->out_bMatchCase	= (m_bMatchCase  == TRUE);
		m_pCurReplaceInfo->out_bWholeWord	= (m_bWholeWord  == TRUE);
		m_pCurReplaceInfo->out_bWrapAround	= (m_bWrapAround == TRUE);
		m_pCurReplaceInfo->out_bInSelection	= (m_bInSelection== TRUE);
		m_pCurReplaceInfo->out_bSearchDown	= true;
		m_pCurReplaceInfo->in_pfCallback(lOp,this->m_hWnd);
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CdlgReplace::OnBnClickedRadioScope()
{
	UpdateData();
	CButton *pBtn = static_cast<CButton*>(GetDlgItem(IDC_FIND));
	pBtn->EnableWindow(!m_bInSelection);
	pBtn = static_cast<CButton*>(GetDlgItem(IDC_REPLACE));
	pBtn->EnableWindow(!m_bInSelection);
}
//------------------------------------------------------------------------
void CdlgReplace::cmdOnFindNext() 
{
	PerformCallback(vbdReplace::opFindNext);
}
//------------------------------------------------------------------------
void CdlgReplace::cmdOnReplace() 
{
	PerformCallback(vbdReplace::opReplace);
}
//------------------------------------------------------------------------
void CdlgReplace::cmdOnReplaceAll() 
{
	PerformCallback(vbdReplace::opReplaceAll);
}
