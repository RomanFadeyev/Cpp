/********************************************************************
    created:    2002/06/15
    created:    15:6:2002   9:41
    filename:   $(Projects)\sources\mtdev\dialogs\findinfilesdlg.cpp
    file path:  $(Projects)\sources\mtdev\dialogs

    file base:  findinfilesdlg
    file ext:   cpp
    author:     Fadeyev R.V.
    
    purpose:    Диалог поиска текста в файлах
*********************************************************************/

#include "stdafx.h"
#include "resource.h"
#include <BrowseForFolder.h>
#include <GUIService.h>

#include "FindInFilesDlg.h"

#include "Names.h"
#include "BaseStructs.h"
#include "ModuleNames.h"

//Создание переходника для объекта
static i_objects::object_thunk< CdlgFindInFiles > FindInFilesDlg(__MSG_PREFIX__);

// CdlgFindInFiles dialog
IMPLEMENT_DYNAMIC(CdlgFindInFiles, CdlgDialog_B)

BEGIN_MESSAGE_MAP(CdlgFindInFiles, CdlgDialog_B)
	ON_CBN_SELCHANGE(IDC_COMBO_LOOK_IN, OnCbnSelchangeComboLookIn)
END_MESSAGE_MAP()

//Список заранее определенных типов поиска. Используется для вып списка "Look In"
cstr CdlgFindInFiles::mc_szLookIn[lkinHighBound__]={"Current Project", "All Opened Documents", "All Opened Projects", "Other..."};

