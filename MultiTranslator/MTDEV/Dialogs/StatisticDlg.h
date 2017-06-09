/********************************************************************
	created:	2002/09/19
	created:	19:9:2002   13:39
	filename: 	$(Projects)\sources\mtdev\dialogs\statisticdlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	statisticdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	������ ����������� ������ �� ������ ���������� �� ����� *.sct
	
*********************************************************************/

#pragma once
//
#include <ExtControls.h>
#include "BaseStructs.h"
#include "Names.h"
#include "CObjectEx.h"		//���� �� ���������
#include "MTDevDialog_B.h"	//���� �� ���������
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CdlgStatistic dialog
class CdlgStatistic : public CdlgDialog_B, public CCObjectEx,public IStatisticLink
{
public:
	CdlgStatistic(CWnd* pParent = NULL);   // standard constructor
	
protected:
    //��������� - ��������� ��� ������� ��������. 
    struct CVariantInfo{
        string_smart    m_strName;
        size_t			m_nPos;             //�������
        size_t			m_nNewPos;			//������������ ������ ��� ���������
        //�����������
        CVariantInfo(cstr szName, size_t nPos, size_t nNewPos):
            m_strName(szName), m_nPos(nPos), m_nNewPos(nNewPos)
        {}
     };

    struct CRuleInfo{
		enum ERuleState {rsReset, rsPassed,rsFailed};
        HTREEITEM       m_hItem1;	//���������� �������� � m_RulesTree
		HTREEITEM       m_hItem2;   //���������� �������� � m_RulesTreeOptmz
        string_smart    m_strName;
        bool			m_bNeedOptimize;
        vector<CVariantInfo> vars_list;
        //�����������
        CRuleInfo(HTREEITEM hItem1, HTREEITEM hItem2, cstr szName, bool bNeedOptimize):
            m_hItem1(hItem1),m_hItem2(hItem2),m_strName(szName),m_bNeedOptimize(bNeedOptimize)
        {}
     };
    
	void Message(cstr szMsg); 
	void SetRuleState(CRuleInfo & Rule, CRuleInfo::ERuleState nState);


	//{{AFX_DATA(CdlgStatistic)
	enum { IDD = IDD_STATISTIC };
	CListBox	m_Messages;
	CProgressCtrl	m_OptimizeProgress;
	CTreeCtrlEx		m_RulesTreeOptmz;
	CTreeCtrlEx		m_RulesTree;
	//}}AFX_DATA

	IProjectDocLink * m_pProjectLink;
	CImageList	m_ImgLst;
	
	typedef vector<CRuleInfo> rules_list;
	rules_list  m_Rules;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	//From IStatisticLink
	override bool Run(IProjectDocLink * pProjectLink);

	// Generated message map functions
	//{{AFX_MSG(CdlgStatistic)
	afx_msg void OnButtonOptimize();
	afx_msg void OnDestroy();
	afx_msg void OnItemExpandedTreeRules(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnItemExpandedTreeRulesOpt(NMHDR *pNMHDR, LRESULT *pResult);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:

};

