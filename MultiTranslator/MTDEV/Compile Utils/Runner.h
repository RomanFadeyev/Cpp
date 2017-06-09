#pragma once
#include <afxmt.h>
#include <setjmp.h>

#include <BaseUtils.h>
#include "CObjectEx.h" //Один из родителей
#include <GrmExecExports.h>
#include <GrmKrnlExports.h>
#include "Names.h"

using namespace grmexec_exports;
using namespace base_utils;

class CRunner : public CCObjectEx, public IRunnerLink
{
public:
	class CDebugContext: public CWnd{
	//окно нужно для синхронизации сообщений. Окно создается в основном потоке
	//а вот некоторые методы работают в другом потоке
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

		IOutputLink *	m_pScreenLink;	//для быстрого доступа
		CRunner	   *	m_pRunnerLink;
		bool			m_bCodeLoaded;
		bool			m_bTerminate;
		string_smart	m_strModuleName;		
		CEvent			m_SyncEvent;
		DWORD			m_nTimerID;		//таймеры отслежвания 
		DWORD			m_nTimerID2;	//завершения потока
		string_smart	m_strOldCurDir;	//старая текущая директория
		bool			m_bStepIn;				//обработка StepIn
		size_t			m_nStepOver;			//обработка StepOver. Хранится ScopeInfo. Должен быть unsigned
		step_info		m_CurProcessedInfo;	//Информация о последней обрабатываемой команде
		
		//Вывод Parse-результатов
		bool			m_bStartParseOutput;			
		file_std		m_ParseFile;
		cstr			m_szPrevMessage;
		cstr			m_szPrevText;
		typedef list<parse_stack_item> parse_list;
		parse_list		m_ParseStack;
		string_smart	m_strTextBehind;
		string_smart	m_strTextAhead;
		ref_cnt_ptr<grmexec_exports::IGrmExec> m_ExecIface;

		//Обратные функции
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
		afx_msg LRESULT msgOnOutputPrint(WPARAM, LPARAM);	//вывод в контексте основного потока
		afx_msg	void msgOnSelfDestroy();
		afx_msg	void msgOnSelfFree();
		afx_msg void OnTimer(UINT nIDEvent);
		DECLARE_MESSAGE_MAP()
	public:
		jmp_buf			m_JmpMark;	/* Address for long jump to jump to */		
				
		CDebugContext(CRunner* pRunnerLink, cstr DebugModule/*что дебагить*/);
		~CDebugContext();

		bool IsReady();
		ERunnerExitCodes Run(cstr szParams,int & iExitCode);
		bool	IsBreakpointUsed(cstr szModuleName, size_t nLine);
		
		void WaitForEvent();	//элементы синхронизации
		void SetEvent();		//
		void ResetEvent();		//		
		bool IsWaiting();		//
		void Terminate(bool bImmediately=false);//взводится флаг, который затем обрабатывается в OnStep 
		void SelfFree();		//уничтожение самого себя. уничтожение производится в критической секции
		
		void StepIn()				{m_bStepIn=true;SetEvent();}
		void StepOver()				{m_nStepOver=m_CurProcessedInfo.m_nScopeLevel;	SetEvent();}
	};
	friend class CDebugContext;
private:
	CDebugContext  * m_pDebugContext; //создается динамически при старте отладки
	IProjectDocLink * m_pProjectLink;
	static CCriticalSection m_CritSect;
	HANDLE  m_hCurrentRunThread; //Запущенный поток, который запускает процесс. По нему можно судить о занятости
	HANDLE	m_hCurrentRunProcess;


	//Выполнение через DLL
	void RunInternal();	
	//Выполнение отдельным EXE-файлом
	static DWORD WINAPI RunExternalThread(LPVOID);	
	static DWORD WINAPI RunInternalThread(LPVOID pData);
protected:
	//Методы IRunnerLink
	override HANDLE IRunnerLink::Run(IProjectDocLink	* pProjectLink);	//Выполнение 
	override void IRunnerLink::Debug(IProjectDocLink	* pProjectLink,bool bInitiallySuspended = true);	//Отладка
	override void IRunnerLink::Pause();									//Пауза
	override void IRunnerLink::Resume();								//ПРодолжение
	override void IRunnerLink::ResumeStepOver();						//Продолжение выполнения на одну строчку
	override void IRunnerLink::ResumeStepIn();							//
	override void IRunnerLink::Stop();									//Остановка
	override bool IRunnerLink::IsBusy();								//Выполняется ли в данный момент 
	override bool IRunnerLink::IsBusy(EActivityType & ActivityType, IProjectDocLink *& pActiveProjectLink);
	override bool IRunnerLink::IsPaused();
	override bool IRunnerLink::IsBreakpointUsed(cstr szModuleName,size_t nLine);	//Проверка на работоспособность точки останова
	
	//Закрытие главного окна
	override void OnCloseQueryAppMainWnd(bool & CanClose);
	//Закрытие приложения
	override void OnStopApp();
public:
	CRunner();
	virtual ~CRunner();
};
