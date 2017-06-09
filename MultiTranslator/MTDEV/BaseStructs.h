/********************************************************************
	created:	2002/02/03
	created:	3:2:2002   23:02
	filename: 	$(Projects)\Sources\MTDEv\BaseStructs.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	BaseStructs
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Основные базовые структуры для передачи в глаголах, событиях 
				и т.п.
*********************************************************************/

#ifndef HEADER_BaseStructs
#define HEADER_BaseStructs

#include <list>
#include <vector>

#include <BaseUtils.h>
#include <IObject.h>	// concept objects
#include <Functors.h>

#include "ComInterfaces.h"

#pragma warning(disable: 4786) // For STL routines

using namespace std;
using namespace i_objects;
using namespace base_utils;

//Дополнительные сообщения 
enum EWindowMessages {CM_BASE=0xB000, 
					CM_SHOW,	//Без параметров
					CM_HIDE,	//Без параметров
					CM_ACTIVATE,//Без параметров
					CM_REALIGN,	//Без параметров
					CM_POST		//Можно использовать по произволному назначению
					};
//--=================== Structures =====================================--

struct IDebugManageLink;

//Этот макрос используется для того, чтобы запретить удаление объекта 
//указанного класса а также его оператор = 
//!!! Следующий после данного макроса код явл public
#define PURE_INTERFACE(class_x) \
protected: ~class_x(){} \
class_x & operator = (const class_x &){}\
public:


//--------- элементы COM --------------------------------------------------
#define DEFINE_ADD_REF_STUB ULONG __stdcall AddRef( void)  {return 0;}
#define DEFINE_RELEASE_STUB ULONG __stdcall Release( void) {return 0;}
#define BEGIN_QUERY_INTERFACE virtual HRESULT __stdcall QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {
#define COM_INTERFACE_SUPPORT(x) if (riid==__uuidof(x)) {*ppvObject=static_cast<x*>(this); return S_OK;}
#define END_QUERY_INTERFACE return E_NOINTERFACE; }
//--------- элементы COM --------------------------------------------------

/*
class e_abort_operation: public std::exception
{
public:
e_abort_operation(string_adapter strCause): std::exception(strCause){}
};
*/

//Описание точки останова
struct BREAKPOINT{
//protected:
	IDebugManageLink * m_pOwner;
	string_smart m_strModuleName;	//название модуля, хранящего данный объект. Заполняется не всегда
	long dwHandle;	//декриптор точки останова
	bool bEnabled;
	long dwLine;
	long dwHitCount;
	string_smart strCondition;

	friend struct IDebugManageLink;
public:
	BREAKPOINT(IDebugManageLink *pOwner,string_adapter szModuleName);
	BREAKPOINT(IDebugManageLink *pOwner,string_adapter szModuleName, long dwL, bool bE=true, cstr szC=NULL, long dwHC=0); 
	BREAKPOINT(const  BREAKPOINT & src);
	BREAKPOINT & operator = (const BREAKPOINT & src);
	
	//Доступ к членам
	bool IsEnabled()			const	{return bEnabled;}
	long GetLine()				const	{return dwLine;}
	long GetHandle()			const	{return dwHandle;}	
	long GetHitCount()			const	{return dwHitCount;}
	string_smart GetCondition()	const	{return strCondition.c_str();}
	string_smart GetModuleName()const	{return m_strModuleName.c_str();}
	
	//void SetLine(long nLine)				{dwLine=nLine;}	
	//void SetModuleName(cstr szModuleName)	{m_strModuleName = szModuleName;}
	//void SetOwner(IDebugManageLink *pOwner)	{m_pOwner = pOwner;}
	//void SetHandle(long nHandle)			{dwHandle = nHandle;}
	
	void Enable(bool bEnabled);
};
//Описание пакета с данными для каждого элемента в WatchList
struct WATCHITEM{
	typedef vector<WATCHITEM> vector_items;
	enum kind{wikSimple,wikStruct,wikArray,wikFunction};
	string_smart strName;	//Для функции = это имя функции
	string_smart strType;
	string_smart strContext;//Для функции - это название модуля
	size_t		 nLine;
	size_t		 nCol;
	kind		 nKind;
	variant		 value;			//для wikStruct это значение не определено
	vector_items ChildItems;	//Элементы вложенные элементы 
};

//Определяет положение чего-то путем указания названия этого файла и позиции в нем
struct ITEMLOCATION{
	string_smart strFileName;
	size_t		 nStartPos;
	size_t		 nEndPos;
	
	ITEMLOCATION():nStartPos(0),nEndPos(0){
	}
	ITEMLOCATION(cstr szFileName, size_t anStartPos, size_t anEndPos): 
	strFileName(szFileName), nStartPos(anStartPos), nEndPos(anEndPos){
	}
};

//определяет название документа и обрабатываемый им фильтр
struct DOCUMENT_INFO
{
	string_std m_strDocTitle;
	string_std m_strDocFilter;

	DOCUMENT_INFO(){
	}
	DOCUMENT_INFO(string_adapter strDocTitle, string_adapter strDocFilter)
		:m_strDocTitle(strDocTitle.c_str()), m_strDocFilter(strDocFilter.c_str()){
	}
};

//Этот интерфейс используется каждой точкой останова для 
struct IDebugManageLink
{
	typedef vector<BREAKPOINT> vector_breakpoints;
	virtual  void SetBreakPoint(BREAKPOINT & Breakpoint) PURE;
	virtual  void ResetBreakPoint(long dwLine) PURE;	
	virtual  bool GetBreakPoint(long dwLine, BREAKPOINT & Breakpoint) PURE;	
	virtual  void GetBreakPoints(vector_breakpoints & Breakpoints) PURE;
	virtual  void SetCommandMarker(long dwLine) PURE;
};

//--======================= INTERFACES =================================--
interface IDocumentLink;

//Расширение шаблона документа
class CDocumentTemplateEx:public CMultiDocTemplate{
public:
	CDocumentTemplateEx(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
	{
	}

	typedef list<DOCUMENT_INFO> lst_doc_info;
	virtual lst_doc_info GetAdditionalSupportedDocs() const {
		return lst_doc_info();
	}
};

//Базовый класс для всех видов документов. Обратите внимание на защищенное 
//наследование. Таким образом мы ограничиваем область применения выдаваемого 
//CDocumentEx * в IDocumentLink (см ниже). Внимание! Все классы документов
//должны наследоваться от данного класса, а не от CDocument. 
class CDocumentEx: public CDocument{
private:
	static size_t m_nCounter;
	size_t m_nID;
protected:
	bool		m_bReadOnly;
	__time64_t	m_tLastSaveAttempt;

	size_t GetObjectID() const {return m_nID;}

	CDocument::GetTitle;
	CDocument::SetTitle;
	//CDocument::GetPathName;
	CDocument::SetPathName;
	CDocument::GetDocTemplate;
	//CDocument::IsModified;
	CDocument::SetModifiedFlag;
	CDocument::AddView;
	CDocument::RemoveView;
	CDocument::GetFirstViewPosition;
	CDocument::GetNextView;
	CDocument::UpdateAllViews;
	CDocument::OnChangedViewList;
	CDocument::DeleteContents;
	//CDocument::OnNewDocument;
	//CDocument::OnOpenDocument;
	//CDocument::OnSaveDocument;
	//CDocument::OnCloseDocument;
	CDocument::ReportSaveLoadException;
	CDocument::GetFile;
	CDocument::ReleaseFile;
	CDocument::CanCloseFrame;
	CDocument::SaveModified;
	CDocument::PreCloseFrame;
public:
	//From CDocument
	override BOOL OnNewDocument();
	override BOOL OnOpenDocument(LPCTSTR lpszPathName);	//Почему public в CDocument?
	override BOOL OnSaveDocument(LPCTSTR lpszPathName);
	override void SetTitle(LPCTSTR lpszTitle);
	override BOOL DoFileSave();

	bool IsReadOnly() const						{return m_bReadOnly;}
	__time64_t GetLastSaveAttemptTime() const	{return m_tLastSaveAttempt;} //may return 0

