/********************************************************************
	created:	2003/09/10
	created:	10:9:2003   9:15
	filename: 	$(Projects)\sources\mtdev\workspaceview\classtreectrl.cpp
	file path:	$(Projects)\sources\mtdev\workspaceview
	file base:	classtreectrl
	file ext:	cpp
	author:		Fadeyev
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include "Resource.h"

#include "GUIService.h"
#include "PropSet.h"

#include "ClassTreeCtrl.h"

#include "Names.h"
#include "BaseStructs.h"

using namespace properties;

static int Images[][9]= {
	// Unselected.Collapsed UnSelected.Expanded Selected.Collapsed Selected.Expanded
	{0,0,0,0},	//Project Root	
	{2,10,2,10},	//Rules Folder
	{2,10,2,10},	//FuncsFolder
	{2,10,2,10},	//StructsFolder
	{5,5,5,5},	//Rule Item
	{6,6,6,6},	//Func Item
	{7,7,7,7},	//Struct Item
	{12,13,12,13},	//User Folder
	{9,9,9,9},	//User Item
};	

#define UC 0
#define UE 1
#define SC 2
#define SE 3

#define ITEM_IMAGE_ROOT 				0
#define ITEM_IMAGE_RULES_FOLDER			1
#define ITEM_IMAGE_FUNCS_FOLDER			2
#define ITEM_IMAGE_STRUCTS_FOLDER		3
#define ITEM_IMAGE_USER_FOLDER			7
#define ITEM_IMAGE_REC_RULES_FOLDER		7

#define ITEM_IMAGE_RULE_ITEM			4
#define ITEM_IMAGE_FUNC_ITEM			5
#define ITEM_IMAGE_STRUCT_ITEM			6
#define ITEM_IMAGE_USER_ITEM			8


#define RECENTLY_RULES_WAIT_TIME		3000
#define RECENTLY_RULES_COUNT			8

#define NO_ID							(UINT)(-255)

//С данного места избавляемся от старых типов данных
typedef void CSkeletonInfo;
typedef void CItemInfo;

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass::CFolderManager
//------------------------------------------------------------------------
string_smart CProjectTreeClass::CFolderManager::GetDataFileName(CClassTreeSkelInfo * pProjectInfo)
{
	return change_fileext(pProjectInfo->GetPath(),EXT_CLW);
}
//------------------------------------------------------------------------
void CProjectTreeClass::CFolderManager::CloseProject(CClassTreeSkelInfo * pProjectInfo)
{
	SaveProject(pProjectInfo);
}
//------------------------------------------------------------------------
bool CProjectTreeClass::CFolderManager::OpenProject(CClassTreeSkelInfo * pProjectInfo)
{
	string_list strData;
	if (!strData.load_from_file(GetDataFileName(pProjectInfo).c_str()))
		return false;

	typedef map<string_std,CClassTreeItemInfo*> map_folders;
	map_folders mapFolders;

	for (size_t i =0; i<strData.items().count();i++)
	{
		string_split strLine(strData.items()[i]);
		if (!strLine.is_success_splitted())
			continue;

		HTREEITEM hExistItem = m_pWnd->GetItemFromPath(strLine.get_right_().c_str());
		CClassTreeItemInfo * pFolder=(hExistItem) ? m_pWnd->GetItemData(hExistItem) : NULL;

		//Создаем пользовательскую папку
		if (!pFolder)
		{
			string_list lstItems=str_split_text(strLine.get_right_(),"\\");
			HTREEITEM hParent=NULL;
			for (size_t j=0; j<lstItems.items().count(); j++){
				cstr szItem     = lstItems.items()[j];
				HTREEITEM hItem = m_pWnd->CTreeCtrlEx::FindItem(hParent,szItem);
				if (!hItem)
					hItem = m_pWnd->AddUserItem(szItem,iuidUserFolder, NULL,0,hParent,TVI_SORT)->m_hItem;
				hParent=hItem;
			}
			pFolder=m_pWnd->GetItemData(hParent);
		}
		
		//Здесь у нас есть папка, и теперь к ней можно привязывать элементы
		if ((strLine.get_left_().length()>0) && (pFolder) && (pFolder->m_nLevelType==CProjectTreeClass::ilvlFolder))
			pProjectInfo->MapItemToFolder(strLine.get_left_(),pFolder);
	}
	return true;
}
//------------------------------------------------------------------------
bool CProjectTreeClass::CFolderManager::SaveProject(CClassTreeSkelInfo * pProjectInfo)
{
	string_list strData;
	//Сохраняем список каталогов
	  
	for (CProjectTreeClass::CTreeItemCache::iterator i = m_pWnd->m_ItemCache.FirstPos();
		 i!=m_pWnd->m_ItemCache.EndPos(); ++i)
	{
		CClassTreeItemInfo * pFolder = (*i);

		if ((pFolder->m_nLevelType==CProjectTreeClass::ilvlFolder) && 
			(pFolder->m_pSkelInfo==pProjectInfo))
			strData.items().add(string_smart("=",pFolder->GetPath().c_str()));
	}

	//Сохраняем всю карту
	for (CClassTreeSkelInfo::mapped_items::iterator i = pProjectInfo->m_mapMappedItems.begin(); i!=pProjectInfo->m_mapMappedItems.end();++i){
		strData.items().add(string_smart((*i).first.c_str(),"=",(*i).second->GetPath().c_str()));
	}

	return strData.save_to_file(GetDataFileName(pProjectInfo).c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass:CTreeItemCache
void CProjectTreeClass::CTreeItemCache::AddItem(CClassTreeItemInfo * pItemInfo)
{
	m_lstCache.push_back(pItemInfo);
}
//------------------------------------------------------------------------
void CProjectTreeClass::CTreeItemCache::RemoveItem(CClassTreeItemInfo * pItemInfo)
{
	for (list_cache::iterator i=m_lstCache.begin();i!=m_lstCache.end();++i)
		if ((*i)==pItemInfo){
			m_lstCache.erase(i);
			return;
		}
	_ASSERT(false);
}
//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo* CProjectTreeClass::CTreeItemCache::FindByName(cstr szName)
{
	for (list_cache::iterator i=m_lstCache.begin();i!=m_lstCache.end();++i)
		if (strcmp((*i)->GetText(),szName)==0)
			return (*i);
	return NULL;
}
//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo * CProjectTreeClass::CTreeItemCache::FindByLocation(
	CProjectTreeClass::CClassTreeSkelInfo * pProjectInfo, cstr szFilePath, size_t nPosInText)
{
	for (list_cache::iterator i=m_lstCache.begin();i!=m_lstCache.end();++i)
		if (((*i)->m_pSkelInfo==pProjectInfo) &&
			(inrange(nPosInText,(size_t )(*i)->m_nPosStart,(size_t )(*i)->m_nPosEnd)) &&
			((*i)->m_strFilePath.cmpi(szFilePath)))
			return (*i);
		return NULL;
}
//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo * CProjectTreeClass::CTreeItemCache::Find
	(CProjectTreeClass::ELevelItems nItemLevel, 
	 CProjectTreeClass::EPredefItems nItemType, 
	 cstr szFilePath,
	 CProjectTreeClass::CClassTreeSkelInfo * pSkelInfo,
	 cstr szItemName)
{
	for (list_cache::iterator i=m_lstCache.begin();i!=m_lstCache.end();++i)
		if ((*i)->IsAppropriate(nItemLevel,nItemType,szFilePath,szItemName,pSkelInfo))
			return (*i);
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass:CRecentlyRules

typedef CProjectTreeClass::CRecentlyRules CPTC_RR;
BEGIN_MESSAGE_MAP(CPTC_RR, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo * CProjectTreeClass::CRecentlyRules::InsertItemIntoRecently(CClassTreeItemInfo * pItemInfo)
{
	HTREEITEM hRecentlyFolder=pItemInfo->m_pSkelInfo->m_hRulesRecentlyFolder; 
	
	//Небольшая оптимизация
	if  (m_pRecentlyAddedRule &&	//Был уже добавлен элемент
		(m_pRecentlyAddedRule==pItemInfo) && //И этот новый похож на предыдущий
		(m_pWnd->GetChildItem(hRecentlyFolder)==pItemInfo->m_hItem)) //И добавляемый элемент есть первый Child в папке
		return m_pRecentlyAddedRule;

	HTREEITEM hItem = m_pWnd->FindItem(hRecentlyFolder,pItemInfo->GetText());

	CClassTreeItemInfo* pItem;
	if (hItem)
		pItem = m_pWnd->GetItemData(hItem);
	else
		pItem = m_pWnd->AddItem(
					pItemInfo->GetText(),
					pItemInfo->GetImages(),
					hRecentlyFolder,
					TVI_FIRST,
					pItemInfo->m_nID,
					pItemInfo->m_nLevelType,
					pItemInfo->m_nType,
					pItemInfo->m_strFilePath.c_str(),
					pItemInfo->m_pSkelInfo,false);

	//Упорядочиваем и укорачиваем список
	if (pItem)
	{
		//Перемещаем элемент в начало списка, если он ее стоит в голове
		if (m_pWnd->GetItemOrderNumber(pItem->m_hItem)!=0)
			m_pWnd->RefresItemHandles(
				m_pWnd->MoveItem(pItem->m_hItem,hRecentlyFolder,TVI_FIRST));

		//Удаляем лишние элементы с хвоста
		int i = (int)(m_pWnd->GetChildCount(hRecentlyFolder))-RECENTLY_RULES_COUNT;
		if (i>0){
			HTREEITEM hChildItem=m_pWnd->GetChildItem(hRecentlyFolder);
			for (HTREEITEM hI=hChildItem; hI; hI = m_pWnd->GetNextItem(hI, TVGN_NEXT))
				hChildItem = hI;

			for (;i>0;i--){
				HTREEITEM hI = m_pWnd->GetNextItem(hChildItem, TVGN_PREVIOUS);
				m_pWnd->DeleteItem(hChildItem);
				hChildItem = hI;
			}
		}
	}

	if (m_bRunning){
		KillTimer(m_nTimerID);
		m_nTimerID=SetTimer(1,RECENTLY_RULES_WAIT_TIME,NULL);
	}
	m_pRecentlyAddedRule = m_pCatchedRule;
	return pItem;
}
//------------------------------------------------------------------------
void CProjectTreeClass::CRecentlyRules::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);
	
	IDocMngrLink *pDocMngr = i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngr);

	LOG_PROTECT_CODE_BEGIN
	
	ISourceDocLink * pSourceLink = pDocMngr->GetActiveSourceDoc();
	if (!pSourceLink)
		return;
	ISourceViewLink * pViewLink = pSourceLink->GetViewLink();
	if (!pViewLink)
		return;

	IProjectDocLink * pProjectLink = pDocMngr->FindParentProject(pSourceLink->GetPath().c_str());
	if (!pProjectLink)
		return;

	long nPos = pViewLink->GetCurPos();
	CClassTreeItemInfo * pItem = m_pWnd->FindSyntaxItem(pProjectLink->GetPath().c_str(),pSourceLink->GetPath().c_str(),nPos);
	if (!pItem){
		m_pCatchedRule=NULL;
		return;
	}

	if ((!m_pCatchedRule) || (m_pCatchedRule!=pItem)) {
		m_pCatchedRule=pItem;
	}
	else {
		InsertItemIntoRecently(m_pCatchedRule);
		m_pCatchedRule=NULL;
	}

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CProjectTreeClass::CRecentlyRules::Begin()
{
	return;
	if (!m_bRunning)
	{
		m_bRunning=true;
		Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),m_pWnd,0);
		m_nTimerID=SetTimer(1,RECENTLY_RULES_WAIT_TIME,NULL);
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::CRecentlyRules::End()
{
	if (IsWindow(m_hWnd)){
		KillTimer(m_nTimerID);
		DestroyWindow();
	}
	m_nTimerID=0;
	m_bRunning=false;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass:CScan

CCriticalSection CProjectTreeClass::CScan::m_CritSect;
bool			 CProjectTreeClass::CScan::m_bScanning = false;

//------------------------------------------------------------------------
CProjectTreeClass::CScan::~CScan(){
	_ASSERT(m_hCurrentScanThread==NULL);
}
//------------------------------------------------------------------------
void CProjectTreeClass::CScan::Begin(){
	//_ASSERT(!m_bScanning); Сейчас вполне допускается повторный вызов
	if (!m_bScanning){
		m_bScanning=true;
		m_hCurrentScanThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ScanThread,this,0,NULL);
		SetThreadPriority(m_hCurrentScanThread,THREAD_PRIORITY_BELOW_NORMAL);
	}
	m_strCurProjectPath.erase();
}
//------------------------------------------------------------------------
void CProjectTreeClass::CScan::End()	
{
	m_bScanning=false;
	if (m_hCurrentScanThread)
	{
		for (int i=0; i<300;i++)
		{
			if (WaitForSingleObject(m_hCurrentScanThread,100)!=WAIT_TIMEOUT)
				break;
			guiProcessMessages();
		}
		if 	(WaitForSingleObject(m_hCurrentScanThread,0)==WAIT_TIMEOUT){
			_RPT1(_CRT_ERROR,"Can't finish scan thread in ",__FUNCTION__);
			TerminateThread(m_hCurrentScanThread,-1);
		}

		CloseHandle(m_hCurrentScanThread);
		m_hCurrentScanThread=NULL;
	}

	m_strCurProjectPath.erase();
	m_mapScanData.clear();
}
//------------------------------------------------------------------------
//Ищет по имени файла и проекта структуру данных в карте
CProjectTreeClass::CScan::ScanData * CProjectTreeClass::CScan::FindData(cstr szProjectPath, cstr szFilePath){
	ASSERT_RETURN_NULL(szFilePath && szProjectPath);
	
	string_smart strKey(szProjectPath,szFilePath);
	map_scan::iterator i = m_mapScanData.find(strKey);
	return  (i!=m_mapScanData.end()) ? &(*i).second : NULL;
}
//------------------------------------------------------------------------
//Добавляет структуру данных для указанного файла проекта
CProjectTreeClass::CScan::ScanData * CProjectTreeClass::CScan::AddData(cstr szProjectPath, cstr szFilePath){
	ASSERT_RETURN_NULL(szFilePath && szProjectPath);

	string_smart strKey(szProjectPath,szFilePath);
	m_mapScanData[strKey]=ScanData(szProjectPath,szFilePath);
	return FindData(szProjectPath, szFilePath);
}
//------------------------------------------------------------------------
//Удаляет структуру данных для указанного файла. Если структура для 
//указанного файла не найдена, возвращает false;
bool CProjectTreeClass::CScan::DeleteData(cstr szProjectPath, cstr szFilePath){
	ASSERT_RETURN_FALSE(szFilePath && szProjectPath);
	
	string_smart strKey(szProjectPath,szFilePath);
	return m_mapScanData.erase(strKey)>0;
}
//------------------------------------------------------------------------
bool CProjectTreeClass::CScan::GetNextScanFile(string_smart & OUT strProjectName, string_smart & OUT strFileName)
{
	LOG_PROTECT_CODE_BEGIN
	cstr szFile;

	//Проверяем есть ли текущий проект
	if ((m_strCurProjectPath.isempty())	|| (!m_pWnd->GetProjectHeader(m_strCurProjectPath.c_str()))){
		if (m_pWnd->m_mapProjSkel.empty())
			return false; 
		m_strCurProjectPath=(*m_pWnd->m_mapProjSkel.begin()).second.GetPath();
		m_strCurFilePath.erase();
	}

	//Получаем описатель текущего проекта
	map_skel::iterator i = m_pWnd->m_mapProjSkel.find(m_strCurProjectPath.c_str());
	CClassTreeSkelInfo * pSkelInfo=(i!=m_pWnd->m_mapProjSkel.end()) ? &(*i).second : NULL;
	ASSERT_RETURN_FALSE(pSkelInfo);

/*	if ((!m_pWnd->IsItemExpanded(pSkelInfo->m_hFuncsFolder)) &&
		(!m_pWnd->IsItemExpanded(pSkelInfo->m_hRulesFolder)) && 
		(!m_pWnd->IsItemExpanded(pSkelInfo->m_hStructsFolder)))
		return false;
*/
	//Проверяем, есть ли текущий проверяемый файл
	if ((m_strCurFilePath.isempty()) ||
		(!pSkelInfo->GetProjectLink()->IsFilePresent(m_strCurFilePath.c_str()))) 	//Проверяем, входит ли этот файл в проект
	{
		for (szFile = pSkelInfo->GetProjectLink()->GetFileFirst();szFile; szFile = pSkelInfo->GetProjectLink()->GetFileNext())
			if (is_file_of_group(szFile, EXT_SOURCES))
				break;
		m_strCurFilePath = szFile;
	}

	//Вот здесь присваиваем полученные значения 
	strProjectName	= m_strCurProjectPath.c_str();
	strFileName		= m_strCurFilePath.c_str();
	
	//Добираемся по списку до текущего файла
	for (szFile = pSkelInfo->GetProjectLink()->GetFileFirst();szFile; szFile = pSkelInfo->GetProjectLink()->GetFileNext())
		if (m_strCurFilePath.cmpi(szFile))
			break;
	
	//Выбираем следующий файл
	if (szFile)
		for (szFile = pSkelInfo->GetProjectLink()->GetFileNext();szFile; szFile = pSkelInfo->GetProjectLink()->GetFileNext())
			if (is_file_of_group(szFile, EXT_SOURCES))
				break;
	m_strCurFilePath=szFile;
	
	//Проверяем, не пора ли перейти к следующему проекту
	if (m_strCurFilePath.isempty())
	{
		if (++i==m_pWnd->m_mapProjSkel.end())	//больше нет проектов
			m_strCurProjectPath.erase();
		else
			m_strCurProjectPath= (*i).second.GetPath();
	}

	LOG_PROTECT_CODE_END
	return (!strProjectName.isempty()) && (!strFileName.isempty());
}
//------------------------------------------------------------------------
void CProjectTreeClass::CScan::ScanFile(cstr szProjectPath, cstr szFilePath, bool bForcedRescan)
{
	m_CritSect.Lock();
	
	LOG_PROTECT_CODE_BEGIN
	
	//Работа со структурой данных
	ScanData * pData=FindData(szProjectPath,szFilePath);
	if (!pData) pData=AddData(szProjectPath,szFilePath);
	
	//Запуск разбора Разбор полученных результатов
	IParserLink::syntax_info SInfo;

	bool bDone=bForcedRescan; 

	IDocMngrLink * pDocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngrLink);

	//Пытаемся найти открытый документи проверяем, есть ли необходимость в сканировании
	ISourceDocLink * pDocIface = dynamic_cast <ISourceDocLink *> 
									(pDocMngrLink->FindDocument(szFilePath));

	//Если есть открытый документ исходного файла
	if (pDocIface){	
		bDone = (bForcedRescan) || (pData->tLastModified !=pDocIface->GetTimeOfLastChange());
		pData->tLastModified =pDocIface->GetTimeOfLastChange();
	}

	//Открытого документа нет, открываем из файла, если есть изменения
	else if (is_file_exist(szFilePath)){	
		FILETIME tWriteFile;
		if (get_file_time(szFilePath,NULL,NULL,&tWriteFile)){
			bDone =(bForcedRescan) || (CompareFileTime(&pData->ftLastModified,&tWriteFile)!=0);
			pData->ftLastModified=tWriteFile;
		}
	}

	//Запуск парсера 
	if (bDone){
		IParserLink * pParserIface = i_objects::GetInterface<IParserLink>();
		if (!pParserIface)
			bDone=false;
		else 
			pParserIface->ParseSourceFile(szFilePath, SInfo);
	}

	if (bDone)
	{
		//Этот класс нужен для того, чтобы записывать данные в основном потоке
		class fill_data_f: public functors::IFunctorRemote
		{
		private:
			ScanData					m_Data; //Не указатель, а объект, Так безопаснее, чем меньше указателей, тем лучше
			IParserLink::syntax_info&	m_SInfo;
			CProjectTreeClass *			m_pWnd;
			size_t						m_nScanNo;
			virtual void invoke()
			{
				m_pWnd->BeginUpdate();

				LOG_PROTECT_CODE_BEGIN
				IParserLink::si_iterator i;
				//Функции
				for (i=m_SInfo.m_Functions.GetFirstUnitPos();!m_SInfo.m_Functions.IsEndUnitPos(i);) {
					IParserLink::syntax_item & SItem = m_SInfo.m_Functions.GetNextUnit(i);
					m_pWnd->AddSyntaxItem(SItem,m_Data.strProjectPath.c_str(),m_nScanNo);
				}
				//Правила
				for (i=m_SInfo.m_Rules.GetFirstUnitPos();!m_SInfo.m_Rules.IsEndUnitPos(i);) {
					IParserLink::syntax_item & SItem = m_SInfo.m_Rules.GetNextUnit(i);
					m_pWnd->AddSyntaxItem(SItem,m_Data.strProjectPath.c_str(),m_nScanNo);
				}

				//Структуры
				for (i=m_SInfo.m_Structs.GetFirstUnitPos();!m_SInfo.m_Structs.IsEndUnitPos(i);) {
					IParserLink::syntax_item & SItem = m_SInfo.m_Rules.GetNextUnit(i);
					m_pWnd->AddSyntaxItem(SItem,m_Data.strProjectPath.c_str(),m_nScanNo);
				}

				LOG_PROTECT_CODE_END

				m_pWnd->SiftSyntaxItems(m_Data.strProjectPath.c_str(), m_Data.strFilePath.c_str(),m_nScanNo);
				m_pWnd->SortProject(m_Data.strProjectPath.c_str());
				m_pWnd->EndUpdate();
			}
		public:
			string_smart m_strProjectName;
			string_smart m_strFileName;
			fill_data_f(CProjectTreeClass * pWnd,ScanData & Data, IParserLink::syntax_info&	SInfo, size_t nScanNo): m_pWnd(pWnd), m_Data(Data), m_SInfo(SInfo), m_nScanNo(nScanNo){
			}
			bool run(){
				i_objects::GetInterface<IMainFrameLink>()->SyncInvoke(*this);//Здесь можно не проверять на NULL
				return true;
			}
		};

		fill_data_f f(m_pWnd,*pData,SInfo,++m_nScanNo);
		f.run();
	}	
	LOG_PROTECT_CODE_END
	m_CritSect.Unlock();
}
//------------------------------------------------------------------------
void CProjectTreeClass::CScan::ScanWholeProject(cstr szProjectPath)
{
	CClassTreeSkelInfo * pSkelInfo=m_pWnd->GetProjectHeader(szProjectPath);
	ASSERT(pSkelInfo);
	if (!pSkelInfo)
		return;
	for (cstr szFile = pSkelInfo->GetProjectLink()->GetFileFirst();szFile; szFile = pSkelInfo->GetProjectLink()->GetFileNext())
		if (is_file_of_group(szFile, EXT_SOURCES))
			ScanFile(szProjectPath,szFile,true);
}
//------------------------------------------------------------------------
DWORD WINAPI CProjectTreeClass::CScan::ScanThread(CScan * pSelf)
{
	Init_Thread_EH();	//поддержка с++	исключений

	//Этот класс нужен для того, чтобы получить синхронно данные из основного потока
	//от функции GetNextScanFile
	class get_next_f: public functors::IFunctorRemote{
	private:
		const bool&	 m_bScanning;
		CScan *		 m_pSelf;
		bool		 m_result;
		virtual void invoke(){
			m_result = m_bScanning && m_pSelf->GetNextScanFile(m_strProjectName,m_strFileName);
		}
	public:
		string_smart m_strProjectName;
		string_smart m_strFileName;
		get_next_f(CScan * pSelf,bool & bScanning): m_pSelf(pSelf), m_bScanning(bScanning){}
		bool run()
		{
			m_result = false;
			IMainFrameLink * pMainFrame = singleton_cache::GetMainFrameLink();
			if (pMainFrame) //Из чужого потока может быть NULL
				pMainFrame->SyncInvoke(*this);
			return m_result;
		}
	};
	//Функтор для IsWindowVisible
	class is_w_visible_f: public functors::IFunctorRemote{
	private:
		const bool&	m_bScanning;
		CScan*		m_pSelf;
		bool		m_result;
		virtual void invoke(){
			m_result = (m_bScanning) && (m_pSelf->GetWnd()) && m_pSelf->GetWnd()->IsWindowVisible()!=FALSE;
		}
	public:
		is_w_visible_f(CScan * pSelf, bool & bScanning): m_pSelf(pSelf), m_bScanning(bScanning){}
		bool run(){
			m_result = false;
			IMainFrameLink * pMainFrame = singleton_cache::GetMainFrameLink();
			if (pMainFrame) //Из чужого потока может быть NULL
				pMainFrame->SyncInvoke(*this);
			return m_result;
		}
	};
	//--main cycle--------------------------------------------------------
	while (m_bScanning) {
		LOG_PROTECT_CODE_BEGIN
		is_w_visible_f iwvf(pSelf,m_bScanning); 
		if (iwvf.run())
		{
			get_next_f f(pSelf,m_bScanning); 
			if (f.run())
				pSelf->ScanFile(f.m_strProjectName.c_str(),f.m_strFileName.c_str(),false);
		}
		Sleep(100);
		LOG_PROTECT_CODE_END
	} 
	//--------------------------------------------------------------------
	return TRUE;
}
//------------------------------------------------------------------------
void CProjectTreeClass::CScan::DeleteFileInfo(cstr szProjectPath, cstr szFilePath)
{
	DeleteData(szProjectPath, szFilePath);
	//Добавлено для залатывания проблемы с удалением файла из проекта.
	//Процедура сканирования ScanCur настолько хитрая, что не всегда 
	//обнаруживает удаление файла
	m_pWnd->BeginUpdate();
	m_pWnd->SiftSyntaxItems(szProjectPath, szFilePath,++m_nScanNo);
	m_pWnd->SortProject(szProjectPath);
	m_pWnd->EndUpdate();
}
/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass
//------------------------------------------------------------------------
#pragma warning(disable:4355)
CProjectTreeClass::CProjectTreeClass(): m_Scaner(this),m_RecentlyRules(this),m_FolderManager(this),m_ItemCache(this)
{
}
#pragma warning(default:4355)
CProjectTreeClass::~CProjectTreeClass()
{
}
BEGIN_MESSAGE_MAP(CProjectTreeClass, CProjectTree_B)
	//{{AFX_MSG_MAP(CProjectTreeClass)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDblclk)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnTvnEndlabeledit)
	ON_COMMAND(ID_CLW_DEFINITION, cmdOnGotoDefinition)
	ON_COMMAND(ID_CLW_SORT, cmdOnViewSort)
	ON_UPDATE_COMMAND_UI(ID_CLW_SORT, cmdupOnViewSort)
	ON_COMMAND(ID_CLW_CLEAR_RECENTLY_LIST, cmdOnClwClearRecentlyList)
	ON_UPDATE_COMMAND_UI(ID_CLW_CLEAR_RECENTLY_LIST, cmdupOnClwClearRecentlyList)
	ON_COMMAND(ID_CLW_NEW_FOLDER, cmdOnNewFolder)
	ON_UPDATE_COMMAND_UI(ID_CLW_NEW_FOLDER, cmdupOnNewFolder)
	ON_COMMAND(ID_CLW_RENAME, cmdOnClwRename)
	ON_UPDATE_COMMAND_UI(ID_CLW_RENAME, cmdupOnClwRename)
	ON_COMMAND(ID_CLW_DELETE, cmdOnClwDelete)
	ON_UPDATE_COMMAND_UI(ID_CLW_DELETE, cmdupOnClwDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CProjectTreeClass::CClassTreeItemInfo *CProjectTreeClass::AddItem(
	cstr szItemName, CButtonState & btnState, HTREEITEM hParent, HTREEITEM hInsertAfter,
	DWORD nID,ELevelItems nLevelType, EPredefItems nType, cstr szFilePath, CClassTreeSkelInfo*  pSkelInfo,
	bool bCheckOnExist)
{
	//Ищем такой-же уже существующий 
	//Элементы должны совпадать по имени, по типу, пор имени файла
	CClassTreeItemInfo * pItem = (bCheckOnExist) ? FindItemUniq(nLevelType,nType,szFilePath,pSkelInfo,szItemName) : NULL;

	//Если элемент не найден, добавляем
	if (!pItem){
		bool bChildren = (hParent==TVI_ROOT) ? true :ItemHasChildren(hParent)!=FALSE;

		HTREEITEM hItem = InsertItem(szItemName, btnState.Get_Unselected_Collapsed(),btnState.Get_Selected_Collapsed(),hParent,hInsertAfter);
		ASSERT_RETURN_NULL(hItem); 

		SetItemData(hItem,(DWORD_PTR)new CClassTreeItemInfo (*this,hItem,btnState,nLevelType,nType,nID,szFilePath,pSkelInfo));
		pItem = GetItemData(hItem);

		//Почему-то TreeView не прорисовывает "+" на экране при первом добавлении
		if (!bChildren && ItemHasChildren(hParent)) 
		{
			RECT rect; 
			if (GetItemRect(pItem->GetParent(),&rect,false))
				InvalidateRect(&rect);
		}
	}
	else{
		_ASSERT(_CrtIsValidHeapPointer(pItem));
		pItem->m_nID=nID; //Просто обновляем идентификатор
	}

	ASSERT_RETURN_NULL(pItem);

	//Уставливаем признак обновления
	//Есть варианты, что этот элемент дублируется несколько раз в разных папках
	//поэтому мы строим еще один цикл и обновляем все однотипные элементы
	for (CTreeItemCache::iterator i = m_ItemCache.FirstPos(); i!=m_ItemCache.EndPos(); ++i)
	{
		CClassTreeItemInfo * pI = (*i);

		if ((pI) && (pI!=pItem) && (pI->IsAppropriate(nLevelType,nType,szFilePath,szItemName,pSkelInfo)))
		{
			if (!pItem->HasPals())
				pItem->IncludeToPals(pI);		//Все однотипные элементы завязываем в цепочку
			pI->m_nID=pItem->m_nID;
		}
	}

	_ASSERT(_CrtIsValidHeapPointer(pItem));
	return pItem;
}

//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo * CProjectTreeClass::AddPredefinedItem(LPCTSTR szName,LPCTSTR szToProject, EPredefItems nType, LPCTSTR szFilePath, DWORD nID,HTREEITEM hInsertAter/*=TVI_LAST*/)
{
	//Получаем привязку с проекту
	map_skel::iterator i = m_mapProjSkel.find(szToProject);
	ASSERT_RETURN_NULL(i!=m_mapProjSkel.end());
	CClassTreeSkelInfo *pSkelInfo= &(*i).second;
	
	//Определяем свойства элемента на основе его типа
	HTREEITEM	hParent=NULL;
	int			*pnImageLine;
	ELevelItems	nItemLevel;
	CClassTreeItemInfo *pMapFolder;

	switch (nType){
	case iidProjectRoot:	
		nItemLevel= ilvlProject; pnImageLine= Images[ITEM_IMAGE_ROOT]; hParent = TVI_ROOT;
		break;
	case iidRulesFolder:	
		nItemLevel= ilvlFolder; pnImageLine = Images[ITEM_IMAGE_RULES_FOLDER]; hParent=pSkelInfo->m_hProjectRoot;
		break;
	case iidFuncsFolder:	
		nItemLevel= ilvlFolder; pnImageLine = Images[ITEM_IMAGE_FUNCS_FOLDER]; hParent=pSkelInfo->m_hProjectRoot;
		break;
	case iidStructsFolder:	
		nItemLevel= ilvlFolder; pnImageLine = Images[ITEM_IMAGE_STRUCTS_FOLDER]; hParent=pSkelInfo->m_hProjectRoot;
		break;
	case iidRulesRecentlyFolder:	
		nItemLevel= ilvlFolder; pnImageLine = Images[ITEM_IMAGE_REC_RULES_FOLDER]; hParent=pSkelInfo->m_hRulesFolder;
		break;
	case iidRuleItem:
		nItemLevel= ilvlItem; pnImageLine = Images[ITEM_IMAGE_RULE_ITEM]; 
		pMapFolder = pSkelInfo->FindMappedFolder(szName);
		hParent = (pMapFolder) ? pMapFolder->m_hItem : pSkelInfo->m_hRulesFolder;
		break;
	case iidFuncItem:
		nItemLevel= ilvlItem; pnImageLine = Images[ITEM_IMAGE_FUNC_ITEM]; 
		pMapFolder = pSkelInfo->FindMappedFolder(szName);
		hParent = (pMapFolder) ? pMapFolder->m_hItem : pSkelInfo->m_hFuncsFolder;
		break;
	case iidStructItem:
		nItemLevel= ilvlItem; pnImageLine = Images[ITEM_IMAGE_STRUCT_ITEM]; 
		pMapFolder = pSkelInfo->FindMappedFolder(szName);
		hParent = (pMapFolder) ? pMapFolder->m_hItem : pSkelInfo->m_hStructsFolder;
		break;
	default:
		ASSERT_RETURN_NULL(false);
	}
	_ASSERT(hParent);

	return AddItem(szName,CButtonState(pnImageLine),hParent,hInsertAter,nID,nItemLevel,nType,szFilePath,pSkelInfo);
}
//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo * CProjectTreeClass::AddUserItem(cstr szName,EUserItems nType, cstr szFilePath, DWORD nID,HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	//Получаем привязку с проекту
	CClassTreeItemInfo *pItemInfo=GetItemData(hParent);

	//Определяем свойства элемента на основе его типа
	int*		pnImageLine;
	ELevelItems	nItemLevel;

	switch (nType)
	{
	case iuidUserFolder:	
		nItemLevel= ilvlFolder; pnImageLine = Images[ITEM_IMAGE_USER_FOLDER];
		break;
	case iuidUserItem:	
		nItemLevel= ilvlItem; pnImageLine = Images[ITEM_IMAGE_USER_ITEM];
		break;
	default:
		ASSERT_RETURN_NULL(false);
	}
	_ASSERT(hParent);

	pItemInfo = AddItem(szName,CButtonState(pnImageLine),hParent,hInsertAfter,nID,nItemLevel,EPredefItems(nType),szFilePath,pItemInfo->m_pSkelInfo);

	return pItemInfo;
}
//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo * CProjectTreeClass::AddSyntaxItem(IParserLink::syntax_item & SItem, cstr szToProject, DWORD nID, HTREEITEM hInsertAfter/*=TVI_LAST*/)
{
	ASSERT(!SItem.strFilename.isempty());

	EPredefItems nItemType;

	//Получаем родительский элемент
	switch (SItem.nClass){
	case IParserLink::syntax_item::icStruct:
		nItemType = iidStructItem;
		break;
	case IParserLink::syntax_item::icFunc:
		nItemType = iidFuncItem;
		break;
	case IParserLink::syntax_item::icRule:
		nItemType = iidRuleItem;
		break;
	default:
		ASSERT_RETURN_NULL(NULL);
	}
	
	CClassTreeItemInfo * pItem = AddPredefinedItem(SItem.strName.c_str(),szToProject,nItemType,SItem.strFilename.c_str(),nID,hInsertAfter);
	ASSERT_RETURN_NULL(pItem);

	//Обновляем у всех друзей и у самого элемента информацию о позиции в тексте
	vector<CClassTreeItemInfo*> vPals; pItem->GetPalChain(vPals);
	for (size_t i=0;i<vPals.size();i++){
		vPals[i]->m_nPosStart=SItem.nPosStart;
		vPals[i]->m_nPosEnd = SItem.nPosEnd;
	}

	return pItem;
}
//------------------------------------------------------------------------
void CProjectTreeClass::SiftSyntaxItems(cstr szProjectPath, cstr szFilePath, DWORD nID)
{
	CClassTreeSkelInfo * pSkelInfo=GetProjectHeader(szProjectPath);
	ASSERT_RETURN_VOID(pSkelInfo);

	for (CTreeItemCache::iterator i = m_ItemCache.FirstPos(); i!=m_ItemCache.EndPos();)
	{
		CTreeItemCache::iterator j = i++;

		CClassTreeItemInfo * pI=(*j);
		if ((pI) && 
			(pI->m_nLevelType==ilvlItem) && 
			(pI->m_strFilePath.cmpi(szFilePath)) &&
			(pI->m_nID!=nID) &&
			(pI->m_nID!=NO_ID))
		{
			//Удаляем друзей и себя
			vector<CClassTreeItemInfo*> vPals; pI->GetPalChain(vPals);
			for (size_t x=0;x<vPals.size();x++)
				if ((*i)!=vPals[x])	//Иначе получиться, что рубим сук, на котором сидим
					DeleteItem(vPals[x]->m_hItem);
		}
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::Clear()
{
	m_mapProjSkel.clear();
	__super::Clear();
}
//------------------------------------------------------------------------
bool CProjectTreeClass::RemoveProject(LPCTSTR szProjectPath){
	CClassTreeSkelInfo *pSkelInfo=GetProjectHeader(szProjectPath);
	ASSERT(pSkelInfo);
	if (!pSkelInfo)
		return false;

	map_skel::iterator i =m_mapProjSkel.find(szProjectPath);
	ASSERT(i!=m_mapProjSkel.end());

	//Закрываем структуру пользовательских папок
	m_FolderManager.CloseProject(pSkelInfo);

	DeleteItem(pSkelInfo->m_hProjectRoot);
	
	if (i==m_mapProjSkel.end())
		return false;

	m_mapProjSkel.erase(i);

	return true;
} 
//------------------------------------------------------------------------
bool CProjectTreeClass::AddProjectHeader(IProjectDocLink & ieProjectLink)
{
	ASSERT(GetProjectHeader(ieProjectLink.GetPath().c_str())==NULL);
	//Записываем в карту пустой CClassTreeSkelInfo, а потом будем заполнять через итератор
	map_skel::iterator i = m_mapProjSkel.insert(map_skel::value_type(ieProjectLink.GetPath().c_str(),CClassTreeSkelInfo(ieProjectLink))).first;
	
	CClassTreeSkelInfo & I=(*i).second;

	I.m_hProjectRoot	= AddPredefinedItem	(DecorateName(I.GetTitle(),ilvlProject,false).c_str(), 
											 I.GetPath(), iidProjectRoot, I.GetPath(),0)->m_hItem;

	I.m_hRulesFolder	= AddPredefinedItem	(DecorateName(string_res(IDS_CLW_RULES_TITLE,HRESOURCES).c_str(),ilvlFolder,false).c_str(),
											 I.GetPath(), iidRulesFolder,NULL,0)->m_hItem;

	I.m_hRulesRecentlyFolder=AddPredefinedItem(DecorateName(string_res(IDS_CLW_RECENTLY_TITLE,HRESOURCES).c_str(),ilvlFolder,false).c_str(),
											 I.GetPath(), iidRulesRecentlyFolder,NULL,0)->m_hItem;

	I.m_hFuncsFolder	= AddPredefinedItem	(DecorateName(string_res(IDS_CLW_FUNCS_TITLE,HRESOURCES).c_str(),ilvlFolder,false).c_str(), 
											 I.GetPath(), iidFuncsFolder,NULL,0)->m_hItem;

	I.m_hStructsFolder  = AddPredefinedItem	(DecorateName(string_res(IDS_CLW_STRUCTS_TITLE,HRESOURCES).c_str(),ilvlFolder,false).c_str(), 
											 I.GetPath(), iidStructsFolder,NULL,0)->m_hItem;

	//Считываем структуру пользовательских папок
	m_FolderManager.OpenProject(&I);
	
	Expand (I.m_hProjectRoot, TVE_EXPAND);

	//Включаем сканирование текста
	m_Scaner.ScanWholeProject(I.GetPath());
	m_Scaner.Begin();

	//Включаем Recently Rules
	m_RecentlyRules.Begin();


	return true;
}
//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeSkelInfo * CProjectTreeClass::GetProjectHeader(LPCTSTR szProjectPath)
{
	ASSERT(szProjectPath);
	if (!szProjectPath)
		return NULL;

	map_skel::iterator i = m_mapProjSkel.find(szProjectPath);
	return  (i==m_mapProjSkel.end())
				? NULL
				: &(*i).second;
}
//------------------------------------------------------------------------
string_smart CProjectTreeClass::DecorateName(cstr szText, ELevelItems nLevel,bool bModified)
{	
	const char *szProjectTitles[2]= {"%s classes","%s classes*"};
	const char *szFileTitles[2]= {"%s","%s*"};

	string_smart S;
	switch (nLevel)
	{
	case ilvlRoot:		S=szText; break;
	case ilvlProject:	S=Format(szProjectTitles[bModified],szText); break;
	case ilvlFolder:	S=szText; break;	
	case ilvlItem:		S=Format(szFileTitles[bModified],extract_filenameext(szText).c_str()) ; break;
	default:
		S=szText;
	}
	return S;
}
//------------------------------------------------------------------------
static int CALLBACK CmpByName(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	typedef CProjectTreeClass::CClassTreeItemInfo * PItemNfo;
	PItemNfo pInfo1= reinterpret_cast<PItemNfo > (lParam1);
	PItemNfo pInfo2= reinterpret_cast<PItemNfo > (lParam2);
	pInfo1= reinterpret_cast<CProjectTreeClass::CClassTreeItemInfo *> (lParam1);
	if ((!pInfo1) && (!pInfo2))
		return 0;
	if (!pInfo1)
		return 1;
	if (!pInfo2)
		return -1;
	
	if (pInfo1->m_nLevelType==pInfo2->m_nLevelType)	{
		if ((pInfo1->m_nType==CProjectTreeClass::iidUserFolder) && 
			(pInfo2->m_nType!=CProjectTreeClass::iidUserFolder))
			return -1;
		else if ((pInfo2->m_nType==CProjectTreeClass::iidUserFolder) && 
			(pInfo1->m_nType!=CProjectTreeClass::iidUserFolder))
			return 1;
		else  if (oneof(pInfo1->m_nType,CProjectTreeClass::iidRulesFolder,CProjectTreeClass::iidFuncsFolder,CProjectTreeClass::iidStructsFolder) && 
				  oneof(pInfo2->m_nType,CProjectTreeClass::iidRulesFolder,CProjectTreeClass::iidFuncsFolder,CProjectTreeClass::iidStructsFolder))
			 return pInfo1->m_nType-pInfo2->m_nType;
		else

			return strcmp(pInfo1->GetText(),pInfo2->GetText());
	}
	else
		return pInfo1->m_nLevelType-pInfo2->m_nLevelType;
	return 0;
}
//------------------------------------------------------------------------
static int CALLBACK CmpByPos(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	typedef CProjectTreeClass::CClassTreeItemInfo * PItemNfo;
	PItemNfo pInfo1= reinterpret_cast<PItemNfo > (lParam1);
	PItemNfo pInfo2= reinterpret_cast<PItemNfo > (lParam2);
	pInfo1= reinterpret_cast<CProjectTreeClass::CClassTreeItemInfo *> (lParam1);
	if ((!pInfo1) && (!pInfo2))
		return 0;
	if (!pInfo1)
		return 1;
	if (!pInfo2)
		return -1;

	if (pInfo1->m_nLevelType==CProjectTreeClass::ilvlFolder &&
		pInfo2->m_nLevelType==CProjectTreeClass::ilvlFolder)	
	{
		if ((pInfo1->m_nType==CProjectTreeClass::iidUserFolder) && 
			(pInfo2->m_nType!=CProjectTreeClass::iidUserFolder))
			return -1;
		else if ((pInfo2->m_nType==CProjectTreeClass::iidUserFolder) && 
				(pInfo1->m_nType!=CProjectTreeClass::iidUserFolder))
			return 1;
		else  if (oneof(pInfo1->m_nType,CProjectTreeClass::iidRulesFolder,CProjectTreeClass::iidFuncsFolder,CProjectTreeClass::iidStructsFolder) && 
				  oneof(pInfo2->m_nType,CProjectTreeClass::iidRulesFolder,CProjectTreeClass::iidFuncsFolder,CProjectTreeClass::iidStructsFolder))
			return pInfo1->m_nType-pInfo2->m_nType;
		else
			return strcmp(pInfo1->GetText(),pInfo2->GetText());
	}

	if (pInfo1->m_nPosStart<pInfo2->m_nPosStart)
		return -1;
	if (pInfo1->m_nPosStart>pInfo2->m_nPosStart)
		return 1;
	return 0;
}
//------------------------------------------------------------------------
void CProjectTreeClass::SortItems(HTREEITEM hParent, ESortMode nSortMode)
{
	TVSORTCB tvs;	// Sort the tree control's items using my  callback procedure.

	ASSERT(hParent);
	switch (nSortMode){
	case smText:
		tvs.hParent = hParent;
		tvs.lpfnCompare = CmpByName;
		SortChildrenCB(&tvs);
		break;
	case smPosition:
		tvs.hParent = hParent;
		tvs.lpfnCompare = CmpByPos;
		SortChildrenCB(&tvs);
		break;
	default:
		ASSERT(false);
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::SortProject(cstr szProjectPath)
{
	CClassTreeSkelInfo *pSkelInfo = GetProjectHeader(szProjectPath);
	if (pSkelInfo){
		SortItems(pSkelInfo->m_hProjectRoot,m_nSortMode);
		SortItems(pSkelInfo->m_hRulesFolder,m_nSortMode);
		SortItems(pSkelInfo->m_hFuncsFolder,m_nSortMode);
		SortItems(pSkelInfo->m_hFuncsFolder,m_nSortMode);
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::RefresItemHandles(HTREEITEM hParent)
{
	if (!hParent)
		hParent=GetRootItem();
	if (hParent==NULL)
		return;

	CClassTreeItemInfo * pI=GetItemData(hParent);
	if (pI) pI->m_hItem=hParent;

	for (HTREEITEM hItem=GetChildItem(hParent); hItem; hItem = GetChildItemAny(hItem,hParent))
	{
		CClassTreeItemInfo * pI=GetItemData(hItem);
		if (pI)	pI->m_hItem=hItem;
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::OnCloseAppMainWnd()
{	
	__super::OnCloseAppMainWnd();
	m_Scaner.End();
}
//------------------------------------------------------------------------
void CProjectTreeClass::evOnOpenDocument(evdOpenDocumentInfo &Info)
{
	__super::evOnOpenDocument(Info);
}
//------------------------------------------------------------------------
void CProjectTreeClass::evOnCloseDocument(evdCloseDocumentInfo &Info)
{
	__super::evOnCloseDocument(Info);
}
//------------------------------------------------------------------------
void CProjectTreeClass::evOnChangeDocument(evdChangeDocumentInfo &Info)
{
	__super::evOnChangeDocument(Info);

	evdChangeDocumentInfo &info=CAST_DATA(Info,evdChangeDocumentInfo);
	ASSERT(info.in_strDocumentPath.c_str()); 

	//Это был проект
	if (info.in_nDocID==evdChangeProjectInfo::idProject)
	{
		//Работаем с информацией, как с информацией о проекте
		evdChangeProjectInfo &I=CAST_DATA(Info,evdChangeProjectInfo);

		CClassTreeSkelInfo * pSkelInfo = GetProjectHeader(I.in_strDocumentPath.c_str());
		ASSERT_RETURN_VOID(pSkelInfo);
		switch (I.in_nChangeType) 
		{
		case evdChangeProjectInfo::changeClose:	break;
		case evdChangeProjectInfo::changeModify: 
		case evdChangeProjectInfo::changeSave:	
			/*				SetItemText(
			pSkelInfo->m_hProjectRoot,DecorateName(
			pSkelInfo->GetTitle(),	
			ilvlProject,
			I.in_nChangeType==evdChangeProjectInfo::changeModify).c_str());*/
			break;
		case evdChangeProjectInfo::changeAddFile:
			ASSERT(!I.in_strIncludedFile.isempty());
			m_Scaner.ScanFile(I.in_strDocumentPath.c_str(),I.in_strIncludedFile.c_str(),true);
			break;
		case evdChangeProjectInfo::changeRemoveFile:
			ASSERT(!I.in_strIncludedFile.isempty());
			m_Scaner.DeleteFileInfo(I.in_strDocumentPath.c_str(),I.in_strIncludedFile.c_str());
			break;
		case evdChangeProjectInfo::changeActivate:
			SetItemBold(pSkelInfo->m_hProjectRoot,true);
			break;
		case evdChangeProjectInfo::changeDeactivate:
			SetItemBold(pSkelInfo->m_hProjectRoot,false);
			break;
		}
		return;
	}
/*	//Это базовый документ, он же документ исходных текстов (Source || Other)
	else if (info.in_nDocID==evdChangeDocumentInfo::idDocument)
	{
	}*/
}
//------------------------------------------------------------------------
void CProjectTreeClass::evOnChangeCursorPos(ISourceViewLink * m_pSender, size_t m_nPos)
{
}
//------------------------------------------------------------------------
CProjectTreeClass::CClassTreeItemInfo * CProjectTreeClass::FindSyntaxItem(cstr szProjectName, cstr szFileName, long nPos) 
{
	CClassTreeSkelInfo * pSkelInfo = GetProjectHeader(szProjectName);
	ASSERT_RETURN_NULL(pSkelInfo);

	return m_ItemCache.FindByLocation(pSkelInfo,szFileName,nPos);
	return NULL;
}
//------------------------------------------------------------------------
bool CProjectTreeClass::SelectSyntaxItem(cstr szProjectName, cstr szFileName, long nPos)
{
	CClassTreeItemInfo * pI=FindSyntaxItem(szProjectName,szFileName,nPos);
	if (pI) {
		this->SelectItem(pI->m_hItem);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
BOOL CProjectTreeClass::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		//Редактирование
		if (GetEditControl()!=NULL) {
			if (oneof(pMsg->wParam,(WPARAM)VK_RETURN,(WPARAM)VK_ESCAPE)){
				TreeView_EndEditLabelNow(m_hWnd,pMsg->wParam==VK_ESCAPE);
				return true;
			}
		}
		//Обычный режим
		else if (pMsg->wParam==VK_DELETE){
			cmdOnClwDelete();
			return true;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
//------------------------------------------------------------------------
bool CProjectTreeClass::OnBeginDrag(POINT ptDragPoint, const TVITEM & ItemData)
{
	CClassTreeItemInfo * pItemInfo = reinterpret_cast<CClassTreeItemInfo *>(ItemData.lParam);
	return (pItemInfo) && ((pItemInfo->m_nType==iidUserFolder) || (pItemInfo->m_nLevelType==ilvlItem));
}
//------------------------------------------------------------------------
bool CProjectTreeClass::OnDragOver(POINT ptDragPoint, const TVITEM & ItemData, HWND hOverWnd, HTREEITEM hOverItem, bool & bUseStdCursors)
{
	if ((hOverWnd!=m_hWnd) || (ItemData.hItem==hOverItem) || (hOverItem==NULL))
		return false;

	CClassTreeItemInfo * pOverItemInfo=GetItemData(hOverItem);
	CClassTreeItemInfo * pDragItemInfo=GetItemData(ItemData.hItem);
	if ((!pOverItemInfo) || 
		(!pDragItemInfo) ||
		(pOverItemInfo->m_nLevelType==ilvlItem) || 
		(GetParentItem(ItemData.hItem)==hOverItem)||
		(ItemData.hItem==hOverItem)||
		(pOverItemInfo->m_pSkelInfo!=pDragItemInfo->m_pSkelInfo))
		return false;

	if (pOverItemInfo->m_nType==iidRulesRecentlyFolder){
		bUseStdCursors=false;
		::SetCursor(guiGetScreen().GetCursorHandle(crDragCopy));
		return true;
	}

	if ((pDragItemInfo->m_nType==iidRuleItem) && (!oneof(pOverItemInfo->m_nType,iidRulesFolder,iidUserFolder)))
		return false;
	if ((pDragItemInfo->m_nType==iidFuncItem) && (!oneof(pOverItemInfo->m_nType,iidFuncsFolder,iidUserFolder)))
		return false;
	if ((pDragItemInfo->m_nType==iidStructItem) && (!oneof(pOverItemInfo->m_nType,iidStructsFolder,iidUserFolder)))
		return false;

	//Копирование, а не перенос
/*	if (oneof(pDragItemInfo->m_nType,iidRuleItem,iidFuncItem,iidStructItem) &&
		CShiftState().IsCtrl())
	{
		bUseStdCursors=false;
		::SetCursor(guiGetScreen().GetCursorHandle(crDragCopy));
		return true;
	}
*/
	return true;
}
//------------------------------------------------------------------------
void CProjectTreeClass::OnDragDrop(POINT ptDragPoint, const TVITEM & ItemData, HWND hOverWnd,HTREEITEM hOverItem)
{
	CClassTreeItemInfo * pOverItemInfo=GetItemData(hOverItem);
	CClassTreeItemInfo * pDragItemInfo=GetItemData(ItemData.hItem);
	HTREEITEM hItem=NULL;
	if (pOverItemInfo->m_pSkelInfo!=pDragItemInfo->m_pSkelInfo) 
		return;
	
	CClassTreeSkelInfo * pProjectInfo = pDragItemInfo->m_pSkelInfo;

	//В Recently-мы просто копируем
	if (pOverItemInfo->m_nType==iidRulesRecentlyFolder){
		m_RecentlyRules.InsertItemIntoRecently(pDragItemInfo);
	}
	//Переносим в пользовательскую папку
	else if (pOverItemInfo->m_nType==iidUserFolder) 
	{
		hItem= MoveItem(ItemData.hItem,hOverItem,TVI_LAST);
		pOverItemInfo->MapItemToMe(ItemData.pszText);
	}
	//Переносим пользовательскую папку
	else if ((pDragItemInfo->m_nType==iidUserFolder) && (pDragItemInfo->m_nLevelType!=ilvlItem)){
		hItem= MoveItem(ItemData.hItem,hOverItem,TVI_FIRST);
	}
	//Возвращаем обратно в папку
	else if (((pDragItemInfo->m_nType==iidRuleItem) && (pOverItemInfo->m_hItem==pProjectInfo->m_hRulesFolder)) ||
			 ((pDragItemInfo->m_nType==iidFuncItem) && (pOverItemInfo->m_hItem==pProjectInfo->m_hFuncsFolder)) ||
			 ((pDragItemInfo->m_nType==iidStructItem) && (pOverItemInfo->m_hItem==pProjectInfo->m_hStructsFolder)))
	{		
		hItem = MoveItem(ItemData.hItem,hOverItem,TVI_LAST);
		pOverItemInfo->m_pSkelInfo->RemoveMapping(ItemData.pszText);
	}
	
	if (hItem){
		SortProject(pProjectInfo->GetPath());
		RefresItemHandles(hOverItem);
		EnsureVisible(hItem);
		m_FolderManager.SaveProject(pProjectInfo);
	}
}
//------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass message handlers
int CProjectTreeClass::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Load view images:
	m_Images.Create (IDB_CLASS_VIEW, 16, 0, RGB (255, 255, 255));
	SetImageList (&m_Images, TVSIL_NORMAL);
	
	SetIndent(15);
	EnableDragAndDrop(true);
	m_nSortMode= (ESortMode	)AfxGetApp()->GetProfileInt(szProfileWorkSpace,szParamSortItemsMode,smPosition);

	return 0;
}
//------------------------------------------------------------------------
void CProjectTreeClass::OnDestroy() 
{
	m_Scaner.End();
	m_RecentlyRules.End();
	Clear();
	AfxGetApp()->WriteProfileInt(szProfileWorkSpace,szParamSortItemsMode,m_nSortMode);
	__super::OnDestroy();
}
//------------------------------------------------------------------------
BOOL CProjectTreeClass::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

    NMHDR* pNMHDR = (NMHDR*)lParam;
    ASSERT (pNMHDR != NULL);

    if (pNMHDR->code == TTN_SHOW && GetToolTips () != NULL)
    {
        GetToolTips ()->SetWindowPos (&wndTop, -1, -1, -1, -1,
            SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
    }

    return bRes;
}
//------------------------------------------------------------------------
void CProjectTreeClass::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//Select Item
	if (point != CPoint (-1, -1)){
		CPoint ptTree = point; ScreenToClient (&ptTree);
		HTREEITEM hTreeItem = HitTest (ptTree);
		if (hTreeItem != NULL)
			SelectItem (hTreeItem);
	}

	CClassTreeItemInfo * pI;
	if (!GetCurItemData(pI))
	{
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_BASE, point, AfxGetMainWnd ());
		return; 
	}
	ASSERT_RETURN_VOID(pI);

	switch (pI->m_nType)
	{
	case iidProjectRoot:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_PROJECT, point, AfxGetMainWnd ());
		break;
	case iidRulesFolder:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_RULES, point, AfxGetMainWnd ());
		break;
	case iidFuncsFolder:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_FUNCS, point, AfxGetMainWnd ());
		break;
	case iidStructsFolder:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_STRUCTS, point, AfxGetMainWnd ());
		break;
	case iidRulesRecentlyFolder:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_RULES_RECENTLY, point, AfxGetMainWnd ());
		break;
	case iidUserFolder:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_USER_FOLDER, point, AfxGetMainWnd ());
		break;
	case iidRuleItem:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_RULE, point, AfxGetMainWnd ());
		break;
	case iidFuncItem:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_RULE, point, AfxGetMainWnd ());
		break;
	case iidStructItem:
		GetWorkspace()->ShowPopupMenu (IDR_POPUP_CLW_RULE, point, AfxGetMainWnd ());
		break;
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	cmdOnGotoDefinition();
}
//------------------------------------------------------------------------
void CProjectTreeClass::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	
	*pResult = 1;
	CClassTreeItemInfo * pItemInfo=GetItemData(pTVDispInfo->item.hItem);

	//Кому можно редактироваться
	if ((pItemInfo) && (pItemInfo->m_nType==iidUserFolder))
		*pResult = 0;
}
//------------------------------------------------------------------------
void CProjectTreeClass::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	*pResult = 0;	//Запрещяем автоматическую замену текста, меняем сами
	CClassTreeItemInfo * pInfo = GetItemData(pTVDispInfo->item.hItem);
	if ((pInfo) && (pTVDispInfo->item.pszText!=NULL))
	{
		//Выкидываем запрещенные символы
		string_smart s;
		for (cstr pS=pTVDispInfo->item.pszText; *pS;pS++)
			if (*pS!=_T('\\'))
				s.cat(*pS);

		pInfo->SetText(s.c_str());//Подменяем текст
		m_FolderManager.SaveProject(pInfo->m_pSkelInfo);  //??? Тормозит??
	}
}
//------------------------------------------------------------------------
BOOL CProjectTreeClass::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdOnViewSort() 
{
	if (m_nSortMode==smText)
		m_nSortMode=smPosition;
	else
		m_nSortMode=smText;
	for (map_skel::iterator i=m_mapProjSkel.begin();i!=m_mapProjSkel.end();++i)
		SortProject((*i).second.GetPath());
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdOnGotoDefinition() 
{
	CClassTreeItemInfo * pI;
	if ((!GetCurItemData(pI))|| (pI->m_nLevelType!=ilvlItem)) //Обрабатываем только элементы уровня Item
		return; 

	IDocMngrLink * pDocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngrLink);

	IDocumentLink * pDocLink = pDocMngrLink->OpenDocument(pI->m_strFilePath.c_str());

	CWaitCursor aWaitCursor;	//Меняем курсор на часы, разрушаться они автоматически

	HTREEITEM hCur = pI->m_hItem;
	m_Scaner.ScanFile(pI->GetProjectPath(),pI->m_strFilePath.c_str(),true);
	if (IsItemValid(hCur))	//Все нормально. Элемент остался
		pI=GetItemData(hCur);
	else
		return;				//Элемента больше нет


	LOG_PROTECT_CODE_BEGIN
	
	//Приводим к документу исходников (полученный документ должен быть с интерфейсом ISourceDocLink )
	ISourceDocLink * pDocIface = dynamic_cast <ISourceDocLink *>(pDocLink);
	if (pDocIface ){
		ISourceViewLink * pViewLink = pDocIface->GetViewLink();
		pViewLink->GotoPos(pI->m_nPosStart);
		pViewLink->EnsureRangeVisible(pI->m_nPosStart,pI->m_nPosEnd);

		//Дополнительно посылаем сообщения в асинхронном режиме
		//Были проблемы с DblClick
		//Переводим фокус
		PostMessage(CM_POST,WM_SETFOCUS,reinterpret_cast<LPARAM>(pViewLink->GetWnd()));
	}
 
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdupOnViewSort(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_nSortMode==smText);
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdOnClwClearRecentlyList()
{
	ASSERT_RETURN_VOID(GetSelectedItem());

	for (HTREEITEM hItem=GetChildItem(GetSelectedItem()); hItem; )
	{
		HTREEITEM hTemp=hItem; hItem = GetNextSiblingItem(hItem);
		
		CClassTreeItemInfo* pItemInfo=GetItemData(hTemp);
		if (pItemInfo->m_nLevelType==ilvlItem)
			DeleteItem(hTemp);
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdupOnClwClearRecentlyList(CCmdUI *pCmdUI)
{
	CClassTreeItemInfo * pItemInfo;
	ASSERT_RETURN_VOID(GetCurItemData(pItemInfo));

	pCmdUI->Enable(GetChildCount(pItemInfo->m_hItem)>0);
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdOnNewFolder()
{
	string_res strPattern(IDS_NEW_FOLDER_d,HRESOURCES);
	string_smart strNewFolder=Format(strPattern.c_str(),1);

	CClassTreeItemInfo * pItemInfo;
	HTREEITEM	hParent=GetSelectedItem();
	if (hParent==NULL)
		return;

	//Подыскиываем подходящего родителя для нашей папки
	for (pItemInfo = GetItemData(hParent);hParent && pItemInfo && (pItemInfo->m_nLevelType==ilvlItem);pItemInfo = GetItemData(hParent))
		hParent=GetParentItem(hParent);

	if (pItemInfo==NULL)
		return;

	//Выбираем автоимя
	for (int i=1;; i++){
		bool bBusy=false;
		for (HTREEITEM hItem=GetChildItem(GetRootItem()); hItem; hItem =GetChildItemAny(hItem,GetRootItem())){
			if (GetItemText(hItem).CompareNoCase(strNewFolder.c_str())==0){
				bBusy=true;
				break;
			}
		}
		if (!bBusy)
			break;
		strNewFolder=Format(strPattern.c_str(),i+1);
	}


	HTREEITEM hItem = AddUserItem(strNewFolder.c_str(),iuidUserFolder, NULL,0,hParent,TVI_SORT)->m_hItem;
	EnsureVisible(hItem);
	EditLabel(hItem);
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdupOnNewFolder(CCmdUI *pCmdUI)
{
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdOnClwRename()
{
	EditLabel(GetSelectedItem());
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdupOnClwRename(CCmdUI *pCmdUI)
{
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdOnClwDelete()
{
	CClassTreeItemInfo * pItemInfo;
	if (!GetCurItemData(pItemInfo))
		return;
	
	if (pItemInfo->m_nType==iidUserFolder){
		CClassTreeSkelInfo * pProject = pItemInfo->m_pSkelInfo;
		DeleteItem(pItemInfo->m_hItem);
		m_Scaner.ScanWholeProject(pProject->GetPath());
		m_FolderManager.SaveProject(pProject);
	}
}
//------------------------------------------------------------------------
void CProjectTreeClass::cmdupOnClwDelete(CCmdUI *pCmdUI)
{
}
