/********************************************************************
	created:	2002/03/08
	created:	8:3:2002   9:50
	filename: 	$(Projects)\Sources\MTDEv\WorkSpaceView\ClassTreeCtrl.h
	file path:	$(Projects)\Sources\MTDEv\WorkSpaceView

	file base:	ClassTreeCtrl
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Реализация вкладки "Classes" в WorkSpaceBar
*********************************************************************/

#if !defined(AFX_ClassTreeCtrl_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_)
#define AFX_ClassTreeCtrl_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include <map>
#include <ProjectTree_B.h>
#include <BaseUtils.h>

#include "BaseStructs.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeClass window
class CProjectTreeClass :	public CProjectTree_B, 
							public IClassTreeLink, 
							public ISourceViewLink::IEventSink
{
public:
	//Какие бывают зарезервированные папки
	enum EPredefItems	{iidNone, iidProjectRoot,iidRulesFolder, iidFuncsFolder, iidStructsFolder, iidRulesRecentlyFolder, iidRuleItem,iidFuncItem,iidStructItem,iidUserFolder,iidUserItem};
	enum EUserItems		{iuidUserFolder=iidUserFolder,iuidUserItem=iidUserItem};

	//Как можно сортировать элементы
	enum ESortMode {smText, smPosition};

	//--------------------------------------------------------------------
	//Информация о скелете - какой ID у главного заголовка, какой - у iidSourcesFolder  и т.д.
	class CClassTreeItemInfo;
	class CClassTreeSkelInfo: public CProjectTree_B::CSkeletonInfo
	{
	public:
		HTREEITEM m_hRulesFolder;
		HTREEITEM m_hFuncsFolder;
		HTREEITEM m_hStructsFolder;
		HTREEITEM m_hRulesRecentlyFolder;

		//Карта перераспределения элементов
		typedef map<string_std, CClassTreeItemInfo*> mapped_items;
		mapped_items m_mapMappedItems;

		CClassTreeSkelInfo(IProjectDocLink & pProjectLink): CSkeletonInfo(pProjectLink){
		}

		//Связывает элемент с определенной папкой
		void MapItemToFolder(string_adapter szItemName, CClassTreeItemInfo* pFolder){
			m_mapMappedItems[szItemName.c_str()]=pFolder;
		}

		//Удаляет папку, а соответсвенно, разрушает все направления на нее
		void RemoveMapFolder(CClassTreeItemInfo* pFolder){
			for (mapped_items::iterator i = m_mapMappedItems.begin(); i!=m_mapMappedItems.end();)
				if ((*i).second==pFolder){
					m_mapMappedItems.erase(i); i=m_mapMappedItems.begin();
				}
				else
					++i;
		}

		//Удаляет поименованную связь
		void RemoveMapping(cstr szItemName){
			mapped_items::iterator i = m_mapMappedItems.find(szItemName); 
			if (i!=m_mapMappedItems.end())
				m_mapMappedItems.erase(i);
		}
		
		//Находит по имени элемента его принадлежность к папке
		CClassTreeItemInfo* FindMappedFolder(cstr szItemName){
			mapped_items::iterator i = m_mapMappedItems.find(szItemName);
			if (i==m_mapMappedItems.end())
				return NULL;
			return (*i).second;
		}

		override CClassTreeSkelInfo & operator = (CClassTreeSkelInfo & Src){
			CSkeletonInfo::operator = (Src);
			m_hRulesFolder	= Src.m_hRulesFolder;
			m_hFuncsFolder	= Src.m_hFuncsFolder;
			m_hStructsFolder= Src.m_hStructsFolder;
			return * this;
		}
	};
	//--------------------------------------------------------------------
	//Информация о данных каждого узла
	class CClassTreeItemInfo: public CProjectTree_B::CItemInfo
	{
		CClassTreeItemInfo * m_pNextPal; //Список всех "Собратьев"
		CClassTreeItemInfo * m_pPrevPal; //Можно объединять несколько элементов в такие списки
	public:
		//Конструктор
		CClassTreeItemInfo(CProjectTreeClass & TreeWnd,HTREEITEM hItem, CButtonState & btnState, ELevelItems LevelType, EPredefItems nType, DWORD nID, cstr szFilePath, CClassTreeSkelInfo*  pSkelInfo):
		  CItemInfo(TreeWnd,hItem, btnState, LevelType, nType, szFilePath, pSkelInfo){
			m_nID=nID;
			m_pNextPal=m_pPrevPal=NULL;
			TreeWnd.m_ItemCache.AddItem(this);
		}
		//Деструктор
		~CClassTreeItemInfo(){
			RemoveFromPals();
			m_pSkelInfo->RemoveMapFolder(this);
			static_cast<CProjectTreeClass*>(m_pTreeWnd)->m_ItemCache.RemoveItem(this);
		}

		//Собратья
		CClassTreeItemInfo * GetNextPal() {return m_pNextPal;}
		CClassTreeItemInfo * GetPrevPal() {return m_pPrevPal;}
		//Удалить себя из цепочки "друзей"
		void RemoveFromPals(){
			if (m_pNextPal){
				ASSERT(m_pNextPal->m_pPrevPal==this);
				m_pNextPal->m_pPrevPal=m_pPrevPal;
			}
			if (m_pPrevPal){
				ASSERT(m_pPrevPal->m_pNextPal==this);
				m_pPrevPal->m_pNextPal=m_pNextPal;
			}
			m_pPrevPal=m_pNextPal=NULL;
		}
		//Добавить себя в цепочку друзей. Сами мы при этому не должны состоять в цепочке
		//Становиться в цепочку перед указанноым Item
		void IncludeToPals(CClassTreeItemInfo * pItem){
			ASSERT(pItem!=this); RemoveFromPals(); 
			m_pNextPal=pItem;
			m_pPrevPal=pItem->m_pPrevPal;
			if (m_pPrevPal)
				m_pPrevPal->m_pNextPal=this;
			pItem->m_pPrevPal=this;
		}
		//Смотрит, есть ли у елемента друзья
		bool HasPals() const {return m_pNextPal || m_pPrevPal;}
		
		//Возвращает полную цепочку друзей и слева, и справа, включая себя самого
		void GetPalChain(vector<CClassTreeItemInfo*> & vPals){
			//Собираемя всех друзей справа и самого себя 
			for (CClassTreeItemInfo * pPalItem=this; pPalItem; pPalItem=pPalItem->GetNextPal())
				vPals.push_back(pPalItem);
			//Собираемя всех друзей слева
			for (CClassTreeItemInfo * pPalItem=this->GetPrevPal(); pPalItem; pPalItem=pPalItem->GetPrevPal())
				vPals.push_back(pPalItem);		
		}
		//Возвращает путь к проекту, укороченный вариант 
		cstr GetProjectPath() const {return __super::m_pSkelInfo->GetPath();}
		
		//Связывает элемент с папкой 
		void MapItemToMe(cstr szItemName) {m_pSkelInfo->MapItemToFolder(szItemName,this);}

		__declspec(property(get=get_Skel)) CClassTreeSkelInfo* m_pSkelInfo;
		__declspec(property(get=get_Type)) EPredefItems m_nType;

		CClassTreeSkelInfo* get_Skel() {return dynamic_cast<CClassTreeSkelInfo*>(__super::m_pSkelInfo);}
		EPredefItems get_Type() {return static_cast<EPredefItems>(__super::m_nType);}
	};

	//--------------------------------------------------------------------
	//Инкапсуляция методов по созданию списка Recently Used Rules
	class CRecentlyRules: public CWnd
	{
		bool				m_bRunning;
		UINT				m_nTimerID;
		CProjectTreeClass *	m_pWnd;
		CProjectTreeClass::CClassTreeItemInfo * m_pCatchedRule;
		CProjectTreeClass::CClassTreeItemInfo * m_pRecentlyAddedRule;
	public:	
		//Запуск и останов
		void Begin();
		void End();
		
		CClassTreeItemInfo * InsertItemIntoRecently(CClassTreeItemInfo * pItemInfo);

		CRecentlyRules(CProjectTreeClass * pOwnerWnd):	m_pWnd(pOwnerWnd),m_bRunning(false),m_nTimerID(0),m_pCatchedRule(NULL),
														m_pRecentlyAddedRule(NULL){
		}
	public:
		DECLARE_MESSAGE_MAP()
		afx_msg void OnTimer(UINT nIDEvent);
	};
	
	//--------------------------------------------------------------------
	//Инкапсуляция методов по сканированию файлов 
	class CScan
	{
	protected:
		//Используется для сохранения информации во врема сканирования. Для каждого
		//файла, входящего в проекта, создается подобная структура
		struct ScanData{
			string_std	strFilePath;		//С каким файлом работаем
			string_std	strProjectPath;		//какого  проекта
			FILETIME	ftLastModified;		//Для загрузки из файла
			time_t		tLastModified;		//Для открытых документов

			ScanData():tLastModified(-1){
				memset(&ftLastModified,0, sizeof(FILETIME));
			}
			ScanData (cstr szProjectPath,cstr szFilePath):tLastModified(-1),strFilePath(szFilePath),strProjectPath(szProjectPath){
				memset(&ftLastModified,0, sizeof(FILETIME));
			}
		};
		//Структура для передачи данных в поток
		struct ScanDataTransfer{
			ScanData*	in_pData;
			bool		in_bForcedRescan;
			CScan*		in_pSelf;
			
			bool		out_bGetIt;
			IParserLink::syntax_info out_SyntaxInfo;

			ScanDataTransfer(ScanData * pData, bool bForced, CScan* pObj):
				in_pData(pData),in_bForcedRescan(bForced),in_pSelf(pObj){
			}
		};

		typedef map<string_smart,ScanData> map_scan;
		string_smart		m_strCurProjectPath;
		string_smart		m_strCurFilePath;
		DWORD				m_nScanNo;		//порядковый номер очереного сканирования
		CProjectTreeClass *	m_pWnd;
		map_scan			m_mapScanData;
		volatile HANDLE		m_hCurrentScanThread; //Запущенный поток, который разбирает данные
		static CCriticalSection	m_CritSect;
		static bool			m_bScanning;

		//Выполнение разбора в потоке
		static DWORD WINAPI ScanThread(CScan * pSelf);
	public:
		CScan(CProjectTreeClass * pOwnerWnd):m_pWnd(pOwnerWnd),m_nScanNo(0),m_hCurrentScanThread(0) {
		}
		
		//Запуск и останов
		void Begin();
		void End();
		bool IsScanning() const {return m_bScanning;}
		CProjectTreeClass *	GetWnd() const {return m_pWnd;};
		
		//Ищет по имени файла и проекта структуру данных в карте
		ScanData * FindData(cstr szProjectPath, cstr szFilePath);
		//Добавляет структуру данных для указанного файла проекта
		ScanData * AddData(cstr szProjectPath, cstr szFilePath);
		//Удаляет структуру данных для указанного файла. Если структура для 
		//указанного файла не найдена, возвращает false;
		bool DeleteData(cstr szProjectPath, cstr szFilePath);

		virtual ~CScan();
		//Находить следующий не проверенный элемент 
		bool GetNextScanFile(string_smart & OUT strProjectName, string_smart & OUT strFileName);
		//Проверяет внеочередной указанный файл. bForcedRescan указывает, отключать ли оптимизацию сканирования
		void ScanFile(cstr szProjectPath, cstr szFilePath, bool bForcedRescan); 
		//сканирует все файлы в проекте
		void ScanWholeProject(cstr szProjectPath); 
		//Удаляет насканированную информацию о файле
		void DeleteFileInfo(cstr szProjectPath, cstr szFilePath); 
	};
	friend class CScan;

	//--------------------------------------------------------------------
	//Класс для сохранения структуры папок, настроенных пользователем
	class CFolderManager{
	private:
		CProjectTreeClass *	m_pWnd;
		
		string_smart GetDataFileName(CClassTreeSkelInfo * pProjectInfo);
	public:
		CFolderManager(CProjectTreeClass * pOwner): m_pWnd(pOwner){}
		
		//Считывает данные с диска
		bool OpenProject(CClassTreeSkelInfo * pProjectInfo);
		//Действия при закрытии проекта
		void CloseProject(CClassTreeSkelInfo * pProjectInfo);
		//Сохраняет данные указанного проекта
		bool SaveProject(CClassTreeSkelInfo * pProjectInfo);
	};

	//--------------------------------------------------------------------
	//Класс для увеличения производительности работы с деревом
	class CTreeItemCache{
	private:
		CProjectTreeClass *	m_pWnd;
		typedef list<CClassTreeItemInfo*> list_cache;
		list_cache m_lstCache;
	public:
		typedef list_cache::iterator iterator;

		CTreeItemCache(CProjectTreeClass * pOwner): m_pWnd(pOwner){
		}
		void AddItem(CClassTreeItemInfo * pItemInfo);
		void RemoveItem(CClassTreeItemInfo * pItemInfo);

		iterator FirstPos() {return m_lstCache.begin();}
		iterator EndPos()	{return m_lstCache.end();}

		CClassTreeItemInfo * FindByName(cstr szName);
		CClassTreeItemInfo * FindByLocation(CClassTreeSkelInfo * pProjectInfo, cstr szFilePath, size_t nPosInText);
		CClassTreeItemInfo * Find(ELevelItems nItemLevel, EPredefItems nItemType, cstr szFilePath,CClassTreeSkelInfo * pSkelInfo,cstr szItemName);
	};

public:
	//Добавляем костяк дерева для проекта
	override bool AddProjectHeader(IProjectDocLink & ieProjectLink);
	//Удаляем весь проект
	override bool RemoveProject(LPCTSTR szProjectPath);
	//Возвращает основные данные о проекте по его имени
	override CClassTreeSkelInfo*  GetProjectHeader(LPCTSTR szProjectPath);
//	//Перекрывает стандартный метод для того, чтобы произвести преобразование типов
//	CClassTreeSkelInfo* GetProjectHeader(cstr szToProject){
//		return dynamic_cast<CClassTreeSkelInfo*> (__super::GetProjectHeader(szToProject));
//	}

	//Эта процедура знает, как правильно сформировать имя в зависимости от уровня
	string_smart DecorateName(cstr szText, ELevelItems nLevel, bool bModified);
    
	CProjectTreeClass();
    virtual ~CProjectTreeClass();
protected:
	typedef map<string_smart, CClassTreeSkelInfo> map_skel;

	map_skel		m_mapProjSkel;
	ESortMode		m_nSortMode;
	CScan			m_Scaner;
	CRecentlyRules	m_RecentlyRules;
	CFolderManager	m_FolderManager;
	CTreeItemCache	m_ItemCache;
	CImageList		m_Images;

public:
	//Перекрывает стандартный метод для того, чтобы произвести преобразование типов
	CClassTreeItemInfo * GetItemData(HTREEITEM hItem){
		return dynamic_cast<CClassTreeItemInfo *>(__super::GetItemData(hItem));
	}
	//Данные беруться из кеша, что гораздо производительнее,чем поиск по дереву
	//Элемент ищется не относительно какой-то родительской папки, а вообще по
	//всему пространству, но с учетом родительского проекта
	CClassTreeItemInfo * FindItemUniq(ELevelItems nItemLevel, EPredefItems nItemType, cstr szFilePath,CClassTreeSkelInfo * pSkelInfo,cstr szItemName){
		return m_ItemCache.Find(nItemLevel, nItemType, szFilePath,pSkelInfo,szItemName);
	}
	//Перекрывает стандартный метод для того, чтобы произвести преобразование типов
	bool GetCurItemData(CClassTreeItemInfo * & pInfo) const {
		CItemInfo* pI; return __super::GetCurItemData(pI) && ((pInfo = dynamic_cast<CClassTreeItemInfo *>(pI))!=NULL);
	}

	//Элементарная операция создания элемента
	//Необьходимо пользоваться ею а не InsertItem. Создает элемент и сразу к нему - 
	//класс CClassTreeItemInfo, хранящий все данные. Перед добавлением проверяет уникальность
	//создаваемого элемента среди дочерних элементов своего родителя. Если найден
	//идентичный элемент - возвращает его. Для удаления элемента нужно пользоваться DeleteItem
	CClassTreeItemInfo * AddItem(cstr szItemName, CButtonState & btnState, HTREEITEM hParent, HTREEITEM hInsertAfter,
								 DWORD nID,ELevelItems LevelType, EPredefItems nType, cstr szFilePath, CClassTreeSkelInfo*  pSkelInfo, bool bCheckOnExist=true);

	//Добавление папки или конечного элемента. Добавляет элемент в том случае, если такового нет среди детей 
	//Если добавляется iidProjectRoot, szToProject игнорируется
	//Нельзя передавать nType==iidUserItem или nType==iidUserFolder
	CClassTreeItemInfo * AddPredefinedItem(cstr szName,cstr szToProject, EPredefItems nType, cstr szFilePath, DWORD nID,HTREEITEM hInsertAfter=TVI_LAST);

	//Более удобная форма AddPredefinedItem для добавления синтаксической единицы
	CClassTreeItemInfo * AddSyntaxItem(IParserLink::syntax_item & SItem, cstr szToProject, DWORD nID, HTREEITEM nInsertAfter=TVI_LAST);

	//Функция для добавления "Произвольного" элемента (nType==iidUserItem или nType==iidUserFolder)
	CClassTreeItemInfo * AddUserItem(cstr szName,EUserItems nType, cstr szFilePath, DWORD nID,HTREEITEM hParent, HTREEITEM hInsertAfter=TVI_LAST);

	//Удаляет элементы, если у него ID!=указанному и при этом m_strFilePath==указанному
	void	SiftSyntaxItems(cstr szProjectPath, cstr szFilePath, DWORD nID);
	//Сортирует элементы дерева указанным способом(утилитная функция)
	void	SortItems(HTREEITEM hParent, ESortMode nSortMode);
	//Сортирует весь указанный проект
	void	SortProject(cstr szProjectPath);
	//Обновляет в CItemInfo параметр hItem для всех элементов начиная от указанного родительского
	void	RefresItemHandles(HTREEITEM hParent=NULL);

	//Определяет по указанной позиции и названию документа синтакцический элемент
	//в тексте
	CClassTreeItemInfo * FindSyntaxItem(cstr szProjectName, cstr szFileName, long nPos);

	//Виртуальные функции для обработки событий Drag&Drop
	override bool OnBeginDrag(POINT ptDragPoint, const TVITEM & ItemData);
	override bool OnDragOver(POINT ptDragPoint, const TVITEM & ItemData, HWND hOverWnd,HTREEITEM hOverItem, bool & bUseStdCursors);
	override void OnDragDrop(POINT ptDragPoint, const TVITEM & ItemData, HWND hOverWnd,HTREEITEM hOverItem);

	//Очищает целиком все дерево
	override void Clear();

	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &);//Открытие очередного документа проекта
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &);//Изменение состояния документа проекта
	override void IDocumentLink::IEventSink::evOnCloseDocument (evdCloseDocumentInfo &);
	override void ISourceViewLink::IEventSink::evOnChangeCursorPos (ISourceViewLink * m_pSender, size_t m_nPos);
	override void OnCloseAppMainWnd();

	//IClassExplorerLink
	override bool IClassTreeLink::SelectSyntaxItem(cstr szProjectName, cstr szFileName, long nPos);

    // Generated message map functions
    //{{AFX_MSG(CProjectTreeClass)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void cmdOnViewSort();
	afx_msg void cmdupOnViewSort(CCmdUI* pCmdUI);
	afx_msg void cmdOnGotoDefinition();
	afx_msg void cmdOnClwClearRecentlyList();
	afx_msg void cmdupOnClwClearRecentlyList(CCmdUI *pCmdUI);
	afx_msg void cmdOnNewFolder();
	afx_msg void cmdupOnNewFolder(CCmdUI *pCmdUI);
	afx_msg void cmdOnClwRename();
	afx_msg void cmdupOnClwRename(CCmdUI *pCmdUI);
	afx_msg void cmdOnClwDelete();
	afx_msg void cmdupOnClwDelete(CCmdUI *pCmdUI);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ClassTreeCtrl_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_)
