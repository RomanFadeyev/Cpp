/********************************************************************
	created:	2002/02/23
	created:	23:2:2002   12:59
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\ProjectDoc.h
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	ProjectDoc
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Реализация документа проекта (Project)
*********************************************************************/

#pragma once

#include <algorithm>
#include <map>
#include <BaseUtils.h>
#include <PropSet.h>
#include <MFCUtils.h> 
#include <GUIService.h> 
#include <AppUtils.h> 
#include <AppMessages.h> 

#include "Names.h"
#include "Resource.h"
#include "CObjectEx.h"
#include "BaseStructs.h"

#include "COMProjectDocument.h"	//COM

using namespace base_utils; 
using namespace mfc_utils;
using namespace properties;
using namespace std;


/////////////////////////////////////////////////////////////////////////////
// CProjectDoc document

//Документ проекта одновременно входит и в систему CO. Имя задается ему на основе
//глобального счетчика объекта
class CProjectDoc : public CDocumentEx, public CCObjectEx, 
					public IProjectDocLink, 
					public IDocumentLink::IEventSink
{
public:
	//Тип списка файлов, входящих в проект
	typedef list<string_smart> list_sources; 
	typedef list_sources::iterator it_file;
	typedef list_sources::const_iterator cit_file;
	typedef string_list	list_libs;
	typedef string_list	list_lang_gen;
	
	typedef IDebugManageLink::vector_breakpoints::iterator dml_vb_i;
	typedef IDebugManageLink::vector_breakpoints::const_iterator dml_vb_ci;
	//Признак выполнения очередной операции
	enum op_state  {opNone=0, opOpening=1, opSaving=2, opCreating=4, opClosing=8, opChangingBreakpoints=16};

	//Класс, занимающийся чтением и сохранением файлов проекта
	class CProjectProps:public property_set
	{
	public:
		//Загружает образ файла из архива
		void ReadFromArchive(CArchive & ar);
	};
	//Класс - утилита, позволяющий легко и просто добавлять и удалять состояния документа 
	//Удобно использовать в пределах процедуры. Основной плюс - корректное снятие
	//установленного флага даже при возникновения исключения
	class CProjectAutoState
	{
	private:
		CProjectDoc & m_Doc;
		long		m_nState;
		bool		m_bRolledBack;
	public:
		CProjectAutoState(CProjectDoc & Doc, long nState):m_Doc(Doc),m_nState(nState), m_bRolledBack(false){
			ASSERT(!(m_Doc.m_nOpState & m_nState));
			m_Doc.m_nOpState|=m_nState;
		}
		void Rollback(){	//Снимает состояние раньше времени.
			if (m_bRolledBack)
				return;
			ASSERT_VALID(&m_Doc);
			m_Doc.m_nOpState&=~m_nState;
			m_bRolledBack = true;
		}
		~CProjectAutoState(){
			Rollback();
		}
	};
	friend class CProjectAutoState;
	
	//Струтура, хранящая описание последнего открытого документа, к который
	//пытались найти
	struct CLastAccessedDoc{
		string_smart m_strFilename;
		ISourceDocLink * m_pSourceLink;
		CLastAccessedDoc():m_pSourceLink(NULL){}
		void Clear(){
			m_pSourceLink=NULL;
			m_strFilename.erase();
		}
		void Assign(ISourceDocLink * pSourcepLink){
			m_pSourceLink=pSourcepLink;
			m_strFilename=pSourcepLink->GetPath();
		}
		bool IsAppropriate(cstr szSource){
			return (szSource) && (m_pSourceLink) && (m_strFilename.cmpi(szSource));
		}
	};
	//--------------------------------------------------------------------
	//интерфейс IProjectDocLink нашего документа проекта с другими объектами

	//From CDocumentEx
	override IDocumentLink* GetDefaultInterface()		{return this;}

	//Multiple Instance Link
	override size_t GetObjectID()const	{return CDocumentEx::GetObjectID();}

	//From IDocumentLink
	override CDocumentEx* GetObject()					{return this;}	//Указатель на документ

	override bool Close();						//Закрывает документ
	override bool Save();						//Сохраняет документ
	override bool SaveAs(cstr szNewFileName);	//Сохраняет документ
	override string_smart GetPath() const;		//Возвращает путь к проекту
	override string_smart GetName() const;		//Возвращает название проекта
	override string_smart GetExt()  const;		//Возвращает расширение текущего пути
	override bool IsReadOnly() const;
	
	//From IProjectDocLink
	override ref_cnt_ptr<COM::IDocument> GetComInterface(){return GetComInterface2().get();}
	override ref_cnt_ptr<COM::IProjectDocument> GetComInterface2();
	override int  GetFileType(cstr szSource) const;	//Возвращает атрибут принадлежности файла к какой-либо группе файлов
	override void SetNotes(cstr szNotes);		//Модификация заметок
	override void SetParams(cstr szParams);		//Модификация строки параметров	
	override void SetModify(bool);				//Устанавливает флаг изменения документа
	override string_smart GetSwitches() const;	//Возвращает ключи компиляции
	override void SetSwitches(cstr szSwitches);	//Модифицирует ключи компиляции
	override void GetBuildSteps(OUT list_build_step & PreBuildSteps, OUT list_build_step & PostBuildSteps); 	//Получение списка дополнительных действий при компиляции
	override void SetBuildSteps(const list_build_step & PreBuildSteps, const list_build_step & PostBuildSteps); 	//Получение списка дополнительных действий при компиляции

	//Возвращает имена транслируемого языка и выходного языка
	//нужно для базы знаний.
	override string_smart GetSourceLanguage() const;
	override string_smart GetTargetLanguage() const ;
	override void SetSourceLanguage(cstr szName);
	override void SetTargetLanguage(cstr szName);

	override string_smart GetInputTranslationDir() const;
	override string_smart GetOutputTranslationDir() const;
	override void SetInputTranslationDir(string_adapter szName);
	override void SetOutputTranslationDir(string_adapter szName) ;

	//Имя info-файла, генерируемого компилятором
	override string_smart GetCompilerInfoFilename() const;
	override void SetCompilerInfoFilename(string_adapter szFilename);

	override parse_report_type GetParseReportType() const;//Возвращает режим трассировки грамматического разбора 
	override string_smart GetParseLogPath() const; //Путь к файлу, который будет использоваться для сброса log-файла разбора
	override bool GetParseStopAtMessage() const;//Возвращает true, если необходимо останавливаться на каждом сообщении от ядра
	override void SetActive(bool);				//Устанавливает флаг изменения документа
	override bool GetActive() const;			//Указывает, является ли данный проект активным
	override void SaveAll();					//Сохраняет докумен+все его исходники
	override bool RemoveFile(cstr szSource);
	override bool AddFile(cstr szFileName);
	override IDocumentLink* AddNewFile();
	override bool AddRule();
	override bool IsFilePresent(cstr szFileName) const;//Проверяет, входит ди в состав проекта указанный файл
	override string_smart GetNotes() const;		//Получение строки заметок	
	override string_smart GetParams()const;		//Получение строки параметров
	override string_smart GetExePath()const;	//Возвращает путь к скомпилированному исполняемому файлу
	
	//Директория выходных файлов, может быть относительный путь
	override string_smart GetOutputDirectory() const;
	override void SetOutputDirectory(string_adapter szDirectory);
	override string_smart GetOutputDirectoryFull()const;//Возвращает директорию, всегда  c конечным \, где будут создаваться исполняемые файлы
	//Директория промежуточных файлов, может быть относительный путь
	override string_smart GetIntermediateDirectory() const;
	override void SetIntermediateDirectory(string_adapter szDirectory);
	override string_smart GetIntermediateDirectoryFull()const;//Возвращает директорию, всегда  c конечным \, где будут создаваться исполняемые файлы

	override cstr GetFileFirst() const;			//Возвращает первый файл в списке файлов проекта 
	override cstr GetFileNext() const;			//Возвращает следующий файл по списку 
	override void GetFiles(OUT string_list & lstFileList, int nFileType) const;
	override void GetLibs(OUT string_list & lst) const;//Возвращает список подключаемых библиотек
	override void GetLangGens(OUT string_list & lst) const; //Возвращает список назначенных генераторов кода на ЯП 
	override void SetLangGens(const string_list & lst);		//Устанавливает список назначенных генераторов кода 
	override void Build();						//Запускает процесс компиляции самого себя
	override void Clean();						//Удаление всех файлов компилятора
	override void Run();						//Запускает процесс выполнения самого себя
	override void Debug();						//Запускает процесс отладки
	override void AddToKnowledgeBase();			//Добавляет себя в базу знаний транслятора
	override IDebugManageLink* GetDebugManageLink(cstr szSource) const;
	override bool IsBreakpointExists(cstr szSource, size_t dwLine, BREAKPOINT & bp) const;
	override bool GetBreakPoint(cstr szSource, long dwLine, BREAKPOINT & Breakpoint);
	override void GetBreakPoints(IDebugManageLink::vector_breakpoints & Breakpoints);
	override void SetBreakPoint(cstr szSource, BREAKPOINT & Breakpoint); //Установка точки останова для указанного модуля (значение имени модуля внутри структуры BREAKPOINT игнорируется)
	override void ResetBreakPoint(cstr szSource, long dwLine); //Снятие точки останова для указанного модуля в указанной строке
	//--------------------------------------------------------------------
protected:
	COM::ProjectDocument* m_pCoClass;	//Это - СоClass для представления документа в COM'e
	friend class COM::AbstractDocument;

	CProjectDoc();				// protected constructor used by dynamic creation
	override  ~CProjectDoc();	
	DECLARE_DYNCREATE(CProjectDoc)

	//Допорлнительные данные к каждому файлу исходника
	struct CFileSourceState{
		struct CViewPos{
			long dwPos;
			long dwFVL;
			long dwFCV;
			CViewPos():dwPos(0),dwFVL(0),dwFCV(0){}
		};
		typedef vector<long> bookmark_vector;
		typedef IDebugManageLink::vector_breakpoints breakpoint_vector;
		
		breakpoint_vector m_Breakpoints; //Список всех точек останова
		bookmark_vector m_Bookmarks; //Список всех точек останова
		CViewPos		m_Pos;
		__time64_t		m_TimeBuild;	//Время модификации файла в момент последней компиляции
		CFileSourceState():m_TimeBuild(0)
		{}
	};

	static	long	g_nID;			//Статический счетчик документов, нужен для получения уникального имени объекта
	mutable it_file	m_itFile;		//Итератор перебора файлов(временный)	
	string_std		m_strNotes;
	string_std		m_strParams;
	string_std		m_strSwitches;
	string_std		m_strTargetLanguage;
	string_std		m_strSourceLanguage;
	string_std		m_strParamsBrowseDir; //Каталог по умолчанию для задания параметров. Используется только при вызове диалога ParamsDlg
	string_std		m_strWorkDir;	//Рабочая директория, где расположен проект
	string_std		m_strPath;		//Полный путь, сюда входит m_strWorkDir. Аналогично GetPathName; Введено, в основном, из-за ссылки в ifaceProjectData 
	string_std		m_strTitle;
	list_sources	m_lstSources;
	list_libs		m_lstLibs;
	list_lang_gen	m_lstLangGens;
	list_build_step	m_lstPreBuildSteps;
	list_build_step	m_lstPostBuildSteps;
	string_std      m_strInputTranslationDir;
	string_std		m_strOutputTranslationDir;
	string_std		m_strCompilerInfoFilename;
	string_std		m_strOldCompilerInfoFilename;
	string_std		m_strOutputDirectory; //Директория, куда будут складываться все файлы. Чаще всего она относительная 
	string_std		m_strIntermediateDirectory; //Директория, куда будут складываться вспомогательные файлы. Чаще всего она относительная 

	bool			m_bShowParamsDialog; //Показывать ли перед запуском диалог параметров
	static int		m_nParsingReportType;
	static bool		m_bParsingReportToFile;
	static bool		m_bParsingStopAtMessage;
	
	long			m_nOpState;
	mutable CLastAccessedDoc m_LastAccessedDoc;
	typedef map<string_smart,CFileSourceState> filedata_map;
	filedata_map m_mapFileData;

	//Возбуждает событие изменения, если состояние документа к тому располагает
	void RaiseChangeEvent(evdChangeProjectInfo & Info);
	//Возбуждает событие открытия
	void RaiseOpenEvent(evdOpenProjectInfo & Info);
	//Возбуждает событие закрытия
	void RaiseCloseEvent(evdCloseProjectInfo & Info);
	//Возвращает итератор в списке файлов, входящих в проект, на найденный файл
	cit_file FileFind(cstr  szFileName) const;
	//Закрывает документ указанного файла, если он открыт
	bool FileClose(cstr szFileName);
	//Пытается сохранить несохраненный документ указанного файла, если он открыт
	bool FileSaveModified(cstr szFileName);
	//Добавляет файл в проект
	bool FileAdd(cstr szFileName);
	//Удаляет файл из проекта
	bool FileRemove(cstr szFileName);
	//Активизрует проект (делает его активным среди всех остальных)
	void Activate(bool bForceActiation=false);
	//Деактивация проекта
	void Deactivate();
	//Функция-утилита. Возвращает интерфейс к менеджеру проектов
	IDocMngrLink* GetDocManagerInterface() const;
	//Функция-утилита. Возвращает интерфейс к Builder
	IBuilderLink * GetBuilderInterface() const;
	//Функция-утилита. Возвращает интерфейс к Runner
	IRunnerLink* GetRunnerInterface() const;
	//Функция-утилита. Возвращает интерфейс к Source-документу, при этом он должен входить в наш проект
	ISourceDocLink* GetSourceInterface(cstr szFilePath) const;	
	//Запоминает в внутренних структурах разнообразные настройки файла-исходника
	bool RestoreSourceFileState(cstr szFilePath);
	//Восстанавливает разнообразные настройки файла-исходника
	bool StoreSourceFileState(cstr szFilePath);
	//Загружает состояние документов проекта. Данные берутся из 
	//одноименного проекту файла с расширением .dsk. Вызывается только 
	//при открытии проекта
	void LoadDSK();
	//Сохраняет состояние открытых документов проекта. Вызывается только 
	//при закрытии проекта + метод Save()
	void SaveDSK();
	//Указывает, готов ли Exe-файл к запуску. Возвр: IDYES-готов, IDNO - не готов, IDCANCEL - отмена
	int IsNeedCompile(bool bForce=FALSE);
	//Обратная функция для билдера. Если билдер вернул ОК, запускаем Runner
	void BuildWaitToRun(bool Succesful);
	//Обратная функция для билдера. Если билдер вернул ОК, добавляем грамматику в базу знаний
	void BuildWaitToAddToKnowledgeBase(bool Succesful);
	//Обратная функция для билдера. Если билдер вернул ОК, запускаем Runner
	void AddToKnowledgeBaseInternal();
	//Внутренняя функция запуска модуля
	void RunInternal();
	//Обратная функция для билдера. Если билдер вернул ОК, запускаем Runner
	void BuildWaitToDebug(bool Succesful);
	//Внутренняя функция запуска модуля
	void DebugInternal();
	//Проверяет у указанного документа исходных текстов допустимость всех точек останова
	void CheckBreakpointsUsed(cstr szFilePath);
	//Данный метод нужно вызывать перед каждой компиляцией
	bool PreBuild();
	
	//Обработчики глобальных событий
	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &);//Открытие очередного документа проекта
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &);//Изменение состояния документа проекта
	override void IDocumentLink::IEventSink::evOnCloseDocument (evdCloseDocumentInfo &);
	
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual POSITION GetFirstViewPosition() const;
	override BOOL SaveModified();
	override BOOL OnNewDocument();
	override BOOL OnOpenDocument(LPCTSTR lpszPathName);
	override void OnCloseDocument();
	bool OnCloseDocumentInternal();
	override BOOL OnSaveDocument(LPCTSTR lpszPathName);
	override void OnIdle();
	override void SetModifiedFlag(BOOL bModified = TRUE);
	override BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);	
	override void LoadSettings();
	override void SaveSettings();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CProjectDoc)
	afx_msg void cmdOnClose();
	afx_msg void cmdOnSave();
	afx_msg void cmdOnBuild();
	afx_msg void cmdupOnBuild(CCmdUI *pCmdUI);
	afx_msg void cmdOnDebugStart();
	afx_msg void cmdupOnDebugStart(CCmdUI *pCmdUI);
	afx_msg void cmdOnRunStart();
	afx_msg void cmdupOnRunStart(CCmdUI *pCmdUI);
	afx_msg void cmdOnDebugStop();
	afx_msg void cmdupOnDebugStop(CCmdUI *pCmdUI);
	afx_msg void cmdOnBuildStop();
	afx_msg void cmdupOnBuildStopBuild(CCmdUI *pCmdUI);
	afx_msg void cmdOnUnloadProject();
	afx_msg void cmdupOnUnloadProject(CCmdUI *pCmdUI);	
	afx_msg void cmdOnAddRule();
	afx_msg void cmdOnAddFiles();
	afx_msg void cmdOnAddCodeTemplate();
	afx_msg void cmdOnSettings();
	afx_msg void cmdOnStatistic();	
	afx_msg void cmdupOnSettings(CCmdUI *pCmdUI);	
	afx_msg void cmdOnClean();
	afx_msg void cmdOnSyntaxDiagrams();
	afx_msg void cmdOnDebugStepInto();
	afx_msg void cmdupOnDebugStepInto(CCmdUI *pCmdUI);
	afx_msg void cmdOnDebugStepOver();
	afx_msg void cmdupOnDebugStepOver(CCmdUI *pCmdUI);
	afx_msg void cmdOnParsingCompleteReport();
	afx_msg void cmdupOnParsingCompleteReport(CCmdUI *pCmdUI);
	afx_msg void cmdOnParsingReportToFile();
	afx_msg void cmdupOnParsingReportToFile(CCmdUI *pCmdUI);
	afx_msg void cmdOnParsingShortReport();
	afx_msg void cmdupOnParsingShortReport(CCmdUI *pCmdUI);
	afx_msg void cmdOnDebugPauseOnParse();
	afx_msg void cmdupOnDebugPauseOnParse(CCmdUI *pCmdUI);
	afx_msg void cmdOnAddToKnowledgeBase();
	afx_msg void cmdOnProjectBuildSyntaxDiagram();
	afx_msg void cmdOnProjectBuildSyntaxTree();
	afx_msg void cmdOnExportToXBNF();
	afx_msg void cmdOnProjectNewSource();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