	virtual	IDocumentLink* GetDefaultInterface() PURE;
	CDocumentEx(): m_nID(++m_nCounter),m_bReadOnly(false),m_tLastSaveAttempt(0){}
};

//Вспомогательный класс для вывода в статусной строке текста и автоматического
//его удаления
class CStatusProcessMessage
{
	string_smart m_strPreviosMessage;
public:
	CStatusProcessMessage (string_adapter strMessageStart);
	~CStatusProcessMessage();
};

//Базовый интерфейс для всех видов интерфейсов документов
interface IApplicationLink: public ILink
{	PURE_INTERFACE(IApplicationLink);

	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		virtual void evOnCreateApp()PURE;		//Создание приложения (всегда вызывается)
		virtual void evOnCreateAppMainWnd()PURE;	//Создание главной формы
		virtual void evOnRunApp()PURE;			//Отображение главной формы и запуск цикла сообщений (не всегда вызывается)
		virtual void evOnStopApp()PURE;			//Остановка приложения, парный evOnRunApp
		virtual void evOnDestroyApp()PURE;		//Разрушение приложения, парный evOnCreateApp
	};

	//Обработчик IDLE
	interface IIdleHandler
	{
		PURE_INTERFACE(IIdleHandler);
		virtual void OnIdle(bool & IN OUT bStopIdle) PURE;	//if bStopIdle = true, application stops sending the message to another objects
	};

	//Включение указанного окна в обработку сообщений OnIdle
	virtual bool IncludeToOnIdleMap(IIdleHandler* pHandler)  PURE;
	//Включение указанного окна в обработку сообщений OnCmdMsg
	virtual bool ExcludeFromOnIdleMap(IIdleHandler* pHandler)  PURE;

	//Закрытие приложения. Не всегда может срабатывать - кто-то может 
	//не дать разрешение на закрытие приложение. Если bForcedClose = true,
	//то закрывает в любом случае
	virtual bool Quit(bool bForcedClose=false) PURE; 
	//Вызов данной функции запрещает или разрешает работу AfxMessageBox. Кол-во 
	//запретов и разрешений должно строго соответствовать
	virtual void SupressMessageBoxes(bool bSupress) PURE;

	//Дать дескриптор ветки в реестре с учетом версии мультитранслятора
	virtual registry GetAppVersionBasedRegistryKey() PURE;
	
	//Дать дескриптор ветки в реестре без учета версии мультитранслятора
	virtual registry GetAppSteadyRegistryKey() PURE;

	//COM Interface
	virtual ref_cnt_ptr<COM::IApplication> GetComInterface() PURE; 
	
};

//Структура, указывающая, что документ XXX изменил состяние
struct evdChangeDocumentInfo;
//Структура, указывающая, что документ XXX открывается
struct evdOpenDocumentInfo;
//Структура, указывающая, что документ XXX закрывается
struct evdCloseDocumentInfo;

//Базовый интерфейс для всех видов интерфейсов документов
//Multiple Instance!!!
interface IDocumentLink: public IMultiInstanceILink
{
	PURE_INTERFACE(IDocumentLink);

	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		virtual void evOnOpenDocument  (evdOpenDocumentInfo &) PURE;
		virtual void evOnChangeDocument(evdChangeDocumentInfo &) PURE;
		virtual void evOnCloseDocument (evdCloseDocumentInfo &) PURE;
	};

	//Указатель на документ
	virtual	CDocumentEx* GetObject() PURE;
	//Возвращает путь к документу
	virtual string_smart GetPath() const  PURE;
	//Возвращает название документа
	virtual string_smart GetName() const  PURE;
	//Возвращает расширение файла, отделенное от пути. Если путь не установлен, возвращает расширение по умолч
	virtual string_smart GetExt() const  PURE;
	//Закрывает документ
	virtual	bool Close() PURE;
	//Сохраняет документ
	virtual bool Save() PURE;
	//Сохраняет документ
	virtual bool SaveAs(cstr szNewFileName) PURE;
	//Возвращает true, если файл доступен только для чтения
	virtual bool IsReadOnly() const PURE;
	//COM Interface
	virtual ref_cnt_ptr<COM::IDocument> GetComInterface() PURE; 
};

//Интерфейс документа проекта
interface IProjectDocLink:public IDocumentLink
{	
public:
	PURE_INTERFACE(IProjectDocLink);
	enum {fileUnknown, fileSource, fileOther};
	enum string_data {strdataNotes};
	enum parse_report_type{prNone,prFull,prShort,prFullToFile,prShortToFile};
	
	class build_step
	{
	public:
		string_std m_strDescription;
		string_std m_strCommandLine;

		build_step(string_adapter strDescription, string_adapter strCommandLine):
			m_strDescription(strDescription.c_str()),m_strCommandLine(strCommandLine.c_str())
		{
		}
		bool operator == (const build_step & src) const{
			return (m_strDescription==src.m_strDescription) && (m_strCommandLine==src.m_strCommandLine);
		}
	};
	
	typedef vector<build_step> list_build_step;
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//Возвращает путь к exe(bin)-файлу, создаваемому при компиляции
	//Файл необязательно должен существовать
	virtual string_smart GetExePath() const  PURE;

	//Директория выходных файлов, может быть относительный путь
	virtual	string_smart GetOutputDirectory() const  PURE;
	virtual	void SetOutputDirectory(string_adapter szDirectory) PURE;
	//Возвращает директорию, всегда  c конечным \, где будут создаваться исполняемые файлы
	//Директория всегда возвращается абсолютным путем
	virtual	string_smart GetOutputDirectoryFull() const  PURE;

	//Директория промежуточных файлов, может быть относительный путь
	virtual	string_smart GetIntermediateDirectory() const  PURE;
	virtual	void SetIntermediateDirectory(string_adapter szDirectory) PURE;
	virtual	string_smart GetIntermediateDirectoryFull() const  PURE;

	//Возвращает первый файл в списке файлов проекта 
	virtual	cstr GetFileFirst() const  PURE;
	//Возвращает следующий файл по списку 
	virtual	cstr GetFileNext() const  PURE;
	//Возвращает список всех файлов, входящих в проект и удовлетворяющих
	//указанному типу
	virtual	void GetFiles(OUT string_list & lstFileList, int nFileType) const  PURE;
	//Возвращает итератор в списке файлов, входящих в проект, на найденный файл
	//virtual list_sources::iterator FindFile(const char* szFileName) PURE;
	//Проверяет, входит ди в состав проекта указанный файл
	virtual bool IsFilePresent(cstr szFileName) const  PURE;
	//Устанавливает флаг изменения документа
	virtual void SetModify(bool bModify=true) PURE;
	//Устанавливает флаг изменения документа
	virtual void SetActive(bool bActive=true) PURE;
	//Указывает, является ли данный проект активным
	virtual bool GetActive() const  PURE;
	//Сохраняет документ+все его исходники
	virtual void SaveAll() PURE;
	//Возвращает атрибут принадлежности файла к какой-либо группе файлов (fileUnknown, fileSource, fileOther...)
	virtual int GetFileType(cstr szSource) const  PURE;
	
	//Возвращает строку заметок 
	virtual string_smart GetNotes() const  PURE;
	//Модифицирует строку заметок 
	virtual void SetNotes(cstr szNotes) PURE;
	
	//Возвращает командную строку параметров
	virtual string_smart GetParams() const  PURE;
	//Модифицирует командную строку параметров
	virtual void SetParams(cstr szNotes) PURE;
	
	//Возвращает ключи компиляции
	virtual string_smart GetSwitches() const  PURE;
	//Модифицирует ключи компиляции
	virtual void SetSwitches(cstr szSwitches) PURE;

	//Получение списка дополнительных действий при компиляции
	virtual void GetBuildSteps(OUT list_build_step & PreBuildSteps, OUT list_build_step & PostBuildSteps) PURE;
	virtual void SetBuildSteps(const list_build_step & PreBuildSteps, const list_build_step & PostBuildSteps) PURE; 	
	//Возвращает имена транслируемого языка и выходного языка
	//нужно для базы знаний.
	virtual string_smart GetSourceLanguage() const  PURE;
	virtual string_smart GetTargetLanguage() const  PURE;
	virtual void SetSourceLanguage(cstr szName)  PURE;
	virtual void SetTargetLanguage(cstr szName)  PURE;
	
