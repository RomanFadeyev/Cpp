/********************************************************************
	created:	2002/02/03
	created:	3:2:2002   23:02
	filename: 	$(Projects)\Sources\MTDEv\BaseStructs.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	BaseStructs
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	�������� ������� ��������� ��� �������� � ��������, �������� 
				� �.�.
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

//�������������� ��������� 
enum EWindowMessages {CM_BASE=0xB000, 
					CM_SHOW,	//��� ����������
					CM_HIDE,	//��� ����������
					CM_ACTIVATE,//��� ����������
					CM_REALIGN,	//��� ����������
					CM_POST		//����� ������������ �� ������������ ����������
					};
//--=================== Structures =====================================--

struct IDebugManageLink;

//���� ������ ������������ ��� ����, ����� ��������� �������� ������� 
//���������� ������ � ����� ��� �������� = 
//!!! ��������� ����� ������� ������� ��� ��� public
#define PURE_INTERFACE(class_x) \
protected: ~class_x(){} \
class_x & operator = (const class_x &){}\
public:


//--------- �������� COM --------------------------------------------------
#define DEFINE_ADD_REF_STUB ULONG __stdcall AddRef( void)  {return 0;}
#define DEFINE_RELEASE_STUB ULONG __stdcall Release( void) {return 0;}
#define BEGIN_QUERY_INTERFACE virtual HRESULT __stdcall QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {
#define COM_INTERFACE_SUPPORT(x) if (riid==__uuidof(x)) {*ppvObject=static_cast<x*>(this); return S_OK;}
#define END_QUERY_INTERFACE return E_NOINTERFACE; }
//--------- �������� COM --------------------------------------------------

/*
class e_abort_operation: public std::exception
{
public:
e_abort_operation(string_adapter strCause): std::exception(strCause){}
};
*/

//�������� ����� ��������
struct BREAKPOINT{
//protected:
	IDebugManageLink * m_pOwner;
	string_smart m_strModuleName;	//�������� ������, ��������� ������ ������. ����������� �� ������
	long dwHandle;	//��������� ����� ��������
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
	
	//������ � ������
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
//�������� ������ � ������� ��� ������� �������� � WatchList
struct WATCHITEM{
	typedef vector<WATCHITEM> vector_items;
	enum kind{wikSimple,wikStruct,wikArray,wikFunction};
	string_smart strName;	//��� ������� = ��� ��� �������
	string_smart strType;
	string_smart strContext;//��� ������� - ��� �������� ������
	size_t		 nLine;
	size_t		 nCol;
	kind		 nKind;
	variant		 value;			//��� wikStruct ��� �������� �� ����������
	vector_items ChildItems;	//�������� ��������� �������� 
};

//���������� ��������� ����-�� ����� �������� �������� ����� ����� � ������� � ���
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

//���������� �������� ��������� � �������������� �� ������
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

//���� ��������� ������������ ������ ������ �������� ��� 
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

//���������� ������� ���������
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

//������� ����� ��� ���� ����� ����������. �������� �������� �� ���������� 
//������������. ����� ������� �� ������������ ������� ���������� ����������� 
//CDocumentEx * � IDocumentLink (�� ����). ��������! ��� ������ ����������
//������ ������������� �� ������� ������, � �� �� CDocument. 
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
	override BOOL OnOpenDocument(LPCTSTR lpszPathName);	//������ public � CDocument?
	override BOOL OnSaveDocument(LPCTSTR lpszPathName);
	override void SetTitle(LPCTSTR lpszTitle);
	override BOOL DoFileSave();

	bool IsReadOnly() const						{return m_bReadOnly;}
	__time64_t GetLastSaveAttemptTime() const	{return m_tLastSaveAttempt;} //may return 0

	virtual	IDocumentLink* GetDefaultInterface() PURE;
	CDocumentEx(): m_nID(++m_nCounter),m_bReadOnly(false),m_tLastSaveAttempt(0){}
};

//��������������� ����� ��� ������ � ��������� ������ ������ � ���������������
//��� ��������
class CStatusProcessMessage
{
	string_smart m_strPreviosMessage;
public:
	CStatusProcessMessage (string_adapter strMessageStart);
	~CStatusProcessMessage();
};

//������� ��������� ��� ���� ����� ����������� ����������
interface IApplicationLink: public ILink
{	PURE_INTERFACE(IApplicationLink);

	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		virtual void evOnCreateApp()PURE;		//�������� ���������� (������ ����������)
		virtual void evOnCreateAppMainWnd()PURE;	//�������� ������� �����
		virtual void evOnRunApp()PURE;			//����������� ������� ����� � ������ ����� ��������� (�� ������ ����������)
		virtual void evOnStopApp()PURE;			//��������� ����������, ������ evOnRunApp
		virtual void evOnDestroyApp()PURE;		//���������� ����������, ������ evOnCreateApp
	};

	//���������� IDLE
	interface IIdleHandler
	{
		PURE_INTERFACE(IIdleHandler);
		virtual void OnIdle(bool & IN OUT bStopIdle) PURE;	//if bStopIdle = true, application stops sending the message to another objects
	};

	//��������� ���������� ���� � ��������� ��������� OnIdle
	virtual bool IncludeToOnIdleMap(IIdleHandler* pHandler)  PURE;
	//��������� ���������� ���� � ��������� ��������� OnCmdMsg
	virtual bool ExcludeFromOnIdleMap(IIdleHandler* pHandler)  PURE;

	//�������� ����������. �� ������ ����� ����������� - ���-�� ����� 
	//�� ���� ���������� �� �������� ����������. ���� bForcedClose = true,
	//�� ��������� � ����� ������
	virtual bool Quit(bool bForcedClose=false) PURE; 
	//����� ������ ������� ��������� ��� ��������� ������ AfxMessageBox. ���-�� 
	//�������� � ���������� ������ ������ ���������������
	virtual void SupressMessageBoxes(bool bSupress) PURE;

	//���� ���������� ����� � ������� � ������ ������ �����������������
	virtual registry GetAppVersionBasedRegistryKey() PURE;
	
	//���� ���������� ����� � ������� ��� ����� ������ �����������������
	virtual registry GetAppSteadyRegistryKey() PURE;

	//COM Interface
	virtual ref_cnt_ptr<COM::IApplication> GetComInterface() PURE; 
	
};