//------------------------------------------------------------------------
//--------------------- CdlgFindInFiles ----------------------------------
//------------------------------------------------------------------------
CdlgFindInFiles::CdlgFindInFiles(CWnd* pParent /*=NULL*/)
	: CdlgDialog_B(CdlgFindInFiles::IDD, pParent)
	, m_RecentlyText(szFindInFilesRecently,MAX_MRU_ITEMS)
	, m_RecentlyFileTypes(szFindInFilesRecentlyTypes,MAX_MRU_ITEMS)
	, m_strText(_T("")),	m_strLookIn(_T(""))
	, m_strFileTypes(string_smart("*",EXT_SOURCES).c_str())
	, m_bWholeWord(false)
	, m_bMatchCase(false)
	, m_bLookInSubFolders(true)
	, m_nFileTypes(0)
	, m_nLookIn(lkinCurrentProject)
{
}
//------------------------------------------------------------------------
CdlgFindInFiles::~CdlgFindInFiles()
{
}
//------------------------------------------------------------------------
void CdlgFindInFiles::DoDataExchange(CDataExchange* pDX)
{
	CdlgDialog_B::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_FIND, m_strText);
	DDX_CBString(pDX, IDC_COMBO_LOOK_IN, m_strLookIn);
	DDX_CBString(pDX, IDC_COMBO_FILE_TYPES, m_strFileTypes);
	DDX_CBIndex(pDX, IDC_COMBO_FILE_TYPES, m_nFileTypes);
	DDX_CBIndex(pDX, IDC_COMBO_LOOK_IN , m_nLookIn);
	DDX_Check(pDX,IDC_CHECK_MATCH_CASE,m_bMatchCase);
	DDX_Check(pDX,IDC_CHECK_WHOLE_WORD,m_bWholeWord);
	DDX_Check(pDX,IDC_CHECK_LOOK_IN_SUBFOLDERS,m_bLookInSubFolders);
}
//------------------------------------------------------------------------
void CdlgFindInFiles::LoadSettings()
{
	CWinApp	* pApp=AfxGetApp();
	ASSERT_RETURN_VOID(pApp);

	m_strLastCustomPath = pApp->GetProfileString(szFindInFiles,szParamLastDir);
	m_strFileTypes = pApp->GetProfileString(szFindInFiles,szParamFileTypes,m_strFileTypes);
	m_bLookInSubFolders = pApp->GetProfileInt(szFindInFiles,szParamLookInSubFolders,m_bLookInSubFolders);
	m_bWholeWord = pApp->GetProfileInt(szFindInFiles,szParamMatchWholeWord,m_bWholeWord);
	m_bMatchCase = pApp->GetProfileInt(szFindInFiles,szParamMatchCase,m_bMatchCase);
	m_nLookIn	 = pApp->GetProfileInt(szFindInFiles,szParamLookIn,m_nLookIn);
	
	m_RecentlyText.ReadList();
	m_RecentlyFileTypes.ReadList();
}
//------------------------------------------------------------------------
void CdlgFindInFiles::SaveSettings()
{
	CWinApp	* pApp=AfxGetApp();
	ASSERT(pApp);
	pApp->WriteProfileString(szFindInFiles,szParamLastDir,m_strLastCustomPath);
	pApp->WriteProfileString(szFindInFiles,szParamFileTypes,m_strFileTypes);
	pApp->WriteProfileInt(szFindInFiles,szParamLookInSubFolders,m_bLookInSubFolders);
	pApp->WriteProfileInt(szFindInFiles,szParamMatchWholeWord,m_bWholeWord);
	pApp->WriteProfileInt(szFindInFiles,szParamMatchCase,m_bMatchCase);
	pApp->WriteProfileInt(szFindInFiles,szParamLookIn,m_nLookIn);
	
	m_RecentlyText.WriteList();
	m_RecentlyFileTypes.WriteList();
}
//------------------------------------------------------------------------
BOOL CdlgFindInFiles::OnInitDialog()
{
	__super::OnInitDialog();
	
	//Заполняем историю
	CComboBox *pComboText = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_FIND));
	for (size_t i=0; i<m_RecentlyText.Size();i++)
		pComboText->AddString(m_RecentlyText.GetString(i));

	if ((m_RecentlyText.Size()>0) && (m_strText.IsEmpty()))
		pComboText->SetWindowText(m_RecentlyText.GetString(0)); //через m_strText не работает
		
	//Заполняем выпадающий список LookIn
	CComboBox *pComboLookIn = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_LOOK_IN));
	pComboLookIn->ResetContent();
	
	for(int i=0; i<lkinHighBound__; i++)
		if (i==lkinOther)
			pComboLookIn->AddString(string_smart(mc_szLookIn[i],m_strLastCustomPath).c_str());
		else
			pComboLookIn->AddString(mc_szLookIn[i]);
	ASSERT(pComboLookIn->GetCount()>0);
	
	pComboLookIn->SetCurSel(m_nLookIn);

	CComboBox * pComboFileTypes = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_FILE_TYPES));
	for (size_t i=0; i<m_RecentlyFileTypes.Size();i++)
		pComboFileTypes->AddString(m_RecentlyFileTypes.GetString(i));
	pComboFileTypes->SetWindowText(m_strFileTypes); //????
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
}
//------------------------------------------------------------------------
void CdlgFindInFiles::OnOK()
{
	UpdateData();
	if (!m_strText.IsEmpty())
		m_RecentlyText.Add(m_strText);
	if (!m_strFileTypes.IsEmpty())
		m_RecentlyFileTypes.Add(m_strFileTypes);
	__super::OnOK();
}
//------------------------------------------------------------------------
bool CdlgFindInFiles::Run(cstr szFindWhat, IOutputLink * pOutput)
{
	m_strText=szFindWhat;
	bool bRes = DoModal()==IDOK;
	
	//Начинаем процесс поиска
	if ((bRes) && (pOutput))
		DoFind(pOutput);
	return bRes;
}
//------------------------------------------------------------------------
string_smart CdlgFindInFiles::GetActiveProjectPath(){
	IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();
	if (pDocMngrLink){
		IProjectDocLink * pProjectDocLink = pDocMngrLink->GetActiveProject();
		if (pProjectDocLink)
			return extract_filedir(pProjectDocLink->GetPath().c_str());
	}
	return "";
}
//------------------------------------------------------------------------
string_smart CdlgFindInFiles::GetAllProjectsPathes()
{
	IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();	
	ASSERT_RETURN(pDocMngrLink,"");
	
	string_list res;
	IDocMngrLink::doc_iterator it;
	for	(CDocumentEx * pDoc = pDocMngrLink->GetDocumentFirst(it); pDoc; pDoc=pDocMngrLink->GetDocumentNext(it)){
		IProjectDocLink * pProjectDocLink = dynamic_cast<IProjectDocLink*>(pDoc->GetDefaultInterface());
		if (pProjectDocLink)
			res.items().add(extract_filedir(pProjectDocLink->GetPath().c_str()).c_str());
	}
	return res.c_str();
}
//------------------------------------------------------------------------
static void RunShellFind(string_smart strCommandLine, IOutputLink * pOutputLink)
{
	//---------------------------------------------------------------
	//Создаем канал
	SECURITY_ATTRIBUTES sa = {0}; sa.nLength=sizeof(sa); sa.bInheritHandle=true;
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite,&sa,0))
		throw exception(Format("Can't open pipe. Cause: '%s'",string_err(GetLastError()).c_str()).c_str());
	//---------------------------------------------------------------
	//Открываем процесс
	STARTUPINFO si={0};
	si.cb			= sizeof(si);
	si.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow	= SW_HIDE;
	si.hStdOutput	= hWrite;
	si.hStdError	= hWrite;
	PROCESS_INFORMATION pi;

	if (!CreateProcess(NULL,strCommandLine.buffer(),NULL,NULL,true,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi)){
		CloseHandle(hWrite);
		CloseHandle(hRead);
		DWORD dwErr  = GetLastError();
		if (dwErr==ERROR_FILE_NOT_FOUND)
			guiMsgAttention(0,Format(string_res(IDS_CANT_FIND_FILE_s, HRESOURCES).c_str(),module_names::szFindInFilesName).c_str());
		throw exception(Format("Can't create process.  Cause: '%s'",string_err(dwErr).c_str()).c_str());
	}
	CloseHandle(hWrite);
	//---------------------------------------------------------------
	//Читаем результаты
	char  szBuff[1025];DWORD nRead;
	while (ReadFile(hRead,szBuff,1024,&nRead,NULL)){
		szBuff[nRead]=0;
		pOutputLink->Print(string_smart(szBuff).oem2ansi().c_str());
	}
	CloseHandle(hRead);	
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
//------------------------------------------------------------------------
void CdlgFindInFiles::DoFind(IOutputLink * pOutputLink)
{
	Init_Thread_EH();	//поддержка с++	исключений

	LOG_PROTECT_CODE_BEGIN
	pOutputLink->StartSession();

	//CommandLine
	string_smart strCommandLineFirst(
			find_full_filepath(module_names::szFindInFilesName).frames_wrap_on('\"','\"').c_str(),
			_T(" /N"),
			(!m_bMatchCase)			? _T(" /I") : NULL,
			(m_bWholeWord)			? _T(" /???") : NULL,	//Такого ключа нет
			(m_bLookInSubFolders)	? _T(" /S") : NULL,
			" \"",m_strText,"\" ");

	//Теперь нам надо определить путь, где искать
	string_smart strText;
	switch (m_nLookIn){
		case lkinCurrentProject		: strText = GetActiveProjectPath(); break;
		case lkinAllOpenedDocuments	: break;
		case lkinAllOpenedProjects	: strText = GetAllProjectsPathes(); break;
		case lkinOther				: strText = m_strLastCustomPath; break;
	}
	if (strText.isempty())
		strText.cat(_T(".\\"));
	strText = include_backslash(strText.c_str());
		
	string_list strPath(strText.c_str());
	//Теперь нам надо выяснить все расширения
	for (size_t j=0; j<strPath.items().count();j++)
	{
		string_smart strCommandLine = strCommandLineFirst.c_str();
		
		string_list strExts(str_replace(m_strFileTypes,";","\n").c_str());
		for (size_t i=0; i<strExts.items().count();i++)
			strCommandLine.cat(" \"")
				.cat(strPath.items()[j])
				.cat(strExts.items().get_at(i).trim().c_str())
				.cat('\"');
		RunShellFind(strCommandLine,pOutputLink);
	}
	LOG_PROTECT_CODE_END
	
	//Оборачиваем в еще один try, мало ли, может указатели на интерфейсы уже не рабочие?
	LOG_PROTECT_CODE_BEGIN
	if (pOutputLink)
		pOutputLink->StopSession();
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CdlgFindInFiles::OnCbnSelchangeComboLookIn()
{
	//Заполняем выпадающий список LookIn
	CComboBox *pComboLookIn = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_LOOK_IN));
	ASSERT_RETURN_VOID(pComboLookIn);
	if (pComboLookIn->GetCurSel()==lkinOther)	
	{	
		//Класс выдачи окна выбора директории
		CBrowseForFolder	BrowseFolder;
		CString strPath = BrowseFolder.GetFolder(AfxGetMainWnd()->GetSafeHwnd(),"Choose Directory",m_strLastCustomPath);
		if (!strPath.IsEmpty()){
			m_strLastCustomPath=strPath;
			pComboLookIn->DeleteString(lkinOther);
			pComboLookIn->InsertString(lkinOther,string_smart(mc_szLookIn[lkinOther],m_strLastCustomPath).c_str());
			pComboLookIn->SetCurSel(lkinOther);
			SaveSettings();
		}
	}
}