	//Направление трансляции, нужно для случая, когда в одном модуле 
	//поддерживается несколько входов и/или выходов
	virtual string_smart GetInputTranslationDir() const  PURE;
	virtual string_smart GetOutputTranslationDir() const  PURE;
	virtual void SetInputTranslationDir(string_adapter szName) PURE;
	virtual void SetOutputTranslationDir(string_adapter szName) PURE;

	//Возвращает список подключаемых библиотек
	virtual void GetLibs(OUT string_list & lst) const  PURE;
	
	//Возвращает список назначенных генераторов кода на ЯП для обработки 
	//процесса трансляции. Генераторы кода есть Plug-Ins, которые находятся
	//в папке Generators
	virtual void GetLangGens(OUT string_list & lst) const  PURE;
	//Устанавливает список назначенных генераторов кода 
	virtual void SetLangGens(const string_list & lst)  PURE;

	//Имя info-файла, генерируемого компилятором
	virtual string_smart GetCompilerInfoFilename() const  PURE;
	virtual void SetCompilerInfoFilename(string_adapter szFilename) PURE;

	//Возвращает режим трассировки грамматического разбора 
	virtual parse_report_type GetParseReportType() const  PURE;
	//Дает путь к файлу, который будет использоваться для сброса log-файла разбора
	virtual string_smart GetParseLogPath() const  PURE;
	//Возвращает true, если необходимо останавливаться на каждом сообщении от ядра
	virtual bool GetParseStopAtMessage() const  PURE;

	//Добавляет файл в проект. Если указать NULL, вызывает диалог выбора файла
	//Также может создавать новый файл, если szFileName не задан и пользователь
	//ввел новое имя
	virtual bool AddFile(cstr szFileName) PURE;
	//Создает новый файл в проект. Имя запрашивается у пользователя
	virtual IDocumentLink* AddNewFile() PURE;
	//Добавляет правило в проект постредством спец диалога
	virtual bool AddRule() PURE;
	//Удаляет файл из проекта
	virtual bool RemoveFile(cstr szSource) PURE;
	//Запускает процесс компиляции самого себя
	virtual void Build() PURE;
	//Удаление всех файлов компилятора
	virtual void Clean() PURE;
	//Запускает процесс выполнения самого себя
	virtual void Run() PURE;
	//Запускает процесс отладки
	virtual  void Debug() PURE; 
	
	//По указанному имени документа дает интерфейс управления точками, останова, отображения курсора и т.д.
	//может вернуть NULL
	virtual IDebugManageLink* GetDebugManageLink(cstr szSource) const  PURE;
	//Дает информацию о точке останова для указанного модуля в указанной строке
	virtual  bool GetBreakPoint(cstr szSource, long dwLine, BREAKPOINT & Breakpoint) PURE;	
	//Дает список всех точек останова, присутствующих в проекте
	//ПРинадлежность к тому или иному модулю иожно определить по BREAKPOINT::GetModuleName
	virtual  void GetBreakPoints(IDebugManageLink::vector_breakpoints & Breakpoints)  PURE;	
	//Установка точки останова для указанного модуля (значение имени модуля внутри структуры BREAKPOINT игнорируется)
	virtual  void SetBreakPoint(cstr szSource, BREAKPOINT & Breakpoint) PURE;
	//Снятие точки останова для указанного модуля в указанной строке
	virtual  void ResetBreakPoint(cstr szSource, long dwLine) PURE;	
	//Проверяет во внутренних структурах проекта, есть ли сведения
	//о указанной точке останова. Может вернуть true, даже 
	//если документ реально не открыт
	virtual bool IsBreakpointExists(cstr szSource, size_t dwLine, BREAKPOINT & bp) const  PURE;

	//COM Interface
	virtual ref_cnt_ptr<COM::IProjectDocument> GetComInterface2() PURE; 
};

//Интерфейс к представлению документа исходных текстов
//Эту ссылку можно получить только через ISourceDocLink::GetViewLink
interface ISourceDocLink;
interface ISourceViewLink:public ILink, public IDebugManageLink
{	PURE_INTERFACE(ISourceViewLink);

	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		virtual void evOnChangeCursorPos (ISourceViewLink * m_pSender, size_t m_nPos) PURE;
	};

	//Указатель на документ
	virtual	CView* GetObject() PURE;
	//дает интерфейс управления точками, останова, отображения курсора и т.д.
	virtual IDebugManageLink* GetDebugManageLink() PURE;
	//дает интерфейс на родительский документ
	virtual ISourceDocLink* GetDocumentLink() const PURE;

	virtual HWND GetWnd() PURE;
	//Размер текста
	virtual long GetTextLen() PURE;
	//Весь текст
	virtual void GetText(string_smart & strBuffer) PURE;
	//Установить текст
	virtual void SetText(cstr szBuffer) PURE;	
	//Только выделенный фрагмент
	virtual void GetSelText(string_smart &strBuffer) PURE;
	//Текст указанной строки
	virtual void GetLineText(string_smart & strBuffer,long dwLine) PURE;
	//Поблочное добавление текста, возвращает новую позицию курсора
	virtual long AddText(cstr szText, size_t nLen) PURE;
	//Вставка текста c одновременным перемещением курсора,возвращает новую позицию курсора
	virtual long InsertText(cstr szText,size_t nPos) PURE;
	//Кол-во строк
	virtual long GetLineCount() PURE;
	//Текущая поз курсора	
	virtual long GetCurPos () PURE;
	//Текущая строка	
	virtual long GetCurLine() PURE;
	//Первая видимая строка
	virtual long GetFirstVisibleLine() PURE;
	//Кол-во строк на экране
	virtual long GetLinesOnScreen() PURE;
	//Первая  позиция по указанной строке
	virtual long GetPosFromLine(long dwLine) PURE;
	//Последняя позиция по указанной строке
	virtual long GetEndPosOfLine(long dwPos) PURE;
	//Номер строки по указанной позиции
	virtual long GetLineFromPos(long dwPos) PURE;
	//Длина указанной строки
	virtual long GetLineLength(long dwLine) PURE;
	//Вычисляет позицию левее указанной, являющуюся началом строки
	virtual long OffsetToStartLinePos(long dwPos) PURE;
	//Вычисляет позицию правее указанной, являющуюся концом строки
	virtual long OffsetToEndLinePos(long dwPos) PURE;
	
	//Начало выделения
	virtual long GetSelStart() PURE;
	//Конец выделения
	virtual long GetSelEnd() PURE;
	//Выдедить указанный диапазон
	virtual void SelectRange(long dwStart,long dwEnd) PURE;
	//Заменить текущее выделение текстом
	virtual void ReplaceSelection(cstr szNewText) PURE;
	//Указывает, есть ли выделение текста
	virtual bool IsSelected() PURE;

	//Переход к указанной позиции
	virtual void GotoPos(long dwPos) PURE;
	//Переход к указанной позиции (строка:столбец)
	virtual void GotoPos(long dwLine, long dwCol) PURE;
	//Переход к указанной строке
	virtual void GotoLine(long dwLine) PURE;
	//Принудительный показ указанного региона
	virtual void EnsureRangeVisible(long nStartPos,long nEndPos) PURE;

	//Получение координат текущей страницы экрана: где позиция курсора, где начало страницы, какое смещенние по x
	virtual void GetViewPos(long & Pos, long & FirstLineVisible, long &FirstColVisible)  PURE;
	//Установка координат текущей страницы
	virtual void SetViewPos(long dwPos, long FirstLineVisible,long FirstColVisible)  PURE;

	virtual  void SetBookmark(long dwLine) PURE;
	virtual  void ResetBookmark(long dwLine) PURE;	
	virtual  bool GetBookmark(long dwLine) PURE;	
	virtual  void GetBookmarks(vector<long> & bookmarks) PURE;
};