//���������, �����������, ��� �������� XXX ������� ��������
struct evdChangeDocumentInfo;
//���������, �����������, ��� �������� XXX �����������
struct evdOpenDocumentInfo;
//���������, �����������, ��� �������� XXX �����������
struct evdCloseDocumentInfo;

//������� ��������� ��� ���� ����� ����������� ����������
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

	//��������� �� ��������
	virtual	CDocumentEx* GetObject() PURE;
	//���������� ���� � ���������
	virtual string_smart GetPath() const  PURE;
	//���������� �������� ���������
	virtual string_smart GetName() const  PURE;
	//���������� ���������� �����, ���������� �� ����. ���� ���� �� ����������, ���������� ���������� �� �����
	virtual string_smart GetExt() const  PURE;
	//��������� ��������
	virtual	bool Close() PURE;
	//��������� ��������
	virtual bool Save() PURE;
	//��������� ��������
	virtual bool SaveAs(cstr szNewFileName) PURE;
	//���������� true, ���� ���� �������� ������ ��� ������
	virtual bool IsReadOnly() const PURE;
	//COM Interface
	virtual ref_cnt_ptr<COM::IDocument> GetComInterface() PURE; 
};

//��������� ��������� �������
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
	//���������� ���� � exe(bin)-�����, ������������ ��� ����������
	//���� ������������� ������ ������������
	virtual string_smart GetExePath() const  PURE;

	//���������� �������� ������, ����� ���� ������������� ����
	virtual	string_smart GetOutputDirectory() const  PURE;
	virtual	void SetOutputDirectory(string_adapter szDirectory) PURE;
	//���������� ����������, ������  c �������� \, ��� ����� ����������� ����������� �����
	//���������� ������ ������������ ���������� �����
	virtual	string_smart GetOutputDirectoryFull() const  PURE;

	//���������� ������������� ������, ����� ���� ������������� ����
	virtual	string_smart GetIntermediateDirectory() const  PURE;
	virtual	void SetIntermediateDirectory(string_adapter szDirectory) PURE;
	virtual	string_smart GetIntermediateDirectoryFull() const  PURE;

	//���������� ������ ���� � ������ ������ ������� 
	virtual	cstr GetFileFirst() const  PURE;
	//���������� ��������� ���� �� ������ 
	virtual	cstr GetFileNext() const  PURE;
	//���������� ������ ���� ������, �������� � ������ � ���������������
	//���������� ����
	virtual	void GetFiles(OUT string_list & lstFileList, int nFileType) const  PURE;
	//���������� �������� � ������ ������, �������� � ������, �� ��������� ����
	//virtual list_sources::iterator FindFile(const char* szFileName) PURE;
	//���������, ������ �� � ������ ������� ��������� ����
	virtual bool IsFilePresent(cstr szFileName) const  PURE;
	//������������� ���� ��������� ���������
	virtual void SetModify(bool bModify=true) PURE;
	//������������� ���� ��������� ���������
	virtual void SetActive(bool bActive=true) PURE;
	//���������, �������� �� ������ ������ ��������
	virtual bool GetActive() const  PURE;
	//��������� ��������+��� ��� ���������
	virtual void SaveAll() PURE;
	//���������� ������� �������������� ����� � �����-���� ������ ������ (fileUnknown, fileSource, fileOther...)
	virtual int GetFileType(cstr szSource) const  PURE;
	
	//���������� ������ ������� 
	virtual string_smart GetNotes() const  PURE;
	//������������ ������ ������� 
	virtual void SetNotes(cstr szNotes) PURE;
	
	//���������� ��������� ������ ����������
	virtual string_smart GetParams() const  PURE;
	//������������ ��������� ������ ����������
	virtual void SetParams(cstr szNotes) PURE;
	
	//���������� ����� ����������
	virtual string_smart GetSwitches() const  PURE;
	//������������ ����� ����������
	virtual void SetSwitches(cstr szSwitches) PURE;

	//��������� ������ �������������� �������� ��� ����������
	virtual void GetBuildSteps(OUT list_build_step & PreBuildSteps, OUT list_build_step & PostBuildSteps) PURE;
	virtual void SetBuildSteps(const list_build_step & PreBuildSteps, const list_build_step & PostBuildSteps) PURE; 	
	//���������� ����� �������������� ����� � ��������� �����
	//����� ��� ���� ������.
	virtual string_smart GetSourceLanguage() const  PURE;
	virtual string_smart GetTargetLanguage() const  PURE;
	virtual void SetSourceLanguage(cstr szName)  PURE;
	virtual void SetTargetLanguage(cstr szName)  PURE;
	
	//����������� ����������, ����� ��� ������, ����� � ����� ������ 
	//�������������� ��������� ������ �/��� �������
	virtual string_smart GetInputTranslationDir() const  PURE;
	virtual string_smart GetOutputTranslationDir() const  PURE;
	virtual void SetInputTranslationDir(string_adapter szName) PURE;
	virtual void SetOutputTranslationDir(string_adapter szName) PURE;

	//���������� ������ ������������ ���������
	virtual void GetLibs(OUT string_list & lst) const  PURE;
	
	//���������� ������ ����������� ����������� ���� �� �� ��� ��������� 
	//�������� ����������. ���������� ���� ���� Plug-Ins, ������� ���������
	//� ����� Generators
	virtual void GetLangGens(OUT string_list & lst) const  PURE;
	//������������� ������ ����������� ����������� ���� 
	virtual void SetLangGens(const string_list & lst)  PURE;

	//��� info-�����, ������������� ������������
	virtual string_smart GetCompilerInfoFilename() const  PURE;
	virtual void SetCompilerInfoFilename(string_adapter szFilename) PURE;

	//���������� ����� ����������� ��������������� ������� 
	virtual parse_report_type GetParseReportType() const  PURE;
	//���� ���� � �����, ������� ����� �������������� ��� ������ log-����� �������
	virtual string_smart GetParseLogPath() const  PURE;
	//���������� true, ���� ���������� ��������������� �� ������ ��������� �� ����
	virtual bool GetParseStopAtMessage() const  PURE;

	//��������� ���� � ������. ���� ������� NULL, �������� ������ ������ �����
	//����� ����� ��������� ����� ����, ���� szFileName �� ����� � ������������
	//���� ����� ���
	virtual bool AddFile(cstr szFileName) PURE;
	//������� ����� ���� � ������. ��� ������������� � ������������
	virtual IDocumentLink* AddNewFile() PURE;
	//��������� ������� � ������ ������������ ���� �������
	virtual bool AddRule() PURE;
	//������� ���� �� �������
	virtual bool RemoveFile(cstr szSource) PURE;
	//��������� ������� ���������� ������ ����
	virtual void Build() PURE;
	//�������� ���� ������ �����������
	virtual void Clean() PURE;
	//��������� ������� ���������� ������ ����
	virtual void Run() PURE;
	//��������� ������� �������
	virtual  void Debug() PURE; 
	
	//�� ���������� ����� ��������� ���� ��������� ���������� �������, ��������, ����������� ������� � �.�.
	//����� ������� NULL
	virtual IDebugManageLink* GetDebugManageLink(cstr szSource) const  PURE;
	//���� ���������� � ����� �������� ��� ���������� ������ � ��������� ������
	virtual  bool GetBreakPoint(cstr szSource, long dwLine, BREAKPOINT & Breakpoint) PURE;	
	//���� ������ ���� ����� ��������, �������������� � �������
	//�������������� � ���� ��� ����� ������ ����� ���������� �� BREAKPOINT::GetModuleName
	virtual  void GetBreakPoints(IDebugManageLink::vector_breakpoints & Breakpoints)  PURE;	
	//��������� ����� �������� ��� ���������� ������ (�������� ����� ������ ������ ��������� BREAKPOINT ������������)
	virtual  void SetBreakPoint(cstr szSource, BREAKPOINT & Breakpoint) PURE;
	//������ ����� �������� ��� ���������� ������ � ��������� ������
	virtual  void ResetBreakPoint(cstr szSource, long dwLine) PURE;	
	//��������� �� ���������� ���������� �������, ���� �� ��������
	//� ��������� ����� ��������. ����� ������� true, ���� 
	//���� �������� ������� �� ������
	virtual bool IsBreakpointExists(cstr szSource, size_t dwLine, BREAKPOINT & bp) const  PURE;

	//COM Interface
	virtual ref_cnt_ptr<COM::IProjectDocument> GetComInterface2() PURE; 
};

