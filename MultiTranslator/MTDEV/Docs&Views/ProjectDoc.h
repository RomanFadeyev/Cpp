/********************************************************************
	created:	2002/02/23
	created:	23:2:2002   12:59
	filename: 	$(Projects)\Sources\MTDEv\Docs&Views\ProjectDoc.h
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	ProjectDoc
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	���������� ��������� ������� (Project)
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

//�������� ������� ������������ ������ � � ������� CO. ��� �������� ��� �� ������
//����������� �������� �������
class CProjectDoc : public CDocumentEx, public CCObjectEx, 
					public IProjectDocLink, 
					public IDocumentLink::IEventSink
{
public:
	//��� ������ ������, �������� � ������
	typedef list<string_smart> list_sources; 
	typedef list_sources::iterator it_file;
	typedef list_sources::const_iterator cit_file;
	typedef string_list	list_libs;
	typedef string_list	list_lang_gen;
	
	typedef IDebugManageLink::vector_breakpoints::iterator dml_vb_i;
	typedef IDebugManageLink::vector_breakpoints::const_iterator dml_vb_ci;
	//������� ���������� ��������� ��������
	enum op_state  {opNone=0, opOpening=1, opSaving=2, opCreating=4, opClosing=8, opChangingBreakpoints=16};

	//�����, ������������ ������� � ����������� ������ �������
	class CProjectProps:public property_set
	{
	public:
		//��������� ����� ����� �� ������
		void ReadFromArchive(CArchive & ar);
	};
	//����� - �������, ����������� ����� � ������ ��������� � ������� ��������� ��������� 
	//������ ������������ � �������� ���������. �������� ���� - ���������� ������
	//�������������� ����� ���� ��� ������������� ����������
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
		void Rollback(){	//������� ��������� ������ �������.
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
	
	//��������, �������� �������� ���������� ��������� ���������, � �������
	//�������� �����
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
	//��������� IProjectDocLink ������ ��������� ������� � ������� ���������

	//From CDocumentEx
	override IDocumentLink* GetDefaultInterface()		{return this;}

	//Multiple Instance Link
	override size_t GetObjectID()const	{return CDocumentEx::GetObjectID();}

	//From IDocumentLink
	override CDocumentEx* GetObject()					{return this;}	//��������� �� ��������

	override bool Close();						//��������� ��������
	override bool Save();						//��������� ��������
	override bool SaveAs(cstr szNewFileName);	//��������� ��������
	override string_smart GetPath() const;		//���������� ���� � �������
	override string_smart GetName() const;		//���������� �������� �������
	override string_smart GetExt()  const;		//���������� ���������� �������� ����
	override bool IsReadOnly() const;
	
	//From IProjectDocLink
	override ref_cnt_ptr<COM::IDocument> GetComInterface(){return GetComInterface2().get();}
	override ref_cnt_ptr<COM::IProjectDocument> GetComInterface2();
	override int  GetFileType(cstr szSource) const;	//���������� ������� �������������� ����� � �����-���� ������ ������
	override void SetNotes(cstr szNotes);		//����������� �������
	override void SetParams(cstr szParams);		//����������� ������ ����������	
	override void SetModify(bool);				//������������� ���� ��������� ���������
	override string_smart GetSwitches() const;	//���������� ����� ����������
	override void SetSwitches(cstr szSwitches);	//������������ ����� ����������
	override void GetBuildSteps(OUT list_build_step & PreBuildSteps, OUT list_build_step & PostBuildSteps); 	//��������� ������ �������������� �������� ��� ����������
	override void SetBuildSteps(const list_build_step & PreBuildSteps, const list_build_step & PostBuildSteps); 	//��������� ������ �������������� �������� ��� ����������

	//���������� ����� �������������� ����� � ��������� �����
	//����� ��� ���� ������.
	override string_smart GetSourceLanguage() const;
	override string_smart GetTargetLanguage() const ;
	override void SetSourceLanguage(cstr szName);
	override void SetTargetLanguage(cstr szName);

	override string_smart GetInputTranslationDir() const;
	override string_smart GetOutputTranslationDir() const;
	override void SetInputTranslationDir(string_adapter szName);
	override void SetOutputTranslationDir(string_adapter szName) ;

	//��� info-�����, ������������� ������������
	override string_smart GetCompilerInfoFilename() const;
	override void SetCompilerInfoFilename(string_adapter szFilename);

	override parse_report_type GetParseReportType() const;//���������� ����� ����������� ��������������� ������� 
	override string_smart GetParseLogPath() const; //���� � �����, ������� ����� �������������� ��� ������ log-����� �������
	override bool GetParseStopAtMessage() const;//���������� true, ���� ���������� ��������������� �� ������ ��������� �� ����
	override void SetActive(bool);				//������������� ���� ��������� ���������
	override bool GetActive() const;			//���������, �������� �� ������ ������ ��������
	override void SaveAll();					//��������� �������+��� ��� ���������
	override bool RemoveFile(cstr szSource);
	override bool AddFile(cstr szFileName);
	override IDocumentLink* AddNewFile();
	override bool AddRule();
	override bool IsFilePresent(cstr szFileName) const;//���������, ������ �� � ������ ������� ��������� ����
	override string_smart GetNotes() const;		//��������� ������ �������	
	override string_smart GetParams()const;		//��������� ������ ����������
	override string_smart GetExePath()const;	//���������� ���� � ����������������� ������������ �����
	
	//���������� �������� ������, ����� ���� ������������� ����
	override string_smart GetOutputDirectory() const;
	override void SetOutputDirectory(string_adapter szDirectory);
	override string_smart GetOutputDirectoryFull()const;//���������� ����������, ������  c �������� \, ��� ����� ����������� ����������� �����
	//���������� ������������� ������, ����� ���� ������������� ����
	override string_smart GetIntermediateDirectory() const;
	override void SetIntermediateDirectory(string_adapter szDirectory);
	override string_smart GetIntermediateDirectoryFull()const;//���������� ����������, ������  c �������� \, ��� ����� ����������� ����������� �����

	override cstr GetFileFirst() const;			//���������� ������ ���� � ������ ������ ������� 
	override cstr GetFileNext() const;			//���������� ��������� ���� �� ������ 
	override void GetFiles(OUT string_list & lstFileList, int nFileType) const;
	override void GetLibs(OUT string_list & lst) const;//���������� ������ ������������ ���������
	override void GetLangGens(OUT string_list & lst) const; //���������� ������ ����������� ����������� ���� �� �� 
	override void SetLangGens(const string_list & lst);		//������������� ������ ����������� ����������� ���� 
	override void Build();						//��������� ������� ���������� ������ ����
	override void Clean();						//�������� ���� ������ �����������
	override void Run();						//��������� ������� ���������� ������ ����
	override void Debug();						//��������� ������� �������
	override void AddToKnowledgeBase();			//��������� ���� � ���� ������ �����������
	override IDebugManageLink* GetDebugManageLink(cstr szSource) const;
	override bool IsBreakpointExists(cstr szSource, size_t dwLine, BREAKPOINT & bp) const;
	override bool GetBreakPoint(cstr szSource, long dwLine, BREAKPOINT & Breakpoint);
	override void GetBreakPoints(IDebugManageLink::vector_breakpoints & Breakpoints);
	override void SetBreakPoint(cstr szSource, BREAKPOINT & Breakpoint); //��������� ����� �������� ��� ���������� ������ (�������� ����� ������ ������ ��������� BREAKPOINT ������������)
	override void ResetBreakPoint(cstr szSource, long dwLine); //������ ����� �������� ��� ���������� ������ � ��������� ������
	//--------------------------------------------------------------------
protected:
	COM::ProjectDocument* m_pCoClass;	//��� - ��Class ��� ������������� ��������� � COM'e
	friend class COM::AbstractDocument;

	CProjectDoc();				// protected constructor used by dynamic creation
	override  ~CProjectDoc();	
	DECLARE_DYNCREATE(CProjectDoc)

	//��������������� ������ � ������� ����� ���������
	struct CFileSourceState{
		struct CViewPos{
			long dwPos;
			long dwFVL;
			long dwFCV;
			CViewPos():dwPos(0),dwFVL(0),dwFCV(0){}
		};
		typedef vector<long> bookmark_vector;
		typedef IDebugManageLink::vector_breakpoints breakpoint_vector;
		
		breakpoint_vector m_Breakpoints; //������ ���� ����� ��������
		bookmark_vector m_Bookmarks; //������ ���� ����� ��������
		CViewPos		m_Pos;
		__time64_t		m_TimeBuild;	//����� ����������� ����� � ������ ��������� ����������
		CFileSourceState():m_TimeBuild(0)
		{}
	};

	static	long	g_nID;			//����������� ������� ����������, ����� ��� ��������� ����������� ����� �������
	mutable it_file	m_itFile;		//�������� �������� ������(���������)	
	string_std		m_strNotes;
	string_std		m_strParams;
	string_std		m_strSwitches;
	string_std		m_strTargetLanguage;
	string_std		m_strSourceLanguage;
	string_std		m_strParamsBrowseDir; //������� �� ��������� ��� ������� ����������. ������������ ������ ��� ������ ������� ParamsDlg
	string_std		m_strWorkDir;	//������� ����������, ��� ���������� ������
	string_std		m_strPath;		//������ ����, ���� ������ m_strWorkDir. ���������� GetPathName; �������, � ��������, ��-�� ������ � ifaceProjectData 
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
	string_std		m_strOutputDirectory; //����������, ���� ����� ������������ ��� �����. ���� ����� ��� ������������� 
	string_std		m_strIntermediateDirectory; //����������, ���� ����� ������������ ��������������� �����. ���� ����� ��� ������������� 

	bool			m_bShowParamsDialog; //���������� �� ����� �������� ������ ����������
	static int		m_nParsingReportType;
	static bool		m_bParsingReportToFile;
	static bool		m_bParsingStopAtMessage;
	
	long			m_nOpState;
	mutable CLastAccessedDoc m_LastAccessedDoc;
	typedef map<string_smart,CFileSourceState> filedata_map;
	filedata_map m_mapFileData;

	//���������� ������� ���������, ���� ��������� ��������� � ���� �����������
	void RaiseChangeEvent(evdChangeProjectInfo & Info);
	//���������� ������� ��������
	void RaiseOpenEvent(evdOpenProjectInfo & Info);
	//���������� ������� ��������
	void RaiseCloseEvent(evdCloseProjectInfo & Info);
	//���������� �������� � ������ ������, �������� � ������, �� ��������� ����
	cit_file FileFind(cstr  szFileName) const;
	//��������� �������� ���������� �����, ���� �� ������
	bool FileClose(cstr szFileName);
	//�������� ��������� ������������� �������� ���������� �����, ���� �� ������
	bool FileSaveModified(cstr szFileName);
	//��������� ���� � ������
	bool FileAdd(cstr szFileName);
	//������� ���� �� �������
	bool FileRemove(cstr szFileName);
	//����������� ������ (������ ��� �������� ����� ���� ���������)
	void Activate(bool bForceActiation=false);
	//����������� �������
	void Deactivate();
	//�������-�������. ���������� ��������� � ��������� ��������
	IDocMngrLink* GetDocManagerInterface() const;
	//�������-�������. ���������� ��������� � Builder
	IBuilderLink * GetBuilderInterface() const;
	//�������-�������. ���������� ��������� � Runner
	IRunnerLink* GetRunnerInterface() const;
	//�������-�������. ���������� ��������� � Source-���������, ��� ���� �� ������ ������� � ��� ������
	ISourceDocLink* GetSourceInterface(cstr szFilePath) const;	
	//���������� � ���������� ���������� ������������� ��������� �����-���������
	bool RestoreSourceFileState(cstr szFilePath);
	//��������������� ������������� ��������� �����-���������
	bool StoreSourceFileState(cstr szFilePath);
	//��������� ��������� ���������� �������. ������ ������� �� 
	//������������ ������� ����� � ����������� .dsk. ���������� ������ 
	//��� �������� �������
	void LoadDSK();
	//��������� ��������� �������� ���������� �������. ���������� ������ 
	//��� �������� ������� + ����� Save()
	void SaveDSK();
	//���������, ����� �� Exe-���� � �������. �����: IDYES-�����, IDNO - �� �����, IDCANCEL - ������
	int IsNeedCompile(bool bForce=FALSE);
	//�������� ������� ��� �������. ���� ������ ������ ��, ��������� Runner
	void BuildWaitToRun(bool Succesful);
	//�������� ������� ��� �������. ���� ������ ������ ��, ��������� ���������� � ���� ������
	void BuildWaitToAddToKnowledgeBase(bool Succesful);
	//�������� ������� ��� �������. ���� ������ ������ ��, ��������� Runner
	void AddToKnowledgeBaseInternal();
	//���������� ������� ������� ������
	void RunInternal();
	//�������� ������� ��� �������. ���� ������ ������ ��, ��������� Runner
	void BuildWaitToDebug(bool Succesful);
	//���������� ������� ������� ������
	void DebugInternal();
	//��������� � ���������� ��������� �������� ������� ������������ ���� ����� ��������
	void CheckBreakpointsUsed(cstr szFilePath);
	//������ ����� ����� �������� ����� ������ �����������
	bool PreBuild();
	
	//����������� ���������� �������
	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &);//�������� ���������� ��������� �������
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &);//��������� ��������� ��������� �������
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