//Интерфейс  к документу исходных текстов
//Получить этот интерфейс чере i_objects::GetInterface<> нельзя, так как онр никак 
//не именуется, но зато можно получить CDocument * от DocManager
//а затем провести dynamic_cast
interface ISourceDocLink:public IDocumentLink
{	PURE_INTERFACE(ISourceDocLink);

	//Указатель на представление. Можно вызывать даже при this=NULL, корректно обрабатывает
	virtual	ISourceViewLink* GetViewLink() PURE;
	//дает интерфейс управления точками, останова, отображения курсора и т.д.
	virtual IDebugManageLink* GetDebugManageLink() PURE;
	//Возвращает время внесения последних изменений в текст	
	virtual clock_t GetTimeOfLastChange() PURE;
	
	//COM Interface
	virtual ref_cnt_ptr<COM::ISourceDocument> GetComInterface2() PURE; 
};

//Интерфейс  к менеджеру документов
interface IDocMngrLink:public ILink
{	PURE_INTERFACE(IDocMngrLink);
	
	struct doc_iterator{
		void * pData1;
		void * pData2;
	};
	
	//Указатель на MFC - класс
	virtual	CDocManager* GetObject() PURE;

	virtual void SetActiveProject(IProjectDocLink* pProject) PURE;
	//Дает активный документ проекта
	virtual IProjectDocLink* GetActiveProject() const  PURE;
	//Дает активный документ исходника
	virtual ISourceDocLink* GetActiveSourceDoc() const  PURE;
	
	//поиск документа среди открытых по его пути. Возвращает интерфейс к найденному документу
	virtual IDocumentLink *	 FindDocument(cstr szDocPath) PURE;
	
	//Находит среди открытых проектов тот, у которого в составе файлов числится
	//указанный документ. Необходимо учитывать, что один и тот же документ может входить
	//в состав разлинчых проектов. Поэтому сначала проверяется активный проект, 
	//а затем - по порядку
	virtual IProjectDocLink* FindParentProject(cstr szDocPath)  PURE;

	//показывает на экране диалог выбора файла
	virtual bool SelectFile(
							string_list	 & out_lstFiles,
							cstr	in_szInitialDir=NULL,//на усмотрение менеджера
							cstr	in_szCaption=NULL,	//По умолчанию Open
							cstr	in_szFilter=NULL,	//по умолч подставляются зарегестрированные фильтры документов+ "*.*" или, если 
							cstr	in_szDefaultExt = NULL, //В списке поддерживаемых расширений подыскивается указанное расширение и по умолчанию именно оно будет выбрано в диалоге
							bool	in_bOpen=true,		
							bool	in_bUseRegisteredFilters=true,		//in_bUseRegisteredFilters=false, только "*.*" 
							bool	in_bAllowMultipleSelection=true,	//Можно выделять несколько файлов
							bool	in_bFileMustExist=true,				//должен ли существовать файл
							bool	in_bCreatePrompt=true) PURE;			//если файл не существует, спрашивать ли о его создании
	
	//позволяет получить путь для открытия собственного диалога с учетом всех настроек 
	//программы
	virtual string_smart GetInitialDir(string_adapter szDefInitialDir="") PURE;
	virtual void SetRecentlyDir(string_adapter strDir) PURE;

	//Отрытие документа документа на основе указанного шаблона. Можно указывать NULL, тогда 
	//вызовется диалоговое окно выбора файлов
	virtual bool OpenDocument(CDocTemplate * pDocTemplate) PURE;

	//Находит соответствующий шаблон по заданному расширению
	virtual CDocTemplate* FindDocTemplate(cstr szExt) PURE;

	//Открывает указанный документ и возвращает интерфейс(!!!) на него
	//если открыть не удалось, возвращает NULL. Параметр bSilentError указывает
	//скрывать ли стандартное сообщение типа "Документ не найден" от пользователя
	//Параметр szAllowedExts определяет, должен ли этот документ принадлежать какому-то 
	//определенному виду документов(по расширению). Если он не принадлежит этому типу, он и не открывается
	virtual IDocumentLink* OpenDocument(cstr szDocPath, cstr szAllowedExts = NULL, bool bSilentError=false) PURE;
	
	//Возвращает первый документ в списке открытых документов (объект, а не интерфейс!)
	virtual	CDocumentEx* GetDocumentFirst(doc_iterator & it) PURE;
	
	//Возвращает следующий документ по списку. Если возвращает NULL, значит, больше ничего нет
	virtual	CDocumentEx* GetDocumentNext(doc_iterator & it) PURE;
	
	//Пытается закрыть все документы
	virtual bool CloseAllDocuments() PURE;
};

//Интерфейс  к блоку копиляции
interface IBuilderLink : public ILink
{	PURE_INTERFACE(IBuilderLink);

	//Начало компиляции. Вторым параметром можно передавать функтор
	//обратной функции void pf(bool bResult), которая вызывается при окончании 
	//работы
	typedef functors::functor<void,ARGS_1(bool)> functor_build;
	virtual void Build( IProjectDocLink * pProjectLink,
						functor_build pfCallBk=functor_build())  PURE;	
	
	//Принудительная остановка текущего выполнения
	virtual void Stop()  PURE;		

	//Выполняется ли в данный момент компиляция
	virtual bool IsBusy() PURE;	

	//Если выполняется компиляция, то какого проекта
	virtual IProjectDocLink * GetBuildingProject() PURE;
};

//Интерфейс  к блоку запуска
interface IRunnerLink : public ILink
{	PURE_INTERFACE(IRunnerLink);

	enum EActivityType{atNone, atRun, atDebug};
	//Начало выполнения. Возвращает дескрипотр внешнего запушенного процесса
	virtual HANDLE Run(IProjectDocLink * pProjectLink)  PURE;
	//Начало отладки. bInitiallySuspended указывает приостановить ли выполнение кода после успешной инициализации
	virtual void Debug(IProjectDocLink * pProjectLink,bool bInitiallySuspended=true)  PURE;
	//Пауза текущего выполнения
	virtual void Pause() PURE;
	//Продолжение выполнения 
	virtual void Resume() PURE;
	//Продолжение выполнения на одну строчку
	virtual void ResumeStepOver() PURE;
	//Продолжение выполнения на одну строчку
	virtual void ResumeStepIn() PURE;	
	//Принудительная остановка текущего выполнения
	virtual void Stop() PURE;
	//Выполняется ли в данный момент выполнение или отладка
	virtual bool IsBusy() PURE;
	virtual bool IsBusy(EActivityType & ActivityType, IProjectDocLink *& pActiveProjectLink) PURE;
	virtual bool IsPaused() PURE;
	
	//Проверка на работоспособность точки останова
	virtual bool IsBreakpointUsed(cstr szModuleName,size_t nLine) PURE;
};

interface IClassTreeLink: public ILink
{PURE_INTERFACE(IClassTreeLink);

	//Выделяет (делает активным) элемент по позиции в указанном файле указанного проекта
	virtual bool SelectSyntaxItem(cstr szProjectName, cstr szFileName, long nPos)  PURE;
};

interface IParserLink: public ILink
{	PURE_INTERFACE(IParserLink);

	class syntax_item;
	typedef vector <syntax_item> vector_items;
	typedef vector_items::iterator  si_iterator;

	typedef vector <syntax_item *> vector_pitems;
	typedef vector_pitems::iterator  psi_iterator;
	enum ps_res {psrOk, psrFail, psrImpossible}; //Результат из трех состояний - да, нет, невозможно
	enum word_class {wcKeyWord, wcReservedVar, wcReservedFunc, wcUserIndentifier, wcUnknown};

	//Основная структура для Parser. Описывает любую синтаксичсекую единицу
	class syntax_item
	{
	protected:
		vector_items m_Units;	//Вложенные синтакcические единицы 
								//не дочерние, а вложенные в правило!
		size_t	m_nID;			//Для парсера
	public:
		enum item_class	{icUndefined,icRule,icVariant,icTerm,icFunc,icStruct};
		enum symbol_kind {tkUndefined,tkNormal, tkOptional, tkChildOptional};
		enum symbol_class {tcUndefined,tcTerminal,tcNonTerminal,tcStdTerminal};

