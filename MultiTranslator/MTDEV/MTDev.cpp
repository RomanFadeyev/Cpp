/********************************************************************
	created:	2003/02/14
	created:	14:2:2003   15:55
	filename: 	$(Projects)\sources\mtdev\mtdev.cpp
	file path:	$(Projects)\sources\mtdev
	file base:	mtdev
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	MTDev.cpp : Defines the class behaviors for the application.
*********************************************************************/

#include "stdafx.h"
#include "MTDev.h"
#include <AppUtils.h>
#include <BaseUtils.h>
#include <MiscUtils.h>  
#include <GUIService.h>  
#include <ModuleNames.h>
#include <initguid.h>	//COM Support

#include "MainFrm.h"
#include "Names.h"
#include "EditionRestrictions.h"

using namespace i_objects;
using namespace base_utils;
using namespace misc_utils;
using namespace ApplicationData;

/////////////////////////////////////////////////////////////////////////////
// ATL Support
[ module(unspecified, uuid = "{B5708153-CD7A-416D-9022-D46E486C53F0}", helpstring = "MultiTranslator IDE Type Library", name = "mtdev") ];
class CMTDevModule :public CAtlMfcModule
{
public:
	static CLSID _Get_Lib_CLSID_() { 
		CLSID res; CLSIDFromString(L"{B5708153-CD7A-416D-9022-D46E486C53F0}",&res);
	}
	override LONG Unlock()
	{
		if (GetLockCount()<=1)
			AfxOleSetUserCtrl(TRUE);    // avoid auto-close
		return __super::Unlock();
	}
	DECLARE_LIBID(_Get_Lib_CLSID_());
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MTDEV, "{81C69FDC-C143-4DB0-ACFD-AD7DAF5AAF15}");
};
CMTDevModule _AtlModule;

/////////////////////////////////////////////////////////////////////////////
// CMTDevApp
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMTDevApp, CWinApp)
    //{{AFX_MSG_MAP(CMTDevApp)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_PROJECT1, ID_FILE_MRU_PROJECT16, cmdOnRecentProject)
	ON_COMMAND_EX_RANGE(ID_FILE_NEW_DOC1, ID_FILE_NEW_DOC16, cmdOnNewDoc)	
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_PROJECT1, cmdupOnRecentProjects)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_DOC1, cmdupOnNewDocs)	
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMTDevApp construction

#pragma warning( disable : 4355) //using "this"in member intialization list
CMTDevApp::CMTDevApp():m_pRecentProjectList(NULL),
	m_nSupressMBCount(0),m_pCoClass(NULL), m_bRunEventRaised(false)
{
}

CMTDevApp::~CMTDevApp()
{
	free_and_null(m_pRecentProjectList);
}

/////////////////////////////////////////////////////////////////////////////

CMTDevApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMTDevApp initialization