//��������� � ������������� ��������� �������� �������
//��� ������ ����� �������� ������ ����� ISourceDocLink::GetViewLink
interface ISourceDocLink;
interface ISourceViewLink:public ILink, public IDebugManageLink
{	PURE_INTERFACE(ISourceViewLink);

	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		virtual void evOnChangeCursorPos (ISourceViewLink * m_pSender, size_t m_nPos) PURE;
	};

	//��������� �� ��������
	virtual	CView* GetObject() PURE;
	//���� ��������� ���������� �������, ��������, ����������� ������� � �.�.
	virtual IDebugManageLink* GetDebugManageLink() PURE;
	//���� ��������� �� ������������ ��������
	virtual ISourceDocLink* GetDocumentLink() const PURE;

	virtual HWND GetWnd() PURE;
	//������ ������
	virtual long GetTextLen() PURE;
	//���� �����
	virtual void GetText(string_smart & strBuffer) PURE;
	//���������� �����
	virtual void SetText(cstr szBuffer) PURE;	
	//������ ���������� ��������
	virtual void GetSelText(string_smart &strBuffer) PURE;
	//����� ��������� ������
	virtual void GetLineText(string_smart & strBuffer,long dwLine) PURE;
	//��������� ���������� ������, ���������� ����� ������� �������
	virtual long AddText(cstr szText, size_t nLen) PURE;
	//������� ������ c ������������� ������������ �������,���������� ����� ������� �������
	virtual long InsertText(cstr szText,size_t nPos) PURE;
	//���-�� �����
	virtual long GetLineCount() PURE;
	//������� ��� �������	
	virtual long GetCurPos () PURE;
	//������� ������	
	virtual long GetCurLine() PURE;
	//������ ������� ������
	virtual long GetFirstVisibleLine() PURE;
	//���-�� ����� �� ������
	virtual long GetLinesOnScreen() PURE;
	//������  ������� �� ��������� ������
	virtual long GetPosFromLine(long dwLine) PURE;
	//��������� ������� �� ��������� ������
	virtual long GetEndPosOfLine(long dwPos) PURE;
	//����� ������ �� ��������� �������
	virtual long GetLineFromPos(long dwPos) PURE;
	//����� ��������� ������
	virtual long GetLineLength(long dwLine) PURE;
	//��������� ������� ����� ���������, ���������� ������� ������
	virtual long OffsetToStartLinePos(long dwPos) PURE;
	//��������� ������� ������ ���������, ���������� ������ ������
	virtual long OffsetToEndLinePos(long dwPos) PURE;
	
	//������ ���������
	virtual long GetSelStart() PURE;
	//����� ���������
	virtual long GetSelEnd() PURE;
	//�������� ��������� ��������
	virtual void SelectRange(long dwStart,long dwEnd) PURE;
	//�������� ������� ��������� �������
	virtual void ReplaceSelection(cstr szNewText) PURE;
	//���������, ���� �� ��������� ������
	virtual bool IsSelected() PURE;

	//������� � ��������� �������
	virtual void GotoPos(long dwPos) PURE;
	//������� � ��������� ������� (������:�������)
	virtual void GotoPos(long dwLine, long dwCol) PURE;
	//������� � ��������� ������
	virtual void GotoLine(long dwLine) PURE;
	//�������������� ����� ���������� �������
	virtual void EnsureRangeVisible(long nStartPos,long nEndPos) PURE;

	//��������� ��������� ������� �������� ������: ��� ������� �������, ��� ������ ��������, ����� ��������� �� x
	virtual void GetViewPos(long & Pos, long & FirstLineVisible, long &FirstColVisible)  PURE;
	//��������� ��������� ������� ��������
	virtual void SetViewPos(long dwPos, long FirstLineVisible,long FirstColVisible)  PURE;

	virtual  void SetBookmark(long dwLine) PURE;
	virtual  void ResetBookmark(long dwLine) PURE;	
	virtual  bool GetBookmark(long dwLine) PURE;	
	virtual  void GetBookmarks(vector<long> & bookmarks) PURE;
};