		//Особенности реализации парсера таковы, что лучше всего хранить данные в string_std
		//одна копия данных все время висит в кэше парсера, поэтому ссылочная схема
		//позволяет избежать постоянных "движений" памяти
		string_std strName;		//Наименование
		string_std strFilename;	//В каком файле данный объект находится. Заполняется не всеми функциями
		string_std strData;		//Кусок текста
		long		 nPosStart;		//Начало в тексте
		long		 nPosEnd;		//Конец в тексте
		item_class	 nClass;		//Классификация элемента (функция, структура и проч)
		symbol_kind	 nSymbolKind;	//Вид терма: обычный, опциональный, опционально-зависимый
		symbol_class nSymbolClass;	//Класс терма: терминал/нетерминал и проч
		void*		 pData;			//Пользовательскте данные
		long		 nData;			//Пользовательскте данные
		
		//string_smart strActionsPre;	//Можно еще вставлять для символов, вариантов и правил
		string_smart strActions;	//действия внутри тела и до (пока не поддерживается)

		vector_pitems pParents;		//указатели на родительские и дочерние объекты
		vector_pitems pChildren;		//используются отдельно для построения иерархии
										//например, чтобы определить зависимости между правилами
		
		syntax_item(){
			Clear();
		}

		//Итератор на первый дочерний объект. Данная функция вызывается перед началом 
		//цикла
		si_iterator GetFirstUnitPos(){
			return m_Units.begin();
		}
		//Дает текущий дочернийобъект и переводит указатель на следующий объект
		syntax_item & GetNextUnit(si_iterator & it){
			return *it++;
		}
		//Конец перебора
		bool IsEndUnitPos(si_iterator it){
			return it==m_Units.end();
		}
		//Можно получать дочерний объект по индексации, но это медленнее
		syntax_item & GetUnitAt(int index){
			return m_Units[index];
		}
		//Добавляет дочерний элемент в список
		syntax_item & AddUnit(){
			m_Units.push_back(syntax_item());
			return *m_Units.rbegin();
		}
		//Кол-во детей
		size_t GetUnitsCount(){
			return m_Units.size();
		}
		//Очистка
		void Clear(){
			m_Units.clear();
			nPosStart=nPosEnd=-1;nClass=icUndefined;nSymbolClass=tcUndefined; nSymbolKind=tkUndefined; pData=NULL;
		}
	};
	
	
	//Вершина иерархии вложенности синтаксических единиц - корень 
	//хотя он и наследуется от syntax_item, он не имеет атрибутов nPosStart, nPosEnd и т.д.
	class syntax_item_root: public syntax_item
	{
	protected:
		syntax_item::strName;			//Скрываем поля базового класса
		syntax_item::nPosStart;		
		syntax_item::nPosEnd;		
		syntax_item::nClass;	
	};
	
	//Класс-агрегат различных списков найденных синтаксических единиц
	typedef long  sir_iterator;
	struct syntax_info{
		syntax_item_root		m_Rules;
		syntax_item_root		m_Functions;
		syntax_item_root		m_Structs;
		void Clear(){
			m_Rules.Clear();m_Functions.Clear();m_Structs.Clear();
		}

		syntax_item_root & GetRoot(const sir_iterator & it){
			switch (it){
				case 0: return m_Rules;
				case 1: return m_Functions;
				case 2: return m_Structs;
				default:
					throw exception("Index out of range");
			}
		}
		//Выдает итератор, указавыающий на первый элемент syntax_info
		sir_iterator GetBeginPos(){
			return 0;
		}
		
		//Выдает итератор, указавыающий за последний элемент syntax_info
		sir_iterator GetEndPos(){
			return 3;
		}
	};

	//Разбирает входной текст и выдает на выходе структуру syntax_info 
	//пераметр szFileName в данном случае играет роль ключа для поиска данных во внутренних
	//таблицах. Рекомендуется вписывать сюда реальные названия, что позволит использовать кэш
	virtual void ParseSourceText	(const string_smart & strText, cstr szFileName, syntax_info & Result, bool bAllowUseCache=true) PURE;
	//Разбирает текст из указанного файла и выдает на выходе структуру syntax_info 
	virtual void ParseSourceFile	(cstr szFileName, syntax_info & Result, bool bAllowUseCache=true) PURE;
	//Разбирает текст из всех файлов указанного проекта  
	virtual void ParseWholeProject	(IProjectDocLink & IProject, syntax_info & Result, bool bAllowUseCache=true) PURE;
	//Ищет в указанном тексте правило с указанным именем. Если правило найдено,
	//Возвращает позицию начала и конца блока этого правила от слова rule до скобки }
	//включительно
	virtual bool FindRule			(const string_smart & strText, cstr szRuleName, size_t & nStart, size_t & nEnd) PURE;
	
	//Ищет определение указанного термина во всех файлах проекта 
	//В aMetAt вызывающим указывается место (файл и позиция), где он это нашел, это помогает парсеру разбирать текст
	//Найденная позиция возвращается в aFoundAt
	//Может двигаться по всем файлам проекта в поисках объявления
	virtual ps_res FindDeclaration(cstr szItemName, ITEMLOCATION aMetAt, ITEMLOCATION & aFoundAt) PURE;

	//Ищет определение синтаксической конструкции в позиции nPos файла szFileName
	virtual ps_res DetectSyntaxItem(cstr szFileName, size_t nPos, OUT syntax_item & aFoundItem) PURE;

	//Возвращает набор вариантов, как может заканчиваться данное выражение
	//возвращает true, если найдено хоть что-то похожее. В списке lstMostUsed можно передавать
	//слова, которые можно считать наиболее употребительными
	virtual bool SuggestAutoComlete(cstr szItemName, const string_list & lstMostUsed, ITEMLOCATION aMetAt, string_list & aSuggestions) PURE;
	
	//Возвращает классификацию переданного слова
	virtual word_class GetWordClass(cstr szItemName) PURE;

	//По переданной структуре syntax_info генерирует текст. Строка strOutText изначально не обнуляется!
	virtual void AssembleToText		(syntax_info & SyntaxData, string_smart & strOutText) PURE;

	//По переданной структуре syntax_info генерирует текст в формате РБНФ. Строка strOutText изначально не обнуляется!
	virtual void AssembleToXBNF		(syntax_info & SyntaxData, string_smart & strOutText) PURE;
	
	//по переданным правилам находит все связи (родитель-потомок между ними) между ними 
	virtual void BuildRuleDependence(syntax_item_root & m_Rules) PURE;

	//Выдает текст главной функции main, 
	virtual string_smart ComposeMainFunction(cstr szRootRule, 
									bool bIgnoreCase,
									list<string_pair> lstIgnore,
									list<string_pair> lstReplace) PURE;

	//Выдает список всех входных и выходных направлений трансляции (#IL, #OL)
	virtual void FindTranslationDirections(IN IProjectDocLink & IProject, OUT string_list & InputDirs,  OUT string_list & OutputDirs) PURE;

	//COM Interface
	virtual ref_cnt_ptr<COM::IParser> GetComInterface() PURE; 
};

//Интерфейс к диалогу About
interface IAboutLink : public ILink
{
	virtual void Run() PURE;
};

//Интерфейс к диалогу TipOfTheDay
interface ITipOfTheDayLink : public ILink
{
	virtual void Run() PURE;
};

//Интерфейс к диалогу AddRule
interface IAddRuleLink : public ILink
{
	virtual bool Run() PURE;
};

//Интерфейс  к объекту управления базой данных знаний
interface IKnowledgeBaseLink: public ILink
{	PURE_INTERFACE(IKnowledgeBaseLink);
	
	struct translate_pair{
		string_std m_strSourceLanguage;
		string_std m_strTargetLanguage;
		string_std m_strModulePath;
	};
	typedef list<translate_pair> tt_pair_list;

	//Операция добавления проекта в базу знаний
	virtual bool Add(IProjectDocLink * pProjectLink)  PURE;
	//Показывает всю базу с внесенными языками
	virtual bool ShowRegistry() PURE;
};