BOOL CMTDevApp::InitInstance()
{
	if (!__super ::InitInstance())
		return FALSE;

	m_nCmdShow=SW_SHOWMAXIMIZED;

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	
	//Initialize OLE Container
	AfxEnableControlContainer();

	// Change the registry key under which our settings	are	stored.
	// TODO: You should	modify this	string to be something appropriate
	// such	as the name	of your	company	or organization.
	SetRegistryKey();
	SetRegistryBase	(szProfileUserInterface);

	//Инициализация	основных данных	о программе
	LoadApplicationInfo();

	// Командная строка
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand	= CCommandLineInfo::FileNothing;//Убираем автооткрытие документа
	ParseCommandLine(cmdInfo);

	//---- Регистрация сервера
	// Register	class factories	via	CoRegisterClassObject().
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER,	REGCLS_MULTIPLEUSE)))
		return FALSE;

	//Регистрируем себя	всегда
	_AtlModule.UpdateRegistryAppId(TRUE);
	_AtlModule.RegisterServer(TRUE);

	// App was launched	with /Unregserver or /Unregister switch.
	if (cmdInfo.m_nShellCommand	== CCommandLineInfo::AppUnregister)
	{
		_AtlModule.UpdateRegistryAppId(FALSE);
		_AtlModule.UnregisterServer(TRUE);
		return FALSE;	//Здесь выходим!!
	}

	//Теперь инициализируем библиотеку. Это можно делать только после вызова SetRegistryKey
	if (!LoadLibraries())
		return false;

	//Проверка ключей командной строки
	string_list lstCmdLineInfo;
	lstCmdLineInfo.load_from_cmd_line(GetCommandLine());
	//cr = clear registry 
	if ((lstCmdLineInfo.items().index_of(_T("/cr"))!=-1) || 
		((lstCmdLineInfo.items().index_of(_T("-cr"))!=-1)))
	{
		ASSERT(m_pszProfileName);
		string_smart strKey (_T("Software\\"),m_pszRegistryKey,_T("\\"),m_pszProfileName);
		DelRegTree(HKEY_CURRENT_USER, strKey.c_str());
	}
	//Зачистка отдельных данных в реестре от старых версий
	else {
		misc_utils::version_info info(NULL);
		string_smart strCV(info.get_FileVersion().c_str()," - ",info.get_FileBuild().c_str());
		string_smart strPV(GetProfileString("",_T("Version Passport")));
		//Зачищаем interface
		if (strPV!=strCV){
			ASSERT(m_pszProfileName);
			string_smart strKey (_T("Software\\"),m_pszRegistryKey,_T("\\"),m_pszProfileName,_T("\\"),szProfileUserInterface);
			DelRegTree(HKEY_CURRENT_USER, strKey.c_str());
			WriteProfileString("",_T("Version Passport"),strCV.c_str());
		}
	}

	//Если пользователь хочет получить информацию о ключах
	if ((lstCmdLineInfo.items().index_of(_T("/?"))!=-1) || 
		((lstCmdLineInfo.items().index_of(_T("/h"))!=-1))||
		((lstCmdLineInfo.items().index_of(_T("-h"))!=-1)))
	{
		string_list s;
		s.items().add("Available switches:\n");
		s.items().add(string_smart("/Register - register all program components, filetypes etc."));
		s.items().add(string_smart("/Unregister - unregister filetypes, OLE server etc."));
		s.items().add(string_smart("/cr - clear registry settings."));
		s.items().add(string_smart("/ns - do not show splash screen."));
		guiMsgInformation(0,s.c_str());
		return false;
	}

	//=================	Проверка активизации программы ===================
	bool bJustActivated=false;
	{	//Обязательно локально
		mtdev_lib::IProductActivationPtr spProductActivation(__uuidof(mtdev_lib::ProductActivation));
		if (!spProductActivation->Activated){
			if (!spProductActivation->Show())
				return false;
			bJustActivated = true;
		}
	}

	//Нужно заново инициализировать библиотеку, так как после активации
	//ее нужно выгрузить и заново загрузить
	if (bJustActivated)
		if (!LoadLibraries())
			return false;

	//Если нас просто попросили загегистрироваться - выходим
	if (cmdInfo.m_nShellCommand==CCommandLineInfo::AppRegister)
		return false;

	//=====================	Фактический	старт программы	==================

	//Создаем объекты, которые не могли	быть созданы до	InitInstance
	//и	были офорлены через	проводники
	i_objects::CreateThunkObjects();

	//Событие старта программы
	i_objects::event<ARGS_0> evCreateApp (IApplicationLink::IEventSink::evOnCreateApp);

	// Initialize all Managers for usage. They are automatically constructed if	not	yet	present
	InitMouseManager();	
	InitContextMenuManager();
	InitKeyboardManager();

	IStorageLink * pStorageLink = singleton_cache::GetStorageLink();
	ASSERT_RETURN_FALSE(pStorageLink); //Без этого работать нет никакого смысла.

	BEGIN_LIMITED_EDITION
	if (pStorageLink->General_TearOffMenus)
		EnableTearOffMenus (_T("Tear-Off"), ID_TEAR_OFF1, ID_TEAR_OFF29);  
	END_LIMITED_EDITION

	//Режим авто сохранения
	m_bAutoSaveFiles = pStorageLink->General_AutosaveFiles!=FALSE;
	m_nAutoSaveNMinutes = pStorageLink->General_AutosaveEachNMinutes;

	//Инициализируем Skins
	#ifndef _BCGCONTROLBAR_STATIC_
		string_smart strSkinDirectory=get_module_filedir().cat(module_names::szFolderSkins);
		force_dirs(strSkinDirectory.c_str());
		InitSkinManager	(strSkinDirectory.c_str());
	#endif
 
	EnableUserTools	(ID_TOOLS_ENTRY, ID_USER_TOOL1,	ID_USER_TOOL10,	
					RUNTIME_CLASS (CBCGUserTool), IDR_MENU_ARGS, IDR_MENU_DIRS);

	//К этому моменту уже все типы документов должны быть зарегистрированы	
	RegisterShellFileTypes(true);

	//Мелочи
	LoadMRU();  // Load standard	INI	file options (including	MRU)


	if (!cmdInfo.m_strFileName.IsEmpty() &&	(cmdInfo.m_nShellCommand ==	CCommandLineInfo::FileNothing))	
		cmdInfo.m_nShellCommand	= CCommandLineInfo::FileOpen;

	// create main MDI Frame window
	CMainFrame*	pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	m_sdDevLibraryInfo->MainWindow = (OLE_HANDLE)pMainFrame->m_hWnd;
	m_pMainWnd = pMainFrame;

	//Может, кому интересно знать, что главное окно создано
	i_objects::event<ARGS_0> evCreateAppMainWnd (IApplicationLink::IEventSink::evOnCreateAppMainWnd);

	// Dispatch	commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE; 

	// App was launched	with /Embedding	or /Automation switch.
	// Run app as automation server.
	if (ApplicationData::APP_AUTOMATED || ApplicationData::APP_EMBEDDED){
		// We Don't show the main window here
		#ifdef _DEBUG
			DebugBreak();
		#endif
	}
	else{
		// The main	window has been	initialized, so	show and update	it.
		pMainFrame->ShowWindow(m_nCmdShow);
		pMainFrame->UpdateWindow();
	}

	i_objects::event<ARGS_0> evRunApp (IApplicationLink::IEventSink::evOnRunApp);
	m_bRunEventRaised = true;

	//Если пользователь не открыл файл с командной строки, то ...
	if (cmdInfo.m_nShellCommand==CCommandLineInfo::FileNothing)
	{
		//Что будем показывать
		if (pStorageLink){
			switch(pStorageLink->General_StartupAction) 
			{
			case mtdev_lib::LoadLastProject:
				if ((m_pRecentProjectList!=NULL) && 
					(m_pRecentProjectList->GetSize()>0))
				{
					CString strPath = (*m_pRecentProjectList)[0];
					if (is_file_exist(strPath))
						OpenDocumentFile(strPath);
				}
				break;
			case mtdev_lib::ShowOpenProjectDialog:
				m_pMainWnd->SendMessage(WM_COMMAND,ID_FILE_PROJECT_OPEN,0);
				break;
			case mtdev_lib::ShowNewProjectDialog:
				m_pMainWnd->SendMessage(WM_COMMAND,ID_FILE_NEW,0);
				break;
			}
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------
bool CMTDevApp::LoadLibraries()
{
	if (m_sdDevLibraryInfo)
		m_sdDevLibraryInfo.Release();
	
	CoFreeUnusedLibraries();

	//библиотека - основная
	LOG_PROTECT_CODE_BEGIN
	if (!CreateInterfaceInstance(m_sdDevLibraryInfo,__uuidof(mtdev_lib::LibraryInfo),module_names::szMTDevLibName))
		return false;

	//Теперь инициализируем библиотеку. Это можно делать только после вызова SetRegistryKey
	m_sdDevLibraryInfo->RegistryKey=ApplicationData::APP_REG_KEY;
	m_sdDevLibraryInfo->SupportMail = ApplicationData::APP_EMAIL;
	m_sdDevLibraryInfo->CompanyName = ApplicationData::APP_COMPANY;
	m_sdDevLibraryInfo->ProductName = ApplicationData::APP_NAME;
	m_sdDevLibraryInfo->ProductVersion = ApplicationData::APP_PRODUCT_VERSION;
	m_sdDevLibraryInfo->AppIcon = (OLE_HANDLE)AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sdDevLibraryInfo->AppInstance = (OLE_HANDLE) AfxGetApp()->m_hInstance;

	return true;
	LOG_PROTECT_CODE_END
	
	return false;
}
//------------------------------------------------------------------------
int	CMTDevApp::ExitInstance() 
{
	_AtlModule.RevokeClassObjects();
	if (m_bRunEventRaised)
		i_objects::event<ARGS_0> evStopApp (IApplicationLink::IEventSink::evOnStopApp);
	i_objects::event<ARGS_0> evDestroyApp (IApplicationLink::IEventSink::evOnDestroyApp);

	BCGCBCleanUp ();
	//Удаляем объекты, которые создавались переходниками
	i_objects::DestroyThunkObjects();

	base_utils::cleanup();
	return CWinApp::ExitInstance();
}
//------------------------------------------------------------------------
void CMTDevApp::SetRegistryKey()
{
	BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	free((void*)m_pszRegistryKey);
	m_pszRegistryKey = _tcsdup(APP_COMPANY);
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(string_smart(APP_NAME,	"\\",APP_VERSION).c_str());
	AfxEnableMemoryTracking(bEnable);
}
//------------------------------------------------------------------------
void CMTDevApp::LoadApplicationInfo()
{
	version_info info(NULL);
	strcpy(ApplicationData::APP_NAME,		info.get_ProductName().c_str());
	strcpy(ApplicationData::APP_COMMENT,	info.get_ProductComment().c_str());	   
	strcpy(ApplicationData::APP_COMPANY,	info.get_CompanyName().c_str());
	
	string_smart s = info.get_ProductVersion(); s.replace(",",".");
	string_list	 strProductVersion(str_split_text(s,"."));
	string_smart strProductVersion2(strProductVersion.items().get_at(0).trim(),".",strProductVersion.items().get_at(1).trim());
	strcpy(ApplicationData::APP_PRODUCT_VERSION,strProductVersion2.c_str());

	strcpy(ApplicationData::APP_COPYRIGHT,	info.get_LegalCopyright().c_str());
	strcpy(ApplicationData::APP_SIGNATURE,	info.get_InternalName().c_str());
	strcpy(ApplicationData::APP_VERSION,	info.get_FileVersion().c_str());

	strcpy(ApplicationData::APP_DIR,		get_module_filedir().c_str());
	strcpy(ApplicationData::APP_LOG_FILENAME,
											string_smart(	ApplicationData::APP_DIR,
														extract_filename(get_module_filename().c_str()).c_str(),
														".log").c_str());
			
	strcpy(ApplicationData::APP_AUTHORS,	PRODUCT_AUTHORS);	
	strcpy(ApplicationData::APP_EMAIL,		COMPANY_EMAIL);
	strcpy(ApplicationData::APP_URL,		COMPANY_URL);
	strcpy(ApplicationData::APP_REG_KEY, string_smart("Software\\",m_pszRegistryKey,"\\",m_pszProfileName).c_str());

	// Parse command line for standard shell commands, DDE,	file open
	CCommandLineInfo cmdInfo;ParseCommandLine(cmdInfo);
	ApplicationData::APP_AUTOMATED = cmdInfo.m_bRunAutomated!=FALSE;
	ApplicationData::APP_EMBEDDED = cmdInfo.m_bRunEmbedded!=FALSE;
} 

/////////////////////////////////////////////////////////////////////////////
// Customization load/save methods

void CMTDevApp::PreLoadState ()
{
	GetMouseManager()->AddView (IDR_DOC_SOURCE,	_T("Source view"), IDR_DOC_SOURCE);
	// TODO: add another views and windows were	mouse double click
	// customization is	required

	GetContextMenuManager()->AddMenu (_T("My menu"), IDR_CONTEXT_MENU);
	
	GetContextMenuManager()->AddMenu (_T("Project popup menu in the Class View"), IDR_POPUP_CLW_PROJECT);
	GetContextMenuManager()->AddMenu (_T("Rules popup menu in the Class View"), IDR_POPUP_CLW_RULES);
	GetContextMenuManager()->AddMenu (_T("Functions popup menu in the Class View"), IDR_POPUP_CLW_FUNCS);
	GetContextMenuManager()->AddMenu (_T("Structures popup menu in the Class View"), IDR_POPUP_CLW_STRUCTS);
	GetContextMenuManager()->AddMenu (_T("Recently used rules popup menu in the Class View"), IDR_POPUP_CLW_RULES_RECENTLY);
	GetContextMenuManager()->AddMenu (_T("Rule popup menu in the Class View"), IDR_POPUP_CLW_RULE);
	GetContextMenuManager()->AddMenu (_T("Variant popup menu in the Class View"), IDR_POPUP_CLW_VARIANT);
	GetContextMenuManager()->AddMenu (_T("User folder popup menu in the Class View"), IDR_POPUP_CLW_USER_FOLDER);
	GetContextMenuManager()->AddMenu (_T("Class View popup menu"), IDR_POPUP_CLW_BASE);		
	GetContextMenuManager()->AddMenu (_T("Files popup menu in the File View"), IDR_POPUP_FLW_SOURCES);
	GetContextMenuManager()->AddMenu (_T("File popup menu in the File View"), IDR_POPUP_FLW_SOURCE);
	GetContextMenuManager()->AddMenu (_T("Project popup menu in the File View"), IDR_POPUP_FLW_PROJECT);
	GetContextMenuManager()->AddMenu (_T("File View menu"), IDR_POPUP_FLW_BASE);			
	
	GetContextMenuManager()->AddMenu (_T("Main Window Area menu"), IDR_POPUP_MAIN_AREA);
	GetContextMenuManager()->AddMenu (_T("MDI Tabs menu"), IDR_POPUP_MDI_TABS);
	GetContextMenuManager()->AddMenu (_T("Source View"),IDR_POPUP_DOC_SOURCE);
	GetContextMenuManager()->AddMenu (_T("Source View Margin"),IDR_POPUP_DOC_SOURCE_MARGIN);
	GetContextMenuManager()->AddMenu (_T("Output Screen	menu"),IDR_POPUP_OUTPUT_SCREEN);
	GetContextMenuManager()->AddMenu (_T("Breakpoints menu"),IDR_POPUP_BREAKPOINTS);	
	GetContextMenuManager()->AddMenu (_T("Output menu"),IDR_POPUP_OUTPUT);
}
//------------------------------------------------------------------------
void CMTDevApp::LoadCustomState	()
{
	BOOL bControlBarCaption	= GetInt (_T("ControlBarCaption"));
	BOOL bGradientCaption =	bControlBarCaption ? GetInt	(_T("GradientCaption"))	: FALSE;

	CBCGSizingControlBar::SetCaptionStyle (bControlBarCaption, bGradientCaption);
 }
//------------------------------------------------------------------------
void CMTDevApp::SaveCustomState	()
{
	WriteInt (_T("ControlBarCaption"), CBCGSizingControlBar::IsDrawCaption ());
	WriteInt (_T("GradientCaption"), CBCGSizingControlBar::IsCaptionGradient ());
	if (m_pRecentProjectList)
		m_pRecentProjectList->WriteList();
}
//------------------------------------------------------------------------
bool CMTDevApp::Quit(bool bForcedClose/*=false*/)
{
	if (AfxGetMainWnd()==NULL)
		return true;

	//"Нормальный" выход
	ASSERT(m_pMainWnd != NULL);
	m_pMainWnd->SendMessage(WM_CLOSE);
	bool bResult=m_pMainWnd==NULL;

	//Принудительное закрытие
	if ((!bResult) && (bForcedClose)){
		CloseAllDocuments(true);
		delete m_pMainWnd;
		m_pMainWnd=NULL;
	}

	return true;
}
//------------------------------------------------------------------------
BOOL CMTDevApp::PumpMessage()
{
	LOG_PROTECT_CODE_BEGIN
	return __super::PumpMessage();
	LOG_PROTECT_CODE_END
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMTDevApp::OnIdle(LONG lCount)
{
	// -- Auto-Save --
	LOG_PROTECT_CODE_BEGIN
	if (lCount <= 0)
	{
		if (m_bAutoSaveFiles)
		{
			__time64_t tNow = _time64(NULL);

			IDocMngrLink * pDocMngrLink	= singleton_cache::GetDocMngrLink();	
			ASSERT(pDocMngrLink);

			IDocMngrLink::doc_iterator it;
			for	(CDocumentEx * pDoc = pDocMngrLink->GetDocumentFirst(it); pDoc; pDoc=pDocMngrLink->GetDocumentNext(it))
			{
				//Если файл доступен для записи, модифицирован и не новый, попытаемся 
				//его сохранить
				if ((!pDoc->IsReadOnly()) && (pDoc->IsModified()) && (!pDoc->GetPathName().IsEmpty())){
					__time64_t tLast = pDoc->GetLastSaveAttemptTime();
					if (tLast!=0)	//Это похоже на сбой
						if (tNow-tLast>m_nAutoSaveNMinutes*60)
						{
							pDoc->OnSaveDocument(pDoc->GetPathName());
							singleton_cache::GetMainFrameLink()->SetStatusMessage(string_res(IDS_AUTOSAVE_COMPLETED,HRESOURCES));
							return FALSE;
						}
				}
			}
		}
	}
	LOG_PROTECT_CODE_END

	BOOL res = __super::OnIdle(lCount); 

	if (res){
		bool bStopIdle = false;
		for (list_idle_handlers::iterator i = m_lstIdleHandlerList.begin();(!bStopIdle) && (i!=m_lstIdleHandlerList.end());++i)
			(*i)->OnIdle(bStopIdle);
	}
	
	return res;
}
//------------------------------------------------------------------------
void CMTDevApp::OnStorageDataChanged(IStorageLink::EParam nParameterChanged)
{
	//Длина MRU file list
	//Видимость/невидимость вертикальных линий 
	if (nParameterChanged == IStorageLink::pmGeneral_MRUSize)
	{
		LoadMRU();
	}
	else if (oneof(nParameterChanged,IStorageLink::pmGeneral_AutosaveFiles,IStorageLink::pmGeneral_AutosaveEachNMinutes))
	{
		IStorageLink * pStorageLink = singleton_cache::GetStorageLink();
		ASSERT_RETURN_VOID (pStorageLink);
		m_bAutoSaveFiles = pStorageLink->General_AutosaveFiles!=FALSE;
		m_nAutoSaveNMinutes = pStorageLink->General_AutosaveEachNMinutes;
	}
}
//------------------------------------------------------------------------
void CMTDevApp::SupressMessageBoxes(bool bSupress){
	if (bSupress)	m_nSupressMBCount++;
	else			m_nSupressMBCount--;
	_ASSERT(m_nSupressMBCount>=0);
	m_nSupressMBCount=max(m_nSupressMBCount,0);
}
//------------------------------------------------------------------------
ref_cnt_ptr<COM::IApplication> CMTDevApp::GetComInterface()
{
	if (!m_pCoClass)
	{
		m_pCoClass = new CComObject<COM::Application>;
		m_pCoClass->LinkToOwner(this);
	}
	return m_pCoClass;
}
//------------------------------------------------------------------------
registry CMTDevApp::GetAppVersionBasedRegistryKey()
{
	return CWinApp::GetAppRegistryKey();
}
//------------------------------------------------------------------------
registry CMTDevApp::GetAppSteadyRegistryKey()
{
	_ASSERT(m_pszRegistryKey != NULL);
	_ASSERT(m_pszProfileName != NULL);

	//Запоминаем старое значение
	string_smart strOldProfileName = m_pszProfileName; 
	free((void*)m_pszProfileName); m_pszProfileName = _tcsdup(APP_NAME);

	//читаем
	HKEY result  = NULL;
	LOG_PROTECT_CODE_BEGIN
	result= CWinApp::GetAppRegistryKey();
	LOG_PROTECT_CODE_END
	
	//Восстанавливаем как было
	free((void*)m_pszProfileName); m_pszProfileName = _tcsdup(strOldProfileName.c_str());
	return result;
}
//------------------------------------------------------------------------
void CMTDevApp::LoadMRU()
{
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);

	free_and_null(m_pRecentProjectList);
	free_and_null(m_pRecentFileList);

	//Считаем, сколько ж разрешено MaxMRU
	UINT nMaxMRU = pStorage->General_MRUSize;
	BOOL bNoRecentDocs = FALSE; GetSysPolicyValue(_AFX_SYSPOLICY_NORECENTDOCHISTORY, &bNoRecentDocs);
	if (bNoRecentDocs)
		nMaxMRU=0;

	//Заполняем Project MRU ---
	if (nMaxMRU!=0){
		m_pRecentProjectList = new CRecentFileList(0, szProfileRecentProjects,szParamRecentProjectName, nMaxMRU);
		m_pRecentProjectList->ReadList();
	}
	
	//Заполняем Documents MRU ---
	__super::LoadStdProfileSettings(nMaxMRU);
}
//------------------------------------------------------------------------
void CMTDevApp::AddToRecentFileList(LPCTSTR	lpszPathName)
{
	if (is_file_of_group(lpszPathName,EXT_PROJECTS)) 
	{
		if (m_pRecentProjectList){
			m_pRecentProjectList->Add(lpszPathName);
			m_pRecentProjectList->WriteList();
		}
	}
	else
		__super::AddToRecentFileList(lpszPathName);
}
//------------------------------------------------------------------------
int CMTDevApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	if (m_nSupressMBCount>0)
		return 0;
	
	return __super::DoMessageBox(lpszPrompt,nType,nIDPrompt);
}
//------------------------------------------------------------------------
bool CMTDevApp::IncludeToOnIdleMap(IIdleHandler* pHandler)
{
	_ASSERT(find(m_lstIdleHandlerList.begin(),m_lstIdleHandlerList.end(),pHandler)==m_lstIdleHandlerList.end());
	m_lstIdleHandlerList.push_back(pHandler);
	return true;
}
//------------------------------------------------------------------------
bool CMTDevApp::ExcludeFromOnIdleMap(IIdleHandler* pHandler)
{
	_ASSERT(find(m_lstIdleHandlerList.begin(),m_lstIdleHandlerList.end(),pHandler)!=m_lstIdleHandlerList.end());
	m_lstIdleHandlerList.remove(pHandler);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CMTDevApp message handlers
BOOL CMTDevApp::cmdOnRecentProject(UINT	nID)
{
	if (!m_pRecentProjectList)
		return FALSE;
	
	ASSERT((nID	>= ID_FILE_MRU_PROJECT1) &&	(nID < ID_FILE_MRU_PROJECT1	+ (UINT)m_pRecentProjectList->GetSize()));
	int	nIndex = nID - ID_FILE_MRU_PROJECT1;

	if (OpenDocumentFile((*m_pRecentProjectList)[nIndex]) == NULL)
		m_pRecentProjectList->Remove(nIndex);

	return TRUE;
}
//------------------------------------------------------------------------
void CMTDevApp::cmdupOnRecentProjects(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pRecentProjectList!=NULL);
	if (!m_pRecentProjectList)
		return;

	if (pCmdUI->m_pMenu) 
		for	(int iMRU =	0; iMRU	< m_pRecentProjectList->GetSize(); iMRU++)
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID +	iMRU, MF_BYCOMMAND);
	m_pRecentProjectList->UpdateMenu(pCmdUI);
}
//------------------------------------------------------------------------
BOOL CMTDevApp::cmdOnNewDoc(UINT nID)
{
	ASSERT((nID	>= ID_FILE_NEW_DOC1) &&	(nID < ID_FILE_NEW_DOC16));
	int	nIndex = nID - ID_FILE_NEW_DOC1;

	for	(POSITION pos =	m_pDocManager->GetFirstDocTemplatePosition();pos;nIndex--)
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(pos);
		if (nIndex==0)
			pTemplate->OpenDocumentFile(NULL);	
		CString	strTypeName;
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
void CMTDevApp::cmdupOnNewDocs(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_pMenu) 
		for	(int i = 0;	i <	16;	i++)
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID +	i, MF_BYCOMMAND);
	
	if (pCmdUI->m_pMenu==NULL)
		return;

	for	(POSITION pos =	m_pDocManager->GetFirstDocTemplatePosition();pos;)
	{
		CDocTemplate* pTemplate	= GetNextDocTemplate(pos);

		CString	strTypeName;
		if (pTemplate->GetDocString(strTypeName, CDocTemplate::fileNewName)	&& !strTypeName.IsEmpty())
		{
			pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++,
			MF_STRING |	MF_BYPOSITION, pCmdUI->m_nID++,	strTypeName);

		}
	}
	pCmdUI->m_nIndex--;	// point to	last menu added
	pCmdUI->m_nIndexMax	= pCmdUI->m_pMenu->GetMenuItemCount();
	pCmdUI->m_bEnableChanged = TRUE;	// all the added items are enabled*/
}