//���������  � ��������� �������� �������
//�������� ���� ��������� ���� i_objects::GetInterface<> ������, ��� ��� ��� ����� 
//�� ���������, �� ���� ����� �������� CDocument * �� DocManager
//� ����� �������� dynamic_cast
interface ISourceDocLink:public IDocumentLink
{	PURE_INTERFACE(ISourceDocLink);

	//��������� �� �������������. ����� �������� ���� ��� this=NULL, ��������� ������������
	virtual	ISourceViewLink* GetViewLink() PURE;
	//���� ��������� ���������� �������, ��������, ����������� ������� � �.�.
	virtual IDebugManageLink* GetDebugManageLink() PURE;
	//���������� ����� �������� ��������� ��������� � �����	
	virtual clock_t GetTimeOfLastChange() PURE;
	
	//COM Interface
	virtual ref_cnt_ptr<COM::ISourceDocument> GetComInterface2() PURE; 
};

//���������  � ��������� ����������
interface IDocMngrLink:public ILink
{	PURE_INTERFACE(IDocMngrLink);
	
	struct doc_iterator{
		void * pData1;
		void * pData2;
	};
	
	//��������� �� MFC - �����
	virtual	CDocManager* GetObject() PURE;

	virtual void SetActiveProject(IProjectDocLink* pProject) PURE;
	//���� �������� �������� �������
	virtual IProjectDocLink* GetActiveProject() const  PURE;
	//���� �������� �������� ���������
	virtual ISourceDocLink* GetActiveSourceDoc() const  PURE;
	
	//����� ��������� ����� �������� �� ��� ����. ���������� ��������� � ���������� ���������
	virtual IDocumentLink *	 FindDocument(cstr szDocPath) PURE;
	
	//������� ����� �������� �������� ���, � �������� � ������� ������ ��������
	//��������� ��������. ���������� ���������, ��� ���� � ��� �� �������� ����� �������
	//� ������ ��������� ��������. ������� ������� ����������� �������� ������, 
	//� ����� - �� �������
	virtual IProjectDocLink* FindParentProject(cstr szDocPath)  PURE;

	//���������� �� ������ ������ ������ �����
	virtual bool SelectFile(
							string_list	 & out_lstFiles,
							cstr	in_szInitialDir=NULL,//�� ���������� ���������
							cstr	in_szCaption=NULL,	//�� ��������� Open
							cstr	in_szFilter=NULL,	//�� ����� ������������� ������������������ ������� ����������+ "*.*" ���, ���� 
							cstr	in_szDefaultExt = NULL, //� ������ �������������� ���������� ������������� ��������� ���������� � �� ��������� ������ ��� ����� ������� � �������
							bool	in_bOpen=true,		
							bool	in_bUseRegisteredFilters=true,		//in_bUseRegisteredFilters=false, ������ "*.*" 
							bool	in_bAllowMultipleSelection=true,	//����� �������� ��������� ������
							bool	in_bFileMustExist=true,				//������ �� ������������ ����
							bool	in_bCreatePrompt=true) PURE;			//���� ���� �� ����������, ���������� �� � ��� ��������
	
	//��������� �������� ���� ��� �������� ������������ ������� � ������ ���� �������� 
	//���������
	virtual string_smart GetInitialDir(string_adapter szDefInitialDir="") PURE;
	virtual void SetRecentlyDir(string_adapter strDir) PURE;

	//������� ��������� ��������� �� ������ ���������� �������. ����� ��������� NULL, ����� 
	//��������� ���������� ���� ������ ������
	virtual bool OpenDocument(CDocTemplate * pDocTemplate) PURE;

	//������� ��������������� ������ �� ��������� ����������
	virtual CDocTemplate* FindDocTemplate(cstr szExt) PURE;

	//��������� ��������� �������� � ���������� ���������(!!!) �� ����
	//���� ������� �� �������, ���������� NULL. �������� bSilentError ���������
	//�������� �� ����������� ��������� ���� "�������� �� ������" �� ������������
	//�������� szAllowedExts ����������, ������ �� ���� �������� ������������ ������-�� 
	//������������� ���� ����������(�� ����������). ���� �� �� ����������� ����� ����, �� � �� �����������
	virtual IDocumentLink* OpenDocument(cstr szDocPath, cstr szAllowedExts = NULL, bool bSilentError=false) PURE;
	
	//���������� ������ �������� � ������ �������� ���������� (������, � �� ���������!)
	virtual	CDocumentEx* GetDocumentFirst(doc_iterator & it) PURE;
	
	//���������� ��������� �������� �� ������. ���� ���������� NULL, ������, ������ ������ ���
	virtual	CDocumentEx* GetDocumentNext(doc_iterator & it) PURE;
	
	//�������� ������� ��� ���������
	virtual bool CloseAllDocuments() PURE;
};

//���������  � ����� ���������
interface IBuilderLink : public ILink
{	PURE_INTERFACE(IBuilderLink);

	//������ ����������. ������ ���������� ����� ���������� �������
	//�������� ������� void pf(bool bResult), ������� ���������� ��� ��������� 
	//������
	typedef functors::functor<void,ARGS_1(bool)> functor_build;
	virtual void Build( IProjectDocLink * pProjectLink,
						functor_build pfCallBk=functor_build())  PURE;	
	
	//�������������� ��������� �������� ����������
	virtual void Stop()  PURE;		

	//����������� �� � ������ ������ ����������
	virtual bool IsBusy() PURE;	

	//���� ����������� ����������, �� ������ �������
	virtual IProjectDocLink * GetBuildingProject() PURE;
};