//Интерфейс  к реестру генераторов кода для грамматического модуля на любые языки  (Plug-Ins)
interface ILangGenRegistryLink: public ILink
{	PURE_INTERFACE(ILangGenRegistryLink);
	
	class lang_gen_item {
	protected:
		string_std m_strPath;
		string_std m_strName;
		string_std m_strDescription;
	public:
		const string_std &	GetName() 			const {return m_strName;}
		const string_std &	GetPath() 			const {return m_strPath;}
		const string_std &	GetDescription() 	const {return m_strDescription;}
	};
	typedef vector<lang_gen_item> lst_lang_gen;

	//Возвращает список всех найденных объектов
	virtual void GetList(lst_lang_gen & lst) const  PURE;
	
	//По имени генератора возсврашает полный путь к DLL
	virtual bool FindByName(const string_adapter strName, OUT lang_gen_item & Item) const  PURE;
};

//Интерфейс  к объекту генерации грамматик по исходному тексту
interface IGrammaticalInferenceLink: public ILink
{	PURE_INTERFACE(IGrammaticalInferenceLink);
	virtual bool Run() PURE;
};

//Интерфейс  к DockedBar Control. Multiple Instance!!!
interface IBarLink : public IMultiInstanceILink
{	PURE_INTERFACE(IBarLink);

	enum ID
	{
		idBarWorkspace,
		idBarInfo,
		idBarFullScreen,
		idBarOutput,
		idBarWatch,
		idBarScreen,
		idBarBreakpoints
	};

	//Создание окна DockBar на стороне IBarLink
	virtual void CreateWnd(CFrameWnd* pParentWnd) PURE;
	//Определение Dock-сторон
	virtual void InitDock() PURE;
	//Вызывается главной формой, когда она решается показаться
	virtual void OnShowMainFrame(bool bVisible) PURE;
	//Указатель на базовый DockBar
	virtual CControlBar * GetObject() PURE;
	//Указатель на TabWnd, если таковой имеется
	virtual CBCGTabWnd* GetTabWnd(){return NULL;}
};

//Интерфейс  к MainFrame
interface IMainFrameLink: public ILink
{	PURE_INTERFACE(IMainFrameLink);

	//обработка событий
	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);

		//Структура, указывающая, на закрытие главного окна программы
		class evdCloseQueryMainWnd
		{
		private:
			bool m_bCanClose;
		public:
			evdCloseQueryMainWnd():m_bCanClose(true){}
			void SupressClose()	{m_bCanClose=false;}
			bool IsCanClose()	{return m_bCanClose;}
		};

		virtual void evOnCloseQueryAppMainWnd(evdCloseQueryMainWnd & Info) PURE;
		virtual void evOnCloseAppMainWnd() PURE;
	};

	//Включение указанного окна в обработку сообщений OnCmdMsg
	virtual bool IncludeToCmdMsgMap(CCmdTarget* pHandler)  PURE;
	//Включение указанного окна в обработку сообщений OnCmdMsg
	virtual bool ExcludeFromCmdMsgMap(CCmdTarget* pHandler)  PURE;

	//Запуск указанной функции с синхронизацией по очереди сообщений
	//это полезно для потоков, когда им нужно выполнить определенную 
	//функцию в контексте главного потока. Безопасна, не требует 
	//наличие объекта IMainFrameLink (это важно для многопоточности)
	virtual void SyncInvoke(functors::IFunctorRemote & func)  PURE;

	//Позволяет показывать строку подсказки в позиции StatusBar
	virtual void ShowTip(cstr szTipText)  PURE;

	//Дать текущее активное окно CView документа
	virtual CView * GetActiveView() const  PURE;

	//Установить набор индикаторов в StatusBar
	virtual void SetStatusBarIndicators(const UINT* lpIDArray, int nIDCount, bool bInitiallyDisabled) PURE;

	//Установить в статусной строке текст, типа "Ready" и проч
	virtual void SetStatusMessage(string_adapter strText) PURE;
	virtual string_smart GetStatusMessage() PURE;
};

//Интерфейс к WatchList
//Multiple Instance!!!
interface IWatchListLink: public IMultiInstanceILink
{	PURE_INTERFACE(IWatchListLink);
	enum ID
	{
		idWatchListAuto,
		idWatchListLocal,
		idWatchListN,
		idWatchCallStack,
		idWatchParseStack,
	};

	//Открывает сессию вывода. Чаще всег оиспользуется для подготовки к выводу
	//Может игнорироваться в некоторых реализациях
	virtual void StartSession(bool bClear=true) PURE;
	//Закрывает сессию вывода. Может игнорироваться в некоторых реализациях
	virtual void StopSession(bool bClear=true) PURE;
	//Очистка всего списка
	virtual  void Clear() PURE;
	//Возвращает кол-во элементов в списке
	virtual size_t GetItemCount() PURE;	
	//В указанную позицию вставляет новое содержимое WATCHITEM
	virtual WATCHITEM GetItem(size_t nIndex) PURE;
	//В указанную позицию вставляет новое содержимое WATCHITEM
	virtual void SetItem(size_t nIndex, const WATCHITEM & WatchItem) PURE;
	//Очищает предыдущее содержимое и устанавливает новый список
	//Позволяет WatchList сравнивать предыдущий список с заданным
	//и подкрашивать измененные величины
	virtual  void SetItems(WATCHITEM::vector_items & WatchItem) PURE;
};

//Интерфейс к IWatchParsePanel
interface IWatchParsePanel: public ILink
{	PURE_INTERFACE(IWatchParsePanel);

	virtual void SetTextBehind(cstr szText) PURE;
	virtual void SetTextAhead(cstr szText) PURE;
};

//Интерфейс для вывода сообщений. Используется для вывода сообщений клмпилятора 
//и непосредственно программы
interface IOutputLink: public IMultiInstanceILink
{	PURE_INTERFACE(IOutputLink);

	enum ID
	{
		idOutputScreen,
		idOutputBuild,
		idOutputParse,
		idOutputFindInFiles
	};

	//Открывает сессию вывода. Чаще всег оиспользуется для подготовки к выводу
	//Может игнорироваться в некоторых реализациях
	virtual void StartSession(bool bClear=true) PURE;
	//Закрывает сессию вывода. Может игнорироваться в некоторых реализациях
	virtual void StopSession(bool bClear=false) PURE;
	//Вывод очередного фрагмента текста
	virtual void Print(cstr szText) PURE;
	//Очистка окна (или буфера) от всех предыдущих сообщений
	virtual void Clear() PURE;
	//Получение всего выведенного текста
	virtual string_smart GetText() PURE;
};

//Интерфейс для диалога поиска текста в файлах (FindInFiles)
interface IGotoLineLink: public ILink
{
	virtual bool Run(long dwCurLine, long dwLastLine, OUT long & out_dwNewLine) PURE;
};

//Интерфейс для диалога поиска текста в файлах (FindInFiles)
interface IFindTextLink: public ILink
{	PURE_INTERFACE(IFindTextLink);

	//Структура для FindDialog. Через нее производится вызов объекта Find
	//и возвращение результатов
	struct vbdFind
	{
		bool in_bInSelection;
		string_smart in_strText;
		long in_dwInitialSelStart;	//Непосредтвенно FindDialog не используются
		long in_dwInitialSelEnd;	//

		bool out_bWholeWord;
		bool out_bMatchCase;
		bool out_bAsRegexp;
		bool out_bSearchDown;	// Down=true; Up=false
		bool out_bInSelection;	// InSelection=true; Whole file=false
		bool out_bWrapAround;	


		string_smart out_strText;

		vbdFind():
		in_bInSelection(false),in_strText(),
			in_dwInitialSelStart(0),in_dwInitialSelEnd(0),
			out_bWholeWord(false),out_bMatchCase(false),out_bAsRegexp(false),
			out_bSearchDown(true),out_bInSelection(false),out_bWrapAround(false){
		}
	};

	//Запуск формы 
	virtual bool Run(vbdFind & Data) PURE;
};

