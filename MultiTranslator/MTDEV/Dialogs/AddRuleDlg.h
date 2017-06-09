/********************************************************************
	created:	2002/07/11
	created:	11:7:2002   13:14
	filename: 	$(Projects)\sources\mtdev\dialogs\addruledlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	addruledlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Диалог для добавления правила в грамматику
*********************************************************************/
#pragma once

#include <ExtControls.h>
#include <xShadeButton.h>

#include "CObjectEx.h" //Один из родителей
#include "MTDevDialog_B.h" //Один из родителей
#include "BaseStructs.h"
#include "afxwin.h"

class CdlgAddRule : public CdlgDialog_B,public CCObjectEx, public IAddRuleLink
{
	DECLARE_DYNAMIC(CdlgAddRule)

	struct rule_data{
		string_std	m_strFileName;
		string_std	m_strName;
		long		m_nStartPos;
		long		m_nEndPos;

		rule_data(cstr szName,cstr szFileName, long nStartPos, long nEndPos):
		m_strName(szName), m_strFileName(szFileName),m_nStartPos(nStartPos),m_nEndPos(nEndPos)
		{}
	};
	typedef vector<rule_data> vector_rule_data;
	vector_rule_data	m_lstRulesLocation;
	int					m_nCurrentlyEditedItem;
	HWND				m_EditorHwnd;

	typedef list<string_smart> variant_contents;
	typedef vector <variant_contents> vector_variants;
	vector_variants m_Variants; //Список для хранения содержимого вариантов

protected:

	// Dialog Data
	enum { IDD = IDD_ADD_RULE };
	enum { ID_ACCEL = IDR_ADD_RULE};

	//{{AFX_DATA(CdlgAddRule)
	CEditEx			m_edRuleName;
	CLabel			m_lblError;
	CComboEx		m_cmbModules;
	CComboEx		m_cmbPlaceRules;
	CShadeButton	m_btnEditTerm;
	CShadeButton	m_btnNewTerm;
	CShadeButton	m_btnMoveTermUp;
	CShadeButton	m_btnMoveTermDown;
	CShadeButton	m_btnDeleteTerm;
	CShadeButton	m_btnAddVariant;
	CShadeButton	m_btnAddToVariant;
	CButton			m_chkIsOptional;
	CButton			m_rdoIsTerm;
	CButton			m_rdoIsUnTerm;
	CTreeCtrlEx		m_ResultTree;
	CEditEx			m_tmpEdt;
	CListBoxEx		m_TermsList;
	CListBoxEx		m_AvailableTermsList;
	CTabCtrl		m_VarTable;
	CString			m_strRuleName;
	CString			m_strModuleName;
	//}}AFX_DATA

	override bool Run();
	override void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	override BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//Обработка циклов простоя
	override void OnIdle();

	void	BuildTree();

	//Выполняет непосредственное добавление правила в редактор
	bool	AddRule();

	// Generated message map functions
	//{{AFX_MSG(CdlgAddRule)
	virtual	BOOL OnInitDialog();
	afx_msg	void OnAddVariant();
	afx_msg	void OnSelchangeVarTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg	void OnDblclkTermsList();
	afx_msg	void OnKillfocusTmpEdt();
	afx_msg	void OnDblclkAvailableTermsList();
	afx_msg	void OnSelchangingVarTab(NMHDR*	pNMHDR,	LRESULT* pResult);
	virtual	void OnOK();
	afx_msg	void OnSelchangeTermsList();
	afx_msg	void OnRadioTerm();
	afx_msg	void OnRadioUnterm();
	afx_msg	void OnEditButton();
	afx_msg	void OnCheckOptional();
	afx_msg void OnCbnSelchangeComboModules();	

	afx_msg	void cmdOnDeleteTerms();
	afx_msg	void cmdOnAddToVariant();
	afx_msg	void cmdOnNewTerm();
	afx_msg	void cmdOnMoveTermUp();
	afx_msg	void cmdOnMoveTermDown();
	//}}AFX_MSG
public:
	CdlgAddRule(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgAddRule();

	DECLARE_MESSAGE_MAP()
};
