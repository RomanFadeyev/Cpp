#pragma once
#include <afxmt.h>
#include <setjmp.h>

#include <BaseUtils.h>
#include "CObjectEx.h" //���� �� ���������
#include <GrmExecExports.h>
#include <GrmKrnlExports.h>
#include "Names.h"

using namespace grmexec_exports;
using namespace base_utils;

class CRunner : public CCObjectEx, public IRunnerLink
{
public:
	class CDebugContext: public CWnd{
	//���� ����� ��� ������������� ���������. ���� ��������� � �������� ������
	//� ��� ��������� ������ �������� � ������ ������
	private:
		struct parse_stack_item{
			string_smart	m_strFileName;
			size_t			m_nLine;
			size_t			m_nCol;
			string_smart	m_strRuleName;

			parse_stack_item(const grmkrnl_exports::parse_info_packet & packet):
				m_strFileName(packet.szFileName),m_nLine(packet.dwLine),m_nCol(packet.dwPos),m_strRuleName(packet.szCurHubName){
			}
		};

		IOutputLink *	m_pScreenLink;	//��� �������� �������
		CRunner	   *	m_pRunnerLink;
		bool			m_bCodeLoaded;
		bool			m_bTerminate;
		string_smart	m_strModuleName;		
		CEvent			m_SyncEvent;
		DWORD			m_nTimerID;		//������� ����������� 
		DWORD			m_nTimerID2;	//���������� ������
		string_smart	m_strOldCurDir;	//������ ������� ����������
		bool			m_bStepIn;				//��������� StepIn
		size_t			m_nStepOver;			//��������� StepOver. �������� ScopeInfo. ������ ���� unsigned
		step_info		m_CurProcessedInfo;	//���������� � ��������� �������������� �������
		
		//����� Parse-�����������
		bool			m_bStartParseOutput;			
		file_std		m_ParseFile;
		cstr			m_szPrevMessage;
		cstr			m_szPrevText;
		typedef list<parse_stack_item> parse_list;
		parse_list		m_ParseStack;
		string_smart	m_strTextBehind;
		string_smart	m_strTextAhead;
		ref_cnt_ptr<grmexec_exports::IGrmExec> m_ExecIface;

		//�������� �������
		void Stream (cstr szText);		
		void OnStep (const step_info & info);
		void OnParse(grmkrnl_exports::parse_info_packet & );
		
		void FillWatches();
		void FillParseOutput(const grmkrnl_exports::parse_info_packet & info);
		
		IOutputLink* GetScreenLink()	{return i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputScreen);}
		IOutputLink* GetOutputLink()	{return i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputBuild);}

		//---------------------------------------------------------------
		
		dll_function<pf_GetGrmExecInterface>	pLibGetGrmExecInterface;
		
		afx_msg LRESULT msgOnStep(WPARAM, LPARAM);			
		afx_msg LRESULT msgOnOutputPrint(WPARAM, LPARAM);	//����� � ��������� ��������� ������
		afx_msg	void msgOnSelfDestroy();
		afx_msg	void msgOnSelfFree();
		afx_msg void OnTimer(UINT nIDEvent);
		DECLARE_MESSAGE_MAP()
	public:
		jmp_buf			m_JmpMark;	/* Address for long jump to jump to */		
				
		CDebugContext(CRunner* pRunnerLink, cstr DebugModule/*��� ��������*/);
		~CDebugContext();

		bool IsReady();
		ERunnerExitCodes Run(cstr szParams,int & iExitCode);
		bool	IsBreakpointUsed(cstr szModuleName, size_t nLine);
		
		void WaitForEvent();	//�������� �������������
		void SetEvent();		//
		void ResetEvent();		//		
		bool IsWaiting();		//
		void Terminate(bool bImmediately=false);//��������� ����, ������� ����� �������������� � OnStep 
		void SelfFree();		//����������� ������ ����. ����������� ������������ � ����������� ������
		
		void StepIn()				{m_bStepIn=true;SetEvent();}
		void StepOver()				{m_nStepOver=m_CurProcessedInfo.m_nScopeLevel;	SetEvent();}
	};
	friend class CDebugContext;
private:
	CDebugContext  * m_pDebugContext; //��������� ����������� ��� ������ �������
	IProjectDocLink * m_pProjectLink;
	static CCriticalSection m_CritSect;
	HANDLE  m_hCurrentRunThread; //���������� �����, ������� ��������� �������. �� ���� ����� ������ � ���������
	HANDLE	m_hCurrentRunProcess;


	//���������� ����� DLL
	void RunInternal();	
	//���������� ��������� EXE-������
	static DWORD WINAPI RunExternalThread(LPVOID);	
	static DWORD WINAPI RunInternalThread(LPVOID pData);
protected:
	//������ IRunnerLink
	override HANDLE IRunnerLink::Run(IProjectDocLink	* pProjectLink);	//���������� 
	override void IRunnerLink::Debug(IProjectDocLink	* pProjectLink,bool bInitiallySuspended = true);	//�������
	override void IRunnerLink::Pause();									//�����
	override void IRunnerLink::Resume();								//�����������
	override void IRunnerLink::ResumeStepOver();						//����������� ���������� �� ���� �������
	override void IRunnerLink::ResumeStepIn();							//
	override void IRunnerLink::Stop();									//���������
	override bool IRunnerLink::IsBusy();								//����������� �� � ������ ������ 
	override bool IRunnerLink::IsBusy(EActivityType & ActivityType, IProjectDocLink *& pActiveProjectLink);
	override bool IRunnerLink::IsPaused();
	override bool IRunnerLink::IsBreakpointUsed(cstr szModuleName,size_t nLine);	//�������� �� ����������������� ����� ��������
	
	//�������� �������� ����
	override void OnCloseQueryAppMainWnd(bool & CanClose);
	//�������� ����������
	override void OnStopApp();
public:
	CRunner();
	virtual ~CRunner();
};
