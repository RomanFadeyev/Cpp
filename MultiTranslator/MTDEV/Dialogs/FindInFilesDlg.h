/********************************************************************
    created:    2002/06/15
    created:    15:6:2002   9:28
    filename:   $(Projects)\sources\mtdev\dialogs\findinfilesdlg.h
    file path:  $(Projects)\sources\mtdev\dialogs

    file base:  findinfilesdlg
    file ext:   h
    author:     Fadeyev R.V.
    
    purpose:    Диалог поиска текста в файлах
*********************************************************************/
#pragma once

#include <MfcUtils.h>
#include "CObjectEx.h" //Один из родителей
#include "MTDevDialog_B.h"	//Один из родителей
#include "BaseStructs.h"

// CdlgFindInFiles dialog
class CdlgFindInFiles : public CdlgDialog_B,public CCObjectEx, public IFindInFilesLink
{
	DECLARE_DYNAMIC(CdlgFindInFiles)
private:
	CString m_strText;			// Переменная для отображения текста контрола IDC_COMBO_FIND 
	CString m_strLookIn;		// IDC_COMBO_LOOK_IN
	CString m_strFileTypes;		// IDC_COMBO_FILE_TYPES (string)
	BOOL m_bWholeWord;			// IDC_CHECK_WHOLE_WORD
	BOOL m_bMatchCase;			// IDC_CHECK_MATCH_CASE
	BOOL m_bLookInSubFolders;	// IDC_CHECK_LOOK_IN_SUBFOLDERS
	INT	 m_nFileTypes;			// IDC_COMBO_FILE_TYPES (index)
	INT	 m_nLookIn;				// IDC_COMBO_LOOK_IN (index)

	// Список применяемых строк для поиска (история)
	mfc_utils::CRecentlyList	m_RecentlyText;
	mfc_utils::CRecentlyList	m_RecentlyFileTypes;
	// Переменная хранит последнее значение директории по выбору
	CString m_strLastCustomPath;

	enum { IDD = IDD_FIND_IN_FILES };
	//Список заранее определенных типов поиска. Используется для вып списка "Look In"
	enum {lkinCurrentProject,lkinAllOpenedDocuments,lkinAllOpenedProjects,lkinOther,lkinHighBound__};
	static cstr mc_szLookIn[lkinHighBound__];
	
	//Дает путь к текущему активному проекту
	string_smart GetActiveProjectPath();
	string_smart GetAllProjectsPathes();
	//Запускает процесс поиска текста в указанных файлах
	void DoFind(IOutputLink * pOutput);

protected:
	//Работа с загрузкой/сохранением параметров в реестре
	override void LoadSettings();
	override void SaveSettings();

	// From IFindInFilesLink
	override bool Run(cstr szFindWhat, IOutputLink * pOutput);
	// From CdlgDialog_B
	override void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	override BOOL OnInitDialog();
	override void OnOK();

	afx_msg void OnCbnSelchangeComboLookIn();
	DECLARE_MESSAGE_MAP()
	
public:
	CdlgFindInFiles(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgFindInFiles();
};