//Интерфейс для диалога поиска текста в файлах (FindInFiles)
interface IReplaceTextLink: public ILink
{	PURE_INTERFACE(IReplaceTextLink);

	struct vbdReplace: public IFindTextLink::vbdFind
	{
		enum operations{opFindNext, opReplace, opReplaceAll};
		// Функтор bool (operations)
		typedef functors::functor<bool,ARGS_2(operations, HWND)> functor_find;

		functor_find in_pfCallback;
		string_smart out_strReplaceText;

		vbdReplace():vbdFind(){
		}
	};

	//Запуск формы 
	virtual bool Run(vbdReplace & Data) PURE;
};

//Интерфейс для диалога поиска текста в файлах (FindInFiles)
interface IFindInFilesLink: public ILink
{	PURE_INTERFACE(IFindInFilesLink);
	//Запуск формы 
	//szFindWhat - текст по умолчанию, который ищется
	//pOutput - интерфейс объекта, в который будут выводтся сообщения
	virtual bool Run(cstr szFindWhat, IOutputLink * pOutput) PURE;
};

//Интерфейс для диалога задания параметров к запускаемому модулю
interface ISetParamsLink: public ILink
{	PURE_INTERFACE(ISetParamsLink);

	//Запуск формы изменения параметров
	//strParams - задается строка параметров и сюда же возвращается измененная строка
	//Если пользователь не изменил  параметр или нажал "отмена", возвращается false.
	virtual bool Run(string_smart & strParams, string_smart & strBrowseInitialDir, bool & bShowInFuture) PURE;
};

//Интерфейс для диалога импорта грамматик
interface IImportGrammarLink: public ILink
{	PURE_INTERFACE(IImportGrammarLink);

	virtual bool Run() PURE;
};

//Интерфейс для диалога свойств
interface IProjPropsLink: public ILink
{	PURE_INTERFACE(IProjPropsLink);

	//Свойства проекта
	virtual bool Run(IProjectDocLink * pProjectLink) PURE;
};

//Интерфейс для диалога свойств
interface ISourcePropsLink: public ILink
{	PURE_INTERFACE(ISourcePropsLink);

	//Свойства 
	virtual bool Run(cstr szFileName) PURE;
};

//Интерфейс для диалога стиатистики и оптимизации правил в грамматиках
interface IStatisticLink: public ILink
{	PURE_INTERFACE(IStatisticLink);
	
	//Запуск формы статистики и оптимизации
	virtual bool Run(IProjectDocLink * pProjectLink) PURE;
};
//Интерфейс для диалога поиска текста в файлах (FindInFiles)
interface ISyntaxDiagramsLink: public ILink
{	PURE_INTERFACE(ISyntaxDiagramsLink);
	enum show_type {stTree,stDiagram};

	//Запуск формы 
	virtual bool Run(IProjectDocLink * pProjectLink, show_type nType) PURE;
};

//Интерфейс для диалога просмотра содержимого библиотек
interface IViewLibLink: public ILink
{	PURE_INTERFACE(IViewLibLink);

	//Запуск формы
	virtual bool Run() PURE;
};


//Интерфейс для диалога добавдения нового элемента в проект
interface INewDocumentWizardLink: public ILink
{	PURE_INTERFACE(INewDocumentWizardLink);

	//Форма запускается, пользователем задается имя документа,
	//файл создается и добавляется в указанный проект. Необходимо 
	virtual IDocumentLink * Run(IProjectDocLink * pProjectLink) PURE;
};

interface INewProjectWizardLink: public ILink
{
	PURE_INTERFACE(INewProjectWizardLink);

	//Форма запускается, пользователем задается имя документа,
	//файл создается и добавляется в указанный проект. Необходимо 
	virtual bool Run(IProjectDocLink * pProjectLink) PURE;
};

//Интерфейс для диалога, выбирающего вид создаваемого проекта
//фактически показывает набор доступных мастеров, 
struct INewProjectLink: public ILink
{
	PURE_INTERFACE(INewProjectLink);


	virtual bool Run(	string_adapter szProjectNameDef, //Имя проекта по умолчанию
						string_adapter szProjectDirDef,	//Родительская директория по умолчанию
						OUT string_smart & out_strProjectName,	//Имя проекта
						OUT string_smart & out_strProjectDir,	//Родительская директория, где будет размещен проект
						INewProjectWizardLink* & pWizardLink 
					)  PURE;
};

//Интерфейс для диалога настроек программы
interface IPreferencesLink: public ILink
{	PURE_INTERFACE(IPreferencesLink);
	
	//Запуск формы 
	virtual bool Run() PURE;
};

//--=========================== EVENTS  ================================--

//Структура, указывающая, что документ XXX изменил состяние
struct evdChangeDocumentInfo
{
	enum {changeModify,changeSave,changeOpen,changeClose};
	//changeOpen и changeClose используются в наследниках
	enum {idDocument=0};
	int				in_nChangeType;
	int				in_nDocID;	
	string_smart	in_strDocumentName;
	string_smart	in_strDocumentPath;
	IDocumentLink&	in_DocInterface;

	//-----------
	evdChangeDocumentInfo(cstr szDocumentName, cstr szDocumentPath,IDocumentLink & DocIface):
		in_strDocumentName(szDocumentName),in_strDocumentPath(szDocumentPath),in_DocInterface(DocIface)
	{
		in_nDocID=idDocument;
	}
	virtual ~evdChangeDocumentInfo(){
	}
};
//Структура, указывающая, что документ XXX открывается
struct evdOpenDocumentInfo: virtual public evdChangeDocumentInfo
{
	//-----------
	evdOpenDocumentInfo(cstr szDocumentName, cstr szDocumentPath,IDocumentLink & DocIface):
			evdChangeDocumentInfo(szDocumentName,szDocumentPath,DocIface)
	{
		in_nChangeType = changeOpen;
	}
};

//Структура, указывающая, что документ XXX закрывается
struct evdCloseDocumentInfo: virtual public evdChangeDocumentInfo
{
	evdCloseDocumentInfo(cstr szDocumentName, cstr szDocumentPath,IDocumentLink & DocIface):
		evdChangeDocumentInfo(szDocumentName,szDocumentPath,DocIface)
	{
		in_nChangeType = changeClose;
		
	}
};
//----------------------- Project Data Info ------------------------------
//Структура, указывающая, что документ проекта изменил состяние
struct evdChangeProjectInfo: virtual public evdChangeDocumentInfo
{
	enum {idProject=idDocument+1};
	enum {changeAddFile=0xFF,changeRemoveFile,changeActivate,changeDeactivate};
	string_smart in_strIncludedFile; //Испольуется только в changeAddFile;changeRemoveFile

	evdChangeProjectInfo(cstr szDocumentName, cstr szDocumentPath,IProjectDocLink & DocIFace):
		evdChangeDocumentInfo(szDocumentName,szDocumentPath,DocIFace)
	{
		in_nDocID=idProject;
		
	}
};
//Структура, указывающая, что документ проекта открывается
//Чтобы соблюсти приниципы наследования от специфичной по виду операции структуры
//доп наследуем от evdOpenDocumentInfo
struct evdOpenProjectInfo: public evdChangeProjectInfo, public evdOpenDocumentInfo
{
	//-----------
	evdOpenProjectInfo(cstr szDocName, cstr szDocPath, IProjectDocLink & DocIface):
		evdChangeDocumentInfo	(szDocName,szDocPath,DocIface),
		evdChangeProjectInfo	(szDocName,szDocPath,DocIface),
		evdOpenDocumentInfo		(szDocName,szDocPath,DocIface)

	{
		in_nChangeType = changeOpen;
		
	}
};
//Структура, указывающая, что документ проекта закрывается
//Чтобы соблюсти приниципы наследования от специфичной по виду операции структуры
//доп наследуем от evdCloseDocumentInfo
struct evdCloseProjectInfo: public evdChangeProjectInfo, public evdCloseDocumentInfo
{
	evdCloseProjectInfo(cstr szDocName, cstr szDocPath, IProjectDocLink & DocIface):
		evdChangeDocumentInfo(szDocName,szDocPath,DocIface),
		evdChangeProjectInfo(szDocName,szDocPath,DocIface),
		evdCloseDocumentInfo(szDocName,szDocPath,DocIface)
	{
		in_nChangeType = changeClose;
		
	}
};