//���������  � ����� �������
interface IRunnerLink : public ILink
{	PURE_INTERFACE(IRunnerLink);

	enum EActivityType{atNone, atRun, atDebug};
	//������ ����������. ���������� ���������� �������� ����������� ��������
	virtual HANDLE Run(IProjectDocLink * pProjectLink)  PURE;
	//������ �������. bInitiallySuspended ��������� ������������� �� ���������� ���� ����� �������� �������������
	virtual void Debug(IProjectDocLink * pProjectLink,bool bInitiallySuspended=true)  PURE;
	//����� �������� ����������
	virtual void Pause() PURE;
	//����������� ���������� 
	virtual void Resume() PURE;
	//����������� ���������� �� ���� �������
	virtual void ResumeStepOver() PURE;
	//����������� ���������� �� ���� �������
	virtual void ResumeStepIn() PURE;	
	//�������������� ��������� �������� ����������
	virtual void Stop() PURE;
	//����������� �� � ������ ������ ���������� ��� �������
	virtual bool IsBusy() PURE;
	virtual bool IsBusy(EActivityType & ActivityType, IProjectDocLink *& pActiveProjectLink) PURE;
	virtual bool IsPaused() PURE;
	
	//�������� �� ����������������� ����� ��������
	virtual bool IsBreakpointUsed(cstr szModuleName,size_t nLine) PURE;
};

interface IClassTreeLink: public ILink
{PURE_INTERFACE(IClassTreeLink);

	//�������� (������ ��������) ������� �� ������� � ��������� ����� ���������� �������
	virtual bool SelectSyntaxItem(cstr szProjectName, cstr szFileName, long nPos)  PURE;
};

interface IParserLink: public ILink
{	PURE_INTERFACE(IParserLink);

	class syntax_item;
	typedef vector <syntax_item> vector_items;
	typedef vector_items::iterator  si_iterator;

	typedef vector <syntax_item *> vector_pitems;
	typedef vector_pitems::iterator  psi_iterator;
	enum ps_res {psrOk, psrFail, psrImpossible}; //��������� �� ���� ��������� - ��, ���, ����������
	enum word_class {wcKeyWord, wcReservedVar, wcReservedFunc, wcUserIndentifier, wcUnknown};

	//�������� ��������� ��� Parser. ��������� ����� �������������� �������
	class syntax_item
	{
	protected:
		vector_items m_Units;	//��������� ������c������� ������� 
								//�� ��������, � ��������� � �������!
		size_t	m_nID;			//��� �������
	public:
		enum item_class	{icUndefined,icRule,icVariant,icTerm,icFunc,icStruct};
		enum symbol_kind {tkUndefined,tkNormal, tkOptional, tkChildOptional};
		enum symbol_class {tcUndefined,tcTerminal,tcNonTerminal,tcStdTerminal};

		//����������� ���������� ������� ������, ��� ����� ����� ������� ������ � string_std
		//���� ����� ������ ��� ����� ����� � ���� �������, ������� ��������� �����
		//��������� �������� ���������� "��������" ������
		string_std strName;		//������������
		string_std strFilename;	//� ����� ����� ������ ������ ���������. ����������� �� ����� ���������
		string_std strData;		//����� ������
		long		 nPosStart;		//������ � ������
		long		 nPosEnd;		//����� � ������
		item_class	 nClass;		//������������� �������� (�������, ��������� � ����)
		symbol_kind	 nSymbolKind;	//��� �����: �������, ������������, �����������-���������
		symbol_class nSymbolClass;	//����� �����: ��������/���������� � ����
		void*		 pData;			//���������������� ������
		long		 nData;			//���������������� ������
		
		//string_smart strActionsPre;	//����� ��� ��������� ��� ��������, ��������� � ������
		string_smart strActions;	//�������� ������ ���� � �� (���� �� ��������������)

		vector_pitems pParents;		//��������� �� ������������ � �������� �������
		vector_pitems pChildren;		//������������ �������� ��� ���������� ��������
										//��������, ����� ���������� ����������� ����� ���������
		
		syntax_item(){
			Clear();
		}

		//�������� �� ������ �������� ������. ������ ������� ���������� ����� ������� 
		//�����
		si_iterator GetFirstUnitPos(){
			return m_Units.begin();
		}
		//���� ������� �������������� � ��������� ��������� �� ��������� ������
		syntax_item & GetNextUnit(si_iterator & it){
			return *it++;
		}
		//����� ��������
		bool IsEndUnitPos(si_iterator it){
			return it==m_Units.end();
		}
		//����� �������� �������� ������ �� ����������, �� ��� ���������
		syntax_item & GetUnitAt(int index){
			return m_Units[index];
		}
		//��������� �������� ������� � ������
		syntax_item & AddUnit(){
			m_Units.push_back(syntax_item());
			return *m_Units.rbegin();
		}
		//���-�� �����
		size_t GetUnitsCount(){
			return m_Units.size();
		}
		//�������
		void Clear(){
			m_Units.clear();
			nPosStart=nPosEnd=-1;nClass=icUndefined;nSymbolClass=tcUndefined; nSymbolKind=tkUndefined; pData=NULL;
		}
	};
	
	
	//������� �������� ����������� �������������� ������ - ������ 
	//���� �� � ����������� �� syntax_item, �� �� ����� ��������� nPosStart, nPosEnd � �.�.
	class syntax_item_root: public syntax_item
	{
	protected:
		syntax_item::strName;			//�������� ���� �������� ������
		syntax_item::nPosStart;		
		syntax_item::nPosEnd;		
		syntax_item::nClass;	
	};
	
	//�����-������� ��������� ������� ��������� �������������� ������
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
		//������ ��������, ������������ �� ������ ������� syntax_info
		sir_iterator GetBeginPos(){
			return 0;
		}
		