//----------------------- Source  Data Info ------------------------------
//Структура, указывающая, что документ проекта изменил состяние
struct evdChangeSourceDocInfo: virtual public evdChangeDocumentInfo
{
	enum {idSourceDoc=idDocument+10};
	enum {changeBreakpoints=0xFFF, changeBookmarks};
	evdChangeSourceDocInfo(cstr szDocumentName, cstr szDocumentPath,ISourceDocLink & DocIFace):
		evdChangeDocumentInfo(szDocumentName,szDocumentPath,DocIFace)
	{
		in_nDocID=idSourceDoc;
		
	}
};
//Структура для WorkSpacBar. Через нее осуществляется добавление описания файлов в дерево 
struct evdOpenSourceDocInfo: public evdChangeSourceDocInfo, public evdOpenDocumentInfo
{
	evdOpenSourceDocInfo(cstr szDocName, cstr szDocPath, ISourceDocLink & DocIface):
		evdChangeDocumentInfo	(szDocName,szDocPath,DocIface),
		evdChangeSourceDocInfo	(szDocName,szDocPath,DocIface),
		evdOpenDocumentInfo		(szDocName,szDocPath,DocIface)
	{
		in_nChangeType = changeOpen;
		
	}
};
//Структура, указывающая, что документ проекта закрывается
struct evdCloseSourceDocInfo: public evdChangeSourceDocInfo, public evdCloseDocumentInfo
{
	evdCloseSourceDocInfo(cstr szDocName, cstr szDocPath, ISourceDocLink & DocIface):
		evdChangeDocumentInfo	(szDocName,szDocPath,DocIface),
		evdChangeSourceDocInfo	(szDocName,szDocPath,DocIface),
		evdCloseDocumentInfo	(szDocName,szDocPath,DocIface)
	{
		in_nChangeType = changeClose;
		
	}
};

//---------------------- Source Doc Styler -------------------------------
interface ISourceDocStylerLink: public ILink, public mtdev_lib::IPreferencesDocumentTypeProvider
{
	PURE_INTERFACE(ISourceDocStylerLink);
	//События
	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		//Изменение параметров. В качестве параметра ParameterChanged
		//приходит идентификатор изменившегося раздела
		virtual void evOnChangeData() PURE;			
	};

	//------------------- style_and_words  -------------
	class style_and_words
	{
		size_t m_nStyleNumber;
		string_smart m_strWords;
	public:
		size_t	GetStyleNumber() const	{return m_nStyleNumber;}	
		bool	IsEmpty()		 const	{return m_strWords.length() == 0; }
		bool	IsSingleChar()	 const	{return m_strWords.length() == 1; }
		char_t	GetSingleChar()  const	{return m_strWords.c_str()[0];}

		bool IsIncludes(cstr szValue){
			if (m_strWords.isempty() || (!szValue))	
				return false;
			// Set of symbols separated by spaces					
			if (isalpha(m_strWords[0]))  
				return m_strWords.pos(szValue," ")!=-1;
			// Set of individual characters. Only one character allowed for now
			return strchr(szValue, m_strWords[0]) != NULL;
		}

		void SetData(size_t nStyleID, string_adapter strWords)
		{
			m_strWords=strWords;
			m_nStyleNumber =nStyleID;
		}
	};

	//Класс для работы со стилем
	class style_definition 
	{
	public:
		//italics -		m_bItalics,		true 
		//bold -		m_bBold,		true 
		//eolfilled -	m_bEolFilled,	
		//underlined -	m_bUnderlined,	true 

		long			m_nStyleID;
		string_smart	m_strStyleName;
		string_smart	m_strFont;
		long			m_iSize;
		bool			m_bBold;
		bool			m_bItalics;
		bool			m_bUnderlined;

		long			m_clFore;	//Может быть <0
		long			m_clBack;	//Может быть <0

		bool			m_bVisible; //Доступен для изменения пользователем
		//??
		bool			m_bEolFilled;	// true by default
		int				m_iCaseForce;	//case - SC_CASE_UPPER, SC_CASE_LOWER, SC_CASE_MIXED
		int				m_nCharset;
	};		

	//COM-интерфейс делаем базовым, но не видимым для клиентов
	interface IDocumentStyleDescriptor: public mtdev_lib::IDocumentStyleDescriptor
	{
		virtual void GetStatementIndent(OUT style_and_words & result) PURE;
		virtual void GetStatementEnd(OUT style_and_words & result)PURE;
		virtual void GetBlockEnd(OUT style_and_words & result) PURE;
		virtual void GetBlockStart(OUT style_and_words & result) PURE;
		virtual bool GetIndentOpening() PURE;
		virtual bool GetIndentClosing() PURE;

		virtual void GetStyleDescByID(int aStyle, OUT ISourceDocStylerLink::style_definition & ) PURE;
		virtual void GetStyleDescByIndex(int aStyleIndex, OUT ISourceDocStylerLink::style_definition & ) PURE;
	};

	//По расширению находит соответсвующий интерфейс, описывающий свойства документа
	virtual IDocumentStyleDescriptor * FindStyleDescriptor(string_adapter strFileExt) PURE;

	//Что это, пока не понятно
	//virtual size_t GetCharset() PURE;

	//Полуение списка поддерживаемых расширений файлов
	typedef list<DOCUMENT_INFO> lst_doc_types;
	virtual const lst_doc_types & GetDocTypes() PURE;
};

//-------------------------- Storage -------------------------------------
interface IStorageLink: public ILink, public mtdev_lib::IPreferencesDataProvider
{
	enum EParam{
	  pmGeneral_MDIStyle,
	  pmGeneral_StartupAction,
	  pmGeneral_MRUSize,
	  pmGeneral_DetectChangesOutside,
	  pmGeneral_AllowEditReadOnlyFiles,
	  pmGeneral_OpenFileUsingActiveDocDir,
	  pmGeneral_AutosaveFiles,
	  pmGeneral_AutosaveEachNMinutes,
	  pmGeneral_SaveAllFilesBeforeRun,
	  pmGeneral_TearOffMenus,
	  pmFind_ShowTipNotMsgBox,
	  pmFind_PutTheText,
	  pmEditor_UseDragDrop,
	  pmEditor_KeepUndoAfterSave,
	  pmEditor_CreateBackupFiles,
	  pmEditor_ShowSelectionMargin,
	  pmEditor_ShowFoldMargin,
	  pmEditor_ShowNumberMargin,
	  pmEditor_ShowVertSB,
	  pmEditor_ShowHorzSB,
	  pmEditor_ShowRightMargin,
	  pmEditor_ShowRightMarginAt,
	  pmEditor_ShowWhiteSpaces,
	  pmEditor_ShowIndentGuides,
	  pmUpdate_UseAutoUpdate,
	  pmUpdate_CheckMode,
	  pmUpdate_CheckEveryNDays ,
	  pmBugReport_UseBugReport,
	  pmBugReport_Address,
	  pmBugReport_IncludeOSInfo,
	  pmLastOpenedProject
	};

	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		//Изменение параметров. В качестве параметра ParameterChanged
		//приходит идентификатор изменившегося раздела
		virtual void evOnChangeData(EParam ParameterChanged) PURE;			
	};

	//Последний открытый проект
	virtual string_smart GetLastOpenedProject() PURE;
	virtual void SetLastOpenedProject(string_adapter value) PURE;
};

namespace singleton_cache
{
	IStorageLink *	GetStorageLink();
	IApplicationLink * GetApplicationLink();
	IMainFrameLink * GetMainFrameLink();
	IDocMngrLink *	GetDocMngrLink();
	IBuilderLink *	GetBuilderLink();
	IRunnerLink *	GetRunnerLink();
	IParserLink *	GetParserLink();
	ISourceDocStylerLink * GetSourceDocStylerLink();
};

#endif;