		//������ ��������, ������������ �� ��������� ������� syntax_info
		sir_iterator GetEndPos(){
			return 3;
		}
	};

	//��������� ������� ����� � ������ �� ������ ��������� syntax_info 
	//�������� szFileName � ������ ������ ������ ���� ����� ��� ������ ������ �� ����������
	//��������. ������������� ��������� ���� �������� ��������, ��� �������� ������������ ���
	virtual void ParseSourceText	(const string_smart & strText, cstr szFileName, syntax_info & Result, bool bAllowUseCache=true) PURE;
	//��������� ����� �� ���������� ����� � ������ �� ������ ��������� syntax_info 
	virtual void ParseSourceFile	(cstr szFileName, syntax_info & Result, bool bAllowUseCache=true) PURE;
	//��������� ����� �� ���� ������ ���������� �������  
	virtual void ParseWholeProject	(IProjectDocLink & IProject, syntax_info & Result, bool bAllowUseCache=true) PURE;
	//���� � ��������� ������ ������� � ��������� ������. ���� ������� �������,
	//���������� ������� ������ � ����� ����� ����� ������� �� ����� rule �� ������ }
	//������������
	virtual bool FindRule			(const string_smart & strText, cstr szRuleName, size_t & nStart, size_t & nEnd) PURE;
	
	//���� ����������� ���������� ������� �� ���� ������ ������� 
	//� aMetAt ���������� ����������� ����� (���� � �������), ��� �� ��� �����, ��� �������� ������� ��������� �����
	//��������� ������� ������������ � aFoundAt
	//����� ��������� �� ���� ������ ������� � ������� ����������
	virtual ps_res FindDeclaration(cstr szItemName, ITEMLOCATION aMetAt, ITEMLOCATION & aFoundAt) PURE;

	//���� ����������� �������������� ����������� � ������� nPos ����� szFileName
	virtual ps_res DetectSyntaxItem(cstr szFileName, size_t nPos, OUT syntax_item & aFoundItem) PURE;

	//���������� ����� ���������, ��� ����� ������������� ������ ���������
	//���������� true, ���� ������� ���� ���-�� �������. � ������ lstMostUsed ����� ����������
	//�����, ������� ����� ������� �������� ����������������
	virtual bool SuggestAutoComlete(cstr szItemName, const string_list & lstMostUsed, ITEMLOCATION aMetAt, string_list & aSuggestions) PURE;
	
	//���������� ������������� ����������� �����
	virtual word_class GetWordClass(cstr szItemName) PURE;

	//�� ���������� ��������� syntax_info ���������� �����. ������ strOutText ���������� �� ����������!
	virtual void AssembleToText		(syntax_info & SyntaxData, string_smart & strOutText) PURE;

	//�� ���������� ��������� syntax_info ���������� ����� � ������� ����. ������ strOutText ���������� �� ����������!
	virtual void AssembleToXBNF		(syntax_info & SyntaxData, string_smart & strOutText) PURE;
	
	//�� ���������� �������� ������� ��� ����� (��������-������� ����� ����) ����� ���� 
	virtual void BuildRuleDependence(syntax_item_root & m_Rules) PURE;

	//������ ����� ������� ������� main, 
	virtual string_smart ComposeMainFunction(cstr szRootRule, 
									bool bIgnoreCase,
									list<string_pair> lstIgnore,
									list<string_pair> lstReplace) PURE;

	//������ ������ ���� ������� � �������� ����������� ���������� (#IL, #OL)
	virtual void FindTranslationDirections(IN IProjectDocLink & IProject, OUT string_list & InputDirs,  OUT string_list & OutputDirs) PURE;

	//COM Interface
	virtual ref_cnt_ptr<COM::IParser> GetComInterface() PURE; 
};

//��������� � ������� About
interface IAboutLink : public ILink
{
	virtual void Run() PURE;
};

//��������� � ������� TipOfTheDay
interface ITipOfTheDayLink : public ILink
{
	virtual void Run() PURE;
};

//��������� � ������� AddRule
interface IAddRuleLink : public ILink
{
	virtual bool Run() PURE;
};

//���������  � ������� ���������� ����� ������ ������
interface IKnowledgeBaseLink: public ILink
{	PURE_INTERFACE(IKnowledgeBaseLink);
	
	struct translate_pair{
		string_std m_strSourceLanguage;
		string_std m_strTargetLanguage;
		string_std m_strModulePath;
	};
	typedef list<translate_pair> tt_pair_list;

	//�������� ���������� ������� � ���� ������
	virtual bool Add(IProjectDocLink * pProjectLink)  PURE;
	//���������� ��� ���� � ���������� �������
	virtual bool ShowRegistry() PURE;
};

//���������  � ������� ����������� ���� ��� ��������������� ������ �� ����� �����  (Plug-Ins)
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

	//���������� ������ ���� ��������� ��������
	virtual void GetList(lst_lang_gen & lst) const  PURE;
	
	//�� ����� ���������� ����������� ������ ���� � DLL
	virtual bool FindByName(const string_adapter strName, OUT lang_gen_item & Item) const  PURE;
};

//���������  � ������� ��������� ��������� �� ��������� ������
interface IGrammaticalInferenceLink: public ILink
{	PURE_INTERFACE(IGrammaticalInferenceLink);
	virtual bool Run() PURE;
};

//���������  � DockedBar Control. Multiple Instance!!!
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

	//�������� ���� DockBar �� ������� IBarLink
	virtual void CreateWnd(CFrameWnd* pParentWnd) PURE;
	//����������� Dock-������
	virtual void InitDock() PURE;
	//���������� ������� ������, ����� ��� �������� ����������
	virtual void OnShowMainFrame(bool bVisible) PURE;
	//��������� �� ������� DockBar
	virtual CControlBar * GetObject() PURE;
	//��������� �� TabWnd, ���� ������� �������
	virtual CBCGTabWnd* GetTabWnd(){return NULL;}
};

//���������  � MainFrame
interface IMainFrameLink: public ILink
{	PURE_INTERFACE(IMainFrameLink);

	//��������� �������
	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);

		//���������, �����������, �� �������� �������� ���� ���������
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

	//��������� ���������� ���� � ��������� ��������� OnCmdMsg
	virtual bool IncludeToCmdMsgMap(CCmdTarget* pHandler)  PURE;
	//��������� ���������� ���� � ��������� ��������� OnCmdMsg
	virtual bool ExcludeFromCmdMsgMap(CCmdTarget* pHandler)  PURE;

	//������ ��������� ������� � �������������� �� ������� ���������
	//��� ������� ��� �������, ����� �� ����� ��������� ������������ 
	//������� � ��������� �������� ������. ���������, �� ������� 
	//������� ������� IMainFrameLink (��� ����� ��� ���������������)
	virtual void SyncInvoke(functors::IFunctorRemote & func)  PURE;

	//��������� ���������� ������ ��������� � ������� StatusBar
	virtual void ShowTip(cstr szTipText)  PURE;

	//���� ������� �������� ���� CView ���������
	virtual CView * GetActiveView() const  PURE;

	//���������� ����� ����������� � StatusBar
	virtual void SetStatusBarIndicators(const UINT* lpIDArray, int nIDCount, bool bInitiallyDisabled) PURE;

	//���������� � ��������� ������ �����, ���� "Ready" � ����
	virtual void SetStatusMessage(string_adapter strText) PURE;
	virtual string_smart GetStatusMessage() PURE;
};

//��������� � WatchList
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

	//��������� ������ ������. ���� ���� ������������� ��� ���������� � ������
	//����� �������������� � ��������� �����������
	virtual void StartSession(bool bClear=true) PURE;
	//��������� ������ ������. ����� �������������� � ��������� �����������
	virtual void StopSession(bool bClear=true) PURE;
	//������� ����� ������
	virtual  void Clear() PURE;
	//���������� ���-�� ��������� � ������
	virtual size_t GetItemCount() PURE;	
	//� ��������� ������� ��������� ����� ���������� WATCHITEM
	virtual WATCHITEM GetItem(size_t nIndex) PURE;
	//� ��������� ������� ��������� ����� ���������� WATCHITEM
	virtual void SetItem(size_t nIndex, const WATCHITEM & WatchItem) PURE;
	//������� ���������� ���������� � ������������� ����� ������
	//��������� WatchList ���������� ���������� ������ � ��������
	//� ������������ ���������� ��������
	virtual  void SetItems(WATCHITEM::vector_items & WatchItem) PURE;
};

//��������� � IWatchParsePanel
interface IWatchParsePanel: public ILink
{	PURE_INTERFACE(IWatchParsePanel);

	virtual void SetTextBehind(cstr szText) PURE;
	virtual void SetTextAhead(cstr szText) PURE;
};

//��������� ��� ������ ���������. ������������ ��� ������ ��������� ����������� 
//� ��������������� ���������
interface IOutputLink: public IMultiInstanceILink
{	PURE_INTERFACE(IOutputLink);

	enum ID
	{
		idOutputScreen,
		idOutputBuild,
		idOutputParse,
		idOutputFindInFiles
	};

	//��������� ������ ������. ���� ���� ������������� ��� ���������� � ������
	//����� �������������� � ��������� �����������
	virtual void StartSession(bool bClear=true) PURE;
	//��������� ������ ������. ����� �������������� � ��������� �����������
	virtual void StopSession(bool bClear=false) PURE;
	//����� ���������� ��������� ������
	virtual void Print(cstr szText) PURE;
	//������� ���� (��� ������) �� ���� ���������� ���������
	virtual void Clear() PURE;
	//��������� ����� ����������� ������
	virtual string_smart GetText() PURE;
};

//��������� ��� ������� ������ ������ � ������ (FindInFiles)
interface IGotoLineLink: public ILink
{
	virtual bool Run(long dwCurLine, long dwLastLine, OUT long & out_dwNewLine) PURE;
};

//��������� ��� ������� ������ ������ � ������ (FindInFiles)
interface IFindTextLink: public ILink
{	PURE_INTERFACE(IFindTextLink);

	//��������� ��� FindDialog. ����� ��� ������������ ����� ������� Find
	//� ����������� �����������
	struct vbdFind
	{
		bool in_bInSelection;
		string_smart in_strText;
		long in_dwInitialSelStart;	//�������������� FindDialog �� ������������
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

	//������ ����� 
	virtual bool Run(vbdFind & Data) PURE;
};

//��������� ��� ������� ������ ������ � ������ (FindInFiles)
interface IReplaceTextLink: public ILink
{	PURE_INTERFACE(IReplaceTextLink);

	struct vbdReplace: public IFindTextLink::vbdFind
	{
		enum operations{opFindNext, opReplace, opReplaceAll};
		// ������� bool (operations)
		typedef functors::functor<bool,ARGS_2(operations, HWND)> functor_find;

		functor_find in_pfCallback;
		string_smart out_strReplaceText;

		vbdReplace():vbdFind(){
		}
	};

	//������ ����� 
	virtual bool Run(vbdReplace & Data) PURE;
};

//��������� ��� ������� ������ ������ � ������ (FindInFiles)
interface IFindInFilesLink: public ILink
{	PURE_INTERFACE(IFindInFilesLink);
	//������ ����� 
	//szFindWhat - ����� �� ���������, ������� ������
	//pOutput - ��������� �������, � ������� ����� �������� ���������
	virtual bool Run(cstr szFindWhat, IOutputLink * pOutput) PURE;
};

//��������� ��� ������� ������� ���������� � ������������ ������
interface ISetParamsLink: public ILink
{	PURE_INTERFACE(ISetParamsLink);

	//������ ����� ��������� ����������
	//strParams - �������� ������ ���������� � ���� �� ������������ ���������� ������
	//���� ������������ �� �������  �������� ��� ����� "������", ������������ false.
	virtual bool Run(string_smart & strParams, string_smart & strBrowseInitialDir, bool & bShowInFuture) PURE;
};

//��������� ��� ������� ������� ���������
interface IImportGrammarLink: public ILink
{	PURE_INTERFACE(IImportGrammarLink);

	virtual bool Run() PURE;
};

//��������� ��� ������� �������
interface IProjPropsLink: public ILink
{	PURE_INTERFACE(IProjPropsLink);

	//�������� �������
	virtual bool Run(IProjectDocLink * pProjectLink) PURE;
};

//��������� ��� ������� �������
interface ISourcePropsLink: public ILink
{	PURE_INTERFACE(ISourcePropsLink);

	//�������� 
	virtual bool Run(cstr szFileName) PURE;
};

//��������� ��� ������� ����������� � ����������� ������ � �����������
interface IStatisticLink: public ILink
{	PURE_INTERFACE(IStatisticLink);
	
	//������ ����� ���������� � �����������
	virtual bool Run(IProjectDocLink * pProjectLink) PURE;
};
//��������� ��� ������� ������ ������ � ������ (FindInFiles)
interface ISyntaxDiagramsLink: public ILink
{	PURE_INTERFACE(ISyntaxDiagramsLink);
	enum show_type {stTree,stDiagram};

	//������ ����� 
	virtual bool Run(IProjectDocLink * pProjectLink, show_type nType) PURE;
};

//��������� ��� ������� ��������� ����������� ���������
interface IViewLibLink: public ILink
{	PURE_INTERFACE(IViewLibLink);

	//������ �����
	virtual bool Run() PURE;
};


//��������� ��� ������� ���������� ������ �������� � ������
interface INewDocumentWizardLink: public ILink
{	PURE_INTERFACE(INewDocumentWizardLink);

	//����� �����������, ������������� �������� ��� ���������,
	//���� ��������� � ����������� � ��������� ������. ���������� 
	virtual IDocumentLink * Run(IProjectDocLink * pProjectLink) PURE;
};

interface INewProjectWizardLink: public ILink
{
	PURE_INTERFACE(INewProjectWizardLink);

	//����� �����������, ������������� �������� ��� ���������,
	//���� ��������� � ����������� � ��������� ������. ���������� 
	virtual bool Run(IProjectDocLink * pProjectLink) PURE;
};

//��������� ��� �������, ����������� ��� ������������ �������
//���������� ���������� ����� ��������� ��������, 
struct INewProjectLink: public ILink
{
	PURE_INTERFACE(INewProjectLink);


	virtual bool Run(	string_adapter szProjectNameDef, //��� ������� �� ���������
						string_adapter szProjectDirDef,	//������������ ���������� �� ���������
						OUT string_smart & out_strProjectName,	//��� �������
						OUT string_smart & out_strProjectDir,	//������������ ����������, ��� ����� �������� ������
						INewProjectWizardLink* & pWizardLink 
					)  PURE;
};

//��������� ��� ������� �������� ���������
interface IPreferencesLink: public ILink
{	PURE_INTERFACE(IPreferencesLink);
	
	//������ ����� 
	virtual bool Run() PURE;
};

//--=========================== EVENTS  ================================--

//���������, �����������, ��� �������� XXX ������� ��������
struct evdChangeDocumentInfo
{
	enum {changeModify,changeSave,changeOpen,changeClose};
	//changeOpen � changeClose ������������ � �����������
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
//���������, �����������, ��� �������� XXX �����������
struct evdOpenDocumentInfo: virtual public evdChangeDocumentInfo
{
	//-----------
	evdOpenDocumentInfo(cstr szDocumentName, cstr szDocumentPath,IDocumentLink & DocIface):
			evdChangeDocumentInfo(szDocumentName,szDocumentPath,DocIface)
	{
		in_nChangeType = changeOpen;
	}
};

//���������, �����������, ��� �������� XXX �����������
struct evdCloseDocumentInfo: virtual public evdChangeDocumentInfo
{
	evdCloseDocumentInfo(cstr szDocumentName, cstr szDocumentPath,IDocumentLink & DocIface):
		evdChangeDocumentInfo(szDocumentName,szDocumentPath,DocIface)
	{
		in_nChangeType = changeClose;
		
	}
};
//----------------------- Project Data Info ------------------------------
//���������, �����������, ��� �������� ������� ������� ��������
struct evdChangeProjectInfo: virtual public evdChangeDocumentInfo
{
	enum {idProject=idDocument+1};
	enum {changeAddFile=0xFF,changeRemoveFile,changeActivate,changeDeactivate};
	string_smart in_strIncludedFile; //����������� ������ � changeAddFile;changeRemoveFile

	evdChangeProjectInfo(cstr szDocumentName, cstr szDocumentPath,IProjectDocLink & DocIFace):
		evdChangeDocumentInfo(szDocumentName,szDocumentPath,DocIFace)
	{
		in_nDocID=idProject;
		
	}
};
//���������, �����������, ��� �������� ������� �����������
//����� �������� ��������� ������������ �� ����������� �� ���� �������� ���������
//��� ��������� �� evdOpenDocumentInfo
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
//���������, �����������, ��� �������� ������� �����������
//����� �������� ��������� ������������ �� ����������� �� ���� �������� ���������
//��� ��������� �� evdCloseDocumentInfo
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
//���������, �����������, ��� �������� ������� ������� ��������
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
//��������� ��� WorkSpacBar. ����� ��� �������������� ���������� �������� ������ � ������ 
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
//���������, �����������, ��� �������� ������� �����������
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
	//�������
	interface IEventSink:public i_objects::IEventSink
	{
		PURE_INTERFACE(IEventSink);		
		//��������� ����������. � �������� ��������� ParameterChanged
		//�������� ������������� ������������� �������
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

	//����� ��� ������ �� ������
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

		long			m_clFore;	//����� ���� <0
		long			m_clBack;	//����� ���� <0

		bool			m_bVisible; //�������� ��� ��������� �������������
		//??
		bool			m_bEolFilled;	// true by default
		int				m_iCaseForce;	//case - SC_CASE_UPPER, SC_CASE_LOWER, SC_CASE_MIXED
		int				m_nCharset;
	};		

	//COM-��������� ������ �������, �� �� ������� ��� ��������
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

	//�� ���������� ������� �������������� ���������, ����������� �������� ���������
	virtual IDocumentStyleDescriptor * FindStyleDescriptor(string_adapter strFileExt) PURE;

	//��� ���, ���� �� �������
	//virtual size_t GetCharset() PURE;

	//�������� ������ �������������� ���������� ������
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
		//��������� ����������. � �������� ��������� ParameterChanged
		//�������� ������������� ������������� �������
		virtual void evOnChangeData(EParam ParameterChanged) PURE;			
	};

	//��������� �������� ������
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
