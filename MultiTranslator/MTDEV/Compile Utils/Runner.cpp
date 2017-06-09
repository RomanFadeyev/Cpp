/********************************************************************
	created:	2002/08/16
	created:	16:8:2002   10:18
	filename: 	$(Projects)\sources\mtdev\compile utils\runner.cpp
	file path:	$(Projects)\sources\mtdev\compile utils
	file base:	runner
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Implementation of executer 
*********************************************************************/
#include "stdafx.h"
#include <GUIService.h>

#include "Resource.h"
#include "runner.h"
#include <ExitCodes.h>	//Здесь коды возврата grmexec.exe
#include <GrmExecExports.h>	//Здесь коды возврата grmexec.exe
#include <ModuleNames.h>

using namespace i_objects;
using namespace module_names;
using namespace exit_codes;
using namespace grmexec_exports;
using namespace grmkrnl_exports;

//Создание переходника для объекта
static i_objects::object_thunk<CRunner> Runner(__MSG_PREFIX__);

//////////////////////////////////////////////////////////////////////
// CRunner::CDebugContext class
//////////////////////////////////////////////////////////////////////
#define WAIT_DEBUG_THREAD_TIMEOUT	5000 //5 секунд ожидания  для неотвечающего потока отладчика

#define WM_DEBUG_STEP WM_USER+100
#define WM_SELF_DESTROY WM_DEBUG_STEP+1
#define WM_SELF_FREE WM_SELF_DESTROY+1
#define WM_OUTPUT_PRINT WM_SELF_FREE+1


CCriticalSection CRunner::m_CritSect;

typedef CRunner::CDebugContext CRDC;
BEGIN_MESSAGE_MAP(CRDC, CWnd)
	ON_MESSAGE(WM_DEBUG_STEP,msgOnStep)
	ON_MESSAGE(WM_OUTPUT_PRINT,msgOnOutputPrint)	
	ON_MESSAGE_VOID(WM_SELF_DESTROY,msgOnSelfDestroy)
	ON_MESSAGE_VOID(WM_SELF_FREE,msgOnSelfFree)
	ON_WM_TIMER()
END_MESSAGE_MAP()
//------------------------------------------------------------------------
CRunner::CDebugContext::CDebugContext(CRunner* pRunnerLink, cstr DebugModule/*что дебагить*/):
					m_pRunnerLink(pRunnerLink),
					m_bTerminate(false),
					m_SyncEvent(NULL,true),
					m_nTimerID(0),
					m_nTimerID2(0),
					m_bStepIn(false),
					m_nStepOver(-1),
					m_bStartParseOutput(false),
					m_szPrevMessage(NULL),
					m_szPrevText(NULL),
					pLibGetGrmExecInterface(szDebuggerName,szf_GetGrmExecInterface)
{
	_ASSERTE(AfxGetThread());
	
	if (!__super::CreateEx(0,AfxRegisterWndClass(0),NULL, WS_POPUP, 0,0,0,0, 0,NULL))
		_ASSERTE(false);
	
	m_pScreenLink = GetScreenLink();

	if (!GetModuleHandle(szDebuggerName)){
		guiMsgAttention(0,Format(string_res(IDS_CANT_FIND_FILE_s, HRESOURCES).c_str(),szDebuggerName).c_str());
		return;
	}

	//Обратные функции. Еще одна обратная функция - OnStep - назначается при 
	//запуске модуля
	m_bCodeLoaded=true; //Это важно, так как IsReady его опрашивает, иначе получится замкнутый цикл
	if (IsReady()){
		m_ExecIface.reset((*pLibGetGrmExecInterface)());
		m_ExecIface->SetExternalMessager(stream_functor(this,Stream));
		m_ExecIface->SetParseCallBck(parse_info_functor(this,OnParse));

		if (m_pScreenLink)
			m_pScreenLink->StartSession();	 

		//Запоминаем старую рабочую директорию и устанавливаем новую
		m_strOldCurDir = get_current_dir();
		SetCurrentDirectory(extract_filedir(DebugModule).c_str());

		//загрузка кода
		m_bCodeLoaded = (m_ExecIface->Load(DebugModule)==recOK);
		
		SetEvent(); //изначально готовы	
	}
}
//------------------------------------------------------------------------
CRunner::CDebugContext::~CDebugContext(){
	SetCurrentDirectory(m_strOldCurDir.c_str());
	m_pScreenLink = GetScreenLink();
	if (m_pScreenLink)
		m_pScreenLink->StopSession();

	for	(i_objects::server::template_object_it<IWatchListLink> it=i_objects::GetServer()->GetObjectIterator();!it.end();it.next())
		(*it)->StopSession();
		
	SendMessage(WM_SELF_DESTROY,0, 0);
	SetEvent();	//на всякий случай от зависания потоков

	m_ExecIface.reset();				//только в такой последовательности!
	pLibGetGrmExecInterface.detach();	//
}
//------------------------------------------------------------------------
bool CRunner::CDebugContext::IsReady()
{
	return  (m_bCodeLoaded) && (m_pScreenLink)&& (m_pRunnerLink) && 
			(pLibGetGrmExecInterface);
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::WaitForEvent(){
	
	_ASSERTE(!AfxGetThread());
	while (WaitForSingleObject(m_SyncEvent,100)==WAIT_TIMEOUT) {
		if (m_bTerminate)
			longjmp(m_JmpMark,-1);
	}
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::SetEvent(){
	m_SyncEvent.SetEvent();
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::ResetEvent(){
	m_SyncEvent.ResetEvent();
}
//------------------------------------------------------------------------
bool CRunner::CDebugContext::IsWaiting(){
	return WaitForSingleObject(m_SyncEvent,0)==WAIT_TIMEOUT;
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::Terminate(bool bImmediately/*=false*/)
{
	InterlockedExchange((LONG*)&m_bTerminate,true);
	
	//Эта функция не можут вызываться потоком
	ASSERT(m_pRunnerLink->m_hCurrentRunThread!=GetCurrentThread());
	
	////Даем возможность потоку самому завершиться - DEADLOCK!!!!
	//WaitForSingleObject(m_pRunnerLink->m_hCurrentRunThread,1000);
	
	CRunner	* pRunner = i_objects::GetInterface<CRunner>();
	//Если выполнение до сих пор не кончилось
	if (pRunner && pRunner->IsPaused())
	{
		if (!m_nTimerID) 
			m_nTimerID = SetTimer((UINT)-1, WAIT_DEBUG_THREAD_TIMEOUT, 0);

		IOutputLink * pOutputLink = GetOutputLink();
		if (pOutputLink)
			pOutputLink->Print("\n");
			
		//Уничтожаем без ожиданий
		if (bImmediately){
			SendMessage(WM_TIMER,m_nTimerID,0);
		}
		else{	//дополнительный таймер ожидания
			if (!m_nTimerID2) 
				m_nTimerID2 = SetTimer((UINT)-2, 1000, 0);
		}
	}
}
//------------------------------------------------------------------------
//уничтожение самого себя. уничтожение производится в критической секции
void CRunner::CDebugContext::SelfFree()
{	
	if (!this) return;
	if ((m_pRunnerLink) && (m_pRunnerLink->m_pDebugContext)){
		_ASSERTE(m_pRunnerLink->m_pDebugContext==this);
		m_pRunnerLink->m_pDebugContext=NULL;
	}

	_ASSERTE(m_hWnd);
	SendMessage(WM_SELF_FREE,0,0);
}
//------------------------------------------------------------------------
ERunnerExitCodes CRunner::CDebugContext::Run(cstr szParams,int & iExitCode){
	_ASSERTE(szParams);

	for	(i_objects::server::template_object_it<IWatchListLink> it=i_objects::GetServer()->GetObjectIterator();!it.end();it.next())
		(*it)->StartSession();
	
	return m_ExecIface->Debug(szParams,iExitCode,step_functor (this,OnStep));
}
//------------------------------------------------------------------------
bool CRunner::CDebugContext::IsBreakpointUsed(cstr szModuleName, size_t nLine){
	_ASSERTE(szModuleName);
	return m_ExecIface->CheckBreakPointOnUsed(szModuleName,nLine);
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::Stream (cstr szText){
	LOG_PROTECT_CODE_BEGIN
	m_pScreenLink->Print(str_translate_eoln(cstr_safe(szText)).c_str());
	LOG_PROTECT_CODE_END(MSG_EI_LocateObj("Stream","<CRunner::CDebugContext>"));
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::OnStep (const step_info & info){
	ASSERT_RETURN_VOID(m_pRunnerLink);
	
	LOG_PROTECT_CODE_BEGIN

	//Пропускаем строки с номером -1 (таких не было) и череду подряд идущих команд
	if ((info.m_bForcedStop) ||
		(info.m_nLine!=size_t(srclnNoNumber)) && (info.m_nLine!=size_t(srclnSameAsPrevious)))
		SendMessage(WM_DEBUG_STEP,(WPARAM)&info,0);
	WaitForEvent();
	LOG_PROTECT_CODE_END(MSG_EI_LocateObj("OnStep","<CRunner::CDebugContext>"));
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::OnParse(grmkrnl_exports::parse_info_packet & info){
	FillParseOutput(info);
	if (m_pRunnerLink->m_pProjectLink->GetParseStopAtMessage()){
		FillWatches();
		ResetEvent();
		WaitForEvent();
	}
}
//------------------------------------------------------------------------
LRESULT CRunner::CDebugContext::msgOnStep(WPARAM wParam, LPARAM lParam)
{
	m_CurProcessedInfo = * ((step_info*)wParam);
	if (!*m_CurProcessedInfo.m_szSource) return 0;

	LOG_PROTECT_CODE_BEGIN
	//Ищем, нет ли в это позиции точки останова
	BREAKPOINT bp(NULL,"");

	if ((m_bStepIn) || 
		(m_CurProcessedInfo.m_bForcedStop) || //Принудительная остановка
		((m_nStepOver!=-1)&&(m_CurProcessedInfo.m_nScopeLevel<=m_nStepOver)) ||  //Не помню что это
		(m_pRunnerLink->m_pProjectLink->IsBreakpointExists(m_CurProcessedInfo.m_szSource,m_CurProcessedInfo.m_nLine,bp))) //Есть точка останова
	{	
		//точку нашли, теперь пытаемся найти, к кому ее пристроить
		AfxGetApp()->OpenDocumentFile(m_CurProcessedInfo.m_szSource); //активизация документа

		IDebugManageLink * pDMLink = m_pRunnerLink->m_pProjectLink->GetDebugManageLink(m_CurProcessedInfo.m_szSource);
		if (pDMLink)
			pDMLink->SetCommandMarker(m_CurProcessedInfo.m_nLine);
		
		FillWatches();		
		
		m_nStepOver = -1; //Если мы остановились, этот параметр в любом случае не важен
		ResetEvent();
	}
	LOG_PROTECT_CODE_END(MSG_EI_LocateObj("OnStep","<CRunner::CDebugContext>"));	
	m_bStepIn=false; //Сбрасываем всегда, т.к. эта переменная действительна только для одного шага
	return 0;	
}
//------------------------------------------------------------------------
LRESULT CRunner::CDebugContext::msgOnOutputPrint(WPARAM wParam, LPARAM lParam)
{
	IOutputLink * pOutputLink = GetOutputLink();
	if (pOutputLink)
		pOutputLink->Print((cstr)wParam);
	return 0;
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::msgOnSelfDestroy(){
	DestroyWindow();
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::msgOnSelfFree(){
	LOG_PROTECT_CODE_BEGIN
	delete this;
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==m_nTimerID)	//
	{
		//Уничтожаем таймер
		try{KillTimer(nIDEvent);} catch (...){}
		try{KillTimer(m_nTimerID2);} catch (...){}

		CRunner	* pRunner = i_objects::GetInterface<CRunner>();
		//Если выполнение до сих пор не кончилось
		if (pRunner && (pRunner->IsPaused())) 
		{
			IOutputLink * pOutputLink = GetOutputLink();

			if (pOutputLink)
				pOutputLink->Print("\ndebug process not responding. Terminating... ");

			pRunner->m_CritSect.Lock();

			if (!TerminateThread(pRunner->m_hCurrentRunThread,-1))
				TRACE("CRunner::CDebugContext::OnTimer: Can't stop debugging thread");
			_ASSERT(_CrtCheckMemory());

			//Подчищаем возможные последствия
			if (pRunner->m_hCurrentRunThread){
				CloseHandle(pRunner->m_hCurrentRunThread);
				pRunner->m_hCurrentRunThread=NULL;
			}
		
			if (pRunner->m_pDebugContext)
				pRunner->m_pDebugContext->SelfFree();
		
			//Дополнительная подстраховка: библиотека отладчика должна быть выгружена
			_ASSERTE(!GetModuleHandle(szDebuggerName));
			while (GetModuleHandle(szDebuggerName))
				FreeLibrary(GetModuleHandle(szDebuggerName));
			
			pRunner->m_CritSect.Unlock();
			
			if (pOutputLink)
				pOutputLink->Print("done.\n");
		}
	}
	//
	else if (nIDEvent==m_nTimerID2){
		CRunner	* pRunner =	i_objects::GetInterface<CRunner>();

		if (pRunner && (pRunner->IsPaused())){
			
			IOutputLink * pOutputLink = GetOutputLink();
			if (pOutputLink)
				pOutputLink->Print("waiting debug process...\n");
		}
	}
	else
		__super::OnTimer(nIDEvent);
}
//------------------------------------------------------------------------
static void FillWatchItem(WATCHITEM & WatchItem,grmexec_exports::object_info& WatchInfo)
{
	WatchItem.strName=WatchInfo.GetName();
	WatchItem.strContext = WatchInfo.GetContext();
	WatchItem.nKind=(WATCHITEM::kind)WatchInfo.GetKind(); //Довольно хлипкое место.
	WatchItem.value=WatchInfo.GetValue();

	if (WatchItem.nKind!=WATCHITEM::wikStruct)
		WatchItem.strType=WatchInfo.GetValue().get_type_string();
	else
		WatchItem.strType="struct";

	if (WatchItem.nKind == WATCHITEM::wikFunction){
		WatchItem.nLine=WatchItem.value.as_int();
		WatchItem.nCol=-1;
	}
	
	for (size_t i=0;i<WatchInfo.ItemCount();i++){
		WatchItem.ChildItems.push_back(WATCHITEM());
		WATCHITEM & ChildItem = WatchItem.ChildItems.back();
		FillWatchItem(ChildItem,WatchInfo.GetItem(i));
	}
}
//------------------------------------------------------------------------
//#pragma optimize("",off)	
void CRunner::CDebugContext::FillWatches()
{
	grmexec_exports::get_oi_result res;
	grmexec_exports::object_info* pInfo;

	WATCHITEM::vector_items WatchItems;

	LOG_PROTECT_CODE_BEGIN
	//Получаем информацию о локальных переменных
	string_list strObjects(m_ExecIface->GetObjectNames(grmexec_exports::goifLocalVars));
	for (size_t i=0;i<strObjects.items().count();i++){
		pInfo = m_ExecIface->GetObjectInfo(strObjects.items()[i],res);
		if (!pInfo){
			_RPT1(_CRT_ERROR,"Can't retrieve information for variable %s while filling LocalVars",strObjects.items()[i]);
		}
		else{
			WatchItems.push_back(WATCHITEM());
			WATCHITEM & WatchItem=WatchItems.back();
			FillWatchItem(WatchItem,*pInfo);
			m_ExecIface->ReleaseObjectInfo(pInfo);
		}
	}
	IWatchListLink * pWatchList = i_objects::GetInterface<IWatchListLink>(IWatchListLink::idWatchListLocal);
	_ASSERTE(pWatchList);
	pWatchList->SetItems(WatchItems);

	//Получаем информацию о авто-переменных
	WatchItems.clear();
	strObjects = m_ExecIface->GetObjectNames(grmexec_exports::goifAutoVars);
	for (size_t i=0;i<strObjects.items().count();i++){
		pInfo = m_ExecIface->GetObjectInfo(strObjects.items()[i],res);
		if (!pInfo){
			_RPT1(_CRT_ERROR,"Can't retrieve information for variable %s while filling LocalVars",strObjects.items()[i]);
		}
		else{
			WatchItems.push_back(WATCHITEM());
			WATCHITEM & WatchItem=WatchItems.back();
			FillWatchItem(WatchItem,*pInfo);
			
			m_ExecIface->ReleaseObjectInfo(pInfo);
		}
	}
	pWatchList = i_objects::GetInterface<IWatchListLink>(IWatchListLink::idWatchListAuto);
	_ASSERTE(pWatchList);
	pWatchList->SetItems(WatchItems);

	IWatchListLink * pWatchCallStack = i_objects::GetInterface<IWatchListLink>(IWatchListLink::idWatchCallStack);
	_ASSERTE(pWatchCallStack);
	WatchItems.clear();
	//Заподняем CallStack
	strObjects = m_ExecIface->GetObjectNames(grmexec_exports::goifStack);
	for (size_t i=0;i<strObjects.items().count();i++){
		pInfo = m_ExecIface->GetObjectInfo(strObjects.items()[i],res);
		if (!pInfo){
			_RPT1(_CRT_ERROR,"Can't retrieve information for function %s while filling CallStack",strObjects.items()[i]);
		}
		else{
			WatchItems.push_back(WATCHITEM());
			WATCHITEM & WatchItem=WatchItems.back();
			FillWatchItem(WatchItem,*pInfo);			
			if (WatchItem.strContext.isempty())	//временная мера!
				WatchItem.strContext = m_pRunnerLink->m_pProjectLink->GetFileFirst();
			m_ExecIface->ReleaseObjectInfo(pInfo);
		}
	}
	pWatchCallStack->SetItems(WatchItems);
	
	//Заполняем ParseStack
	IWatchListLink * pWatchParseStack = i_objects::GetInterface<IWatchListLink>(IWatchListLink::idWatchParseStack);
	_ASSERTE(pWatchParseStack);
	WatchItems.clear();

	for (parse_list::iterator i=m_ParseStack.begin(); i!=m_ParseStack.end();++i){
		WatchItems.push_back(WATCHITEM());
		WATCHITEM & WatchItem=WatchItems.back();
		WatchItem.nKind = WATCHITEM::wikFunction;
		WatchItem.strName=(*i).m_strRuleName.c_str();
		WatchItem.strContext=(*i).m_strFileName.c_str();
		WatchItem.nLine = (*i).m_nLine;
		WatchItem.nCol = (*i).m_nCol;
	}
	pWatchParseStack->SetItems(WatchItems);

	//заполняем ParsePanel
	IWatchParsePanel * pParsePanel = i_objects::GetInterface<IWatchParsePanel>();
	_ASSERTE(pParsePanel);
	pParsePanel->SetTextAhead(str_translate_eoln(m_strTextAhead).c_str());
	pParsePanel->SetTextBehind(str_translate_eoln(m_strTextBehind).c_str());

	
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CRunner::CDebugContext::FillParseOutput(const grmkrnl_exports::parse_info_packet & info)
{
	typedef IProjectDocLink::parse_report_type prt;
	prt Prt = m_pRunnerLink->m_pProjectLink->GetParseReportType();
	if ((Prt==IProjectDocLink::prNone) && (!m_pRunnerLink->m_pProjectLink->GetParseStopAtMessage()))
		return;
		
	bool bShortMode		= (Prt==IProjectDocLink::prShort) || (Prt==IProjectDocLink::prShortToFile);
	bool bReportToFile	= (Prt==IProjectDocLink::prShortToFile) || (Prt==IProjectDocLink::prFullToFile);
	bool bPrint			= false;
	IOutputLink * pParseOutput = (bReportToFile && (m_bStartParseOutput)) ? NULL : i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputParse);
	
	if (!m_bStartParseOutput)
		if (pParseOutput)
			pParseOutput->StartSession();
	m_bStartParseOutput=true;

	LOG_PROTECT_CODE_BEGIN
	m_strTextAhead=info.szTextAhead;
	m_strTextBehind=info.szTextBehind;

	//стек парсинга
	if (info.dwCmd==parse_info_packet::nfoENTER)
		m_ParseStack.push_back(info);
	else if ((info.dwCmd==parse_info_packet::nfoEXIT_TRUE) || (info.dwCmd==parse_info_packet::nfoROLLBACK))
		m_ParseStack.pop_back();

	//собираем строку отчета
	string_smart strMsg;	
	if (bShortMode)	// Сокращенный вариант
	{
		if ((info.dwCmd!=parse_info_packet::nfoNEXT_VARIANT) && 
			(info.dwCmd!=parse_info_packet::nfoENTER) && 
			(info.dwCmd!=parse_info_packet::nfoEXIT_TRUE) && 
			(info.dwCmd!=parse_info_packet::nfoROLLBACK))
			bPrint = ((info.dwCmd==parse_info_packet::nfoMSG) ||(m_szPrevMessage!=info.szMessage) || (m_szPrevText!=info.szText));
	}
	else  //Полный вариант. В нем без разбора печатаем все
		bPrint = true;
	
	//Составление строки
	if (bPrint)
		strMsg.cat(strnchar(' ',info.dwRecursionLevel).c_str()).cat(
				Format("%s \"%s\" (%s) <%d,%d>",
						info.szMessage,
						str_translate_control(info.szText).c_str(),
						info.szCurHubName,
						info.dwLine,
						info.dwPos).c_str());
	
	

	
	if (!strMsg.isempty())
		//Запись в файл
		if (bReportToFile) {
			if (!m_ParseFile.isopen())
			{	
				string_smart s = m_pRunnerLink->m_pProjectLink->GetParseLogPath();
				if (m_ParseFile.open(s.c_str(),"wt"))
				//Это что-то вроде оптимизации
				if (!pParseOutput)
					pParseOutput=i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputParse);
				if (pParseOutput)
					pParseOutput->Print(
						Format(string_res(IDS_PARSE_REPORT_TO_FILE_s,HRESOURCES).c_str(),s.frames_wrap_on('\'','\'').c_str())
						.cat("\r\n").c_str());
			}
			if (m_ParseFile.isopen())
				m_ParseFile<<strMsg<<"\n";
		}
		//Вывод на экран
		else if (pParseOutput) 
			pParseOutput->Print(strMsg.cat('\n').c_str());

	m_szPrevMessage=info.szMessage;
	m_szPrevText=info.szText;
	LOG_PROTECT_CODE_END;	
}
//////////////////////////////////////////////////////////////////////
// CRunner class
//////////////////////////////////////////////////////////////////////

CRunner::CRunner():CCObjectEx(),
					m_hCurrentRunProcess(NULL),
					m_hCurrentRunThread(NULL),
					m_pDebugContext(NULL),
					m_pProjectLink(NULL)
{
	
}
//------------------------------------------------------------------------
CRunner::~CRunner()
{
	if (IsBusy())
		Stop();
}
//------------------------------------------------------------------------
HANDLE CRunner::Run(IProjectDocLink	* pProjectLink)
{
	ASSERT_RETURN_NULL(!IsBusy());
	
	m_pProjectLink=pProjectLink;
	m_hCurrentRunProcess=NULL;

	DWORD ThreadID;
	m_hCurrentRunThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RunExternalThread,this,0,&ThreadID);
	while ((IsBusy()) && (m_hCurrentRunProcess==0))
		guiProcessMessages();
	
	return m_hCurrentRunProcess;
}
//------------------------------------------------------------------------
void CRunner::Debug(IProjectDocLink	* pProjectLink,bool bInitiallySuspended/*true*/)
{
	ASSERT_RETURN_VOID(!IsBusy());
	_ASSERTE(m_pDebugContext==NULL);

	//Дополнительная подстраховка: библиотека отладчика должна быть выгружена
	_ASSERTE(!GetModuleHandle(szDebuggerName));
	while (GetModuleHandle(szDebuggerName))
		FreeLibrary(GetModuleHandle(szDebuggerName));
	
	LOG_PROTECT_CODE_BEGIN
	m_pDebugContext = new CDebugContext(this,pProjectLink->GetExePath().c_str());

	_ASSERTE(pProjectLink->GetExePath().c_str());
	_ASSERTE(pProjectLink->GetParams().c_str());
	
	//Убеждаемся в рабоспособности библиотеки
	if (!m_pDebugContext->IsReady())
	{
		delete m_pDebugContext; 
		m_pDebugContext=NULL;
		return;
	}

	m_pProjectLink=pProjectLink;
	m_hCurrentRunProcess=NULL;

	if (bInitiallySuspended)
		m_pDebugContext->ResetEvent();
		
	DWORD ThreadID;
	m_hCurrentRunThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RunInternalThread,this,0,&ThreadID);
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CRunner::Pause(){
	ASSERT_RETURN_VOID(!IsPaused());
	ASSERT_RETURN_VOID(m_pDebugContext);
	m_pDebugContext->ResetEvent();
}
//------------------------------------------------------------------------
void CRunner::Resume(){
	ASSERT_RETURN_VOID(IsPaused());
	ASSERT_RETURN_VOID(m_pDebugContext);
	m_pDebugContext->SetEvent();
}
//------------------------------------------------------------------------
void CRunner::ResumeStepOver()
{
	ASSERT_RETURN_VOID(IsPaused());
	ASSERT_RETURN_VOID(m_pDebugContext);
	m_pDebugContext->StepOver();
}
//------------------------------------------------------------------------
void CRunner::ResumeStepIn()
{
	ASSERT_RETURN_VOID(IsPaused());
	ASSERT_RETURN_VOID(m_pDebugContext);
	m_pDebugContext->StepIn();
}
//------------------------------------------------------------------------
void CRunner::Stop()
{
	IRunnerLink::EActivityType ActivityType;
	IProjectDocLink * pActiveProjectLink;
	ASSERT_RETURN_VOID(IsBusy(ActivityType,pActiveProjectLink));

	LOG_PROTECT_CODE_BEGIN	
	//Режим отладки
	if (ActivityType==atDebug) {
		m_pDebugContext->ResetEvent();		//ставим стопор для потока
		m_pDebugContext->Terminate(true);	//ставим флаг завершения

		_ASSERTE(m_CritSect.m_sect.LockCount<=0);
	}
	//Режим выполнения
	else{
		if (TerminateProcess(m_hCurrentRunProcess,exit_codes::recFORCED_TERMINATION))
			m_hCurrentRunProcess=NULL;
		else{
			TRACE("CRunner::Stop: Can't stop running process");
			//Что ж, будем останавливать поток
			if (!TerminateThread(m_hCurrentRunThread,-1))
				TRACE("CRunner::Stop: Can't stop running thread");
			CloseHandle(m_hCurrentRunThread);
			m_hCurrentRunThread=NULL;
		}
	}
	LOG_PROTECT_CODE_END(MSG_EI_LocateObj("CRunner::Stop","<CRunner>"));
}
//------------------------------------------------------------------------
bool CRunner::IsBusy() 
{
	
	if (m_hCurrentRunThread==NULL) return false;
	DWORD dwExitCode=0;
	GetExitCodeThread(m_hCurrentRunThread,&dwExitCode);
	return dwExitCode==STILL_ACTIVE;
}
//------------------------------------------------------------------------
bool CRunner::IsBusy(IRunnerLink::EActivityType & ActivityType, IProjectDocLink *& pActiveProjectLink) 
{
	m_CritSect.Lock();	//блокируем одновременный доступ к m_hCurrentRunThread
	
	bool ret_code = IsBusy();
	
	if (!ret_code){
		ActivityType=atNone;
		pActiveProjectLink=NULL;
	}
	else{
		_ASSERTE(m_pProjectLink);
		ActivityType=(m_pDebugContext) ? atDebug :atRun;
		pActiveProjectLink=m_pProjectLink;
	}

	m_CritSect.Unlock();
	return ret_code;
}
//------------------------------------------------------------------------
bool CRunner::IsPaused() 
{
	m_CritSect.Lock();	//блокируем одновременный доступ к m_hCurrentRunThread	
	bool ret_code = false;
	LOG_PROTECT_CODE_BEGIN
	
	IRunnerLink::EActivityType ActivityType;
	IProjectDocLink * pActiveProjectLink;
	ret_code = IsBusy(ActivityType,pActiveProjectLink);
	//Пауза может быть только у отладчика. Здесь собраны все мыслимые проверки
	ret_code = (ret_code) && (m_hCurrentRunThread!=NULL) && (ActivityType==atDebug) && (m_pDebugContext) && (m_pDebugContext->IsWaiting());

	LOG_PROTECT_CODE_END(MSG_EI_LocateObj("CRunner::IsPaused","<CRunner>"));		
	m_CritSect.Unlock();
	
	return ret_code;
}
//------------------------------------------------------------------------
bool CRunner::IsBreakpointUsed(cstr szModuleName,size_t nLine)
{
	_ASSERTE(IsBusy());
	_ASSERTE(m_pDebugContext);
	return m_pDebugContext->IsBreakpointUsed(szModuleName,nLine);
}
//------------------------------------------------------------------------
void CRunner::OnCloseQueryAppMainWnd(bool & CanClose)
{
	if (IsBusy()){ 
		if (guiMsgConfirm(0,Format(string_res(IDS_CLOSE_APP_DEBUG_IN_PROGRESS,HRESOURCES).c_str()).c_str())){
			Stop();
		}
		else
			CanClose=false;
	}
}
//------------------------------------------------------------------------
void CRunner::OnStopApp()
{
	m_CritSect.Lock();	//блокируем одновременный доступ к m_hCurrentRunThread	
	if (m_pDebugContext)
		m_pDebugContext->Terminate(true);	//если отладка все еще пытается остановить процесс, делаем это немедленно
											//больше ждать нечего
	m_CritSect.Unlock();
}
//------------------------------------------------------------------------
DWORD WINAPI CRunner::RunInternalThread(LPVOID pData)
{
	Init_Thread_EH();	//поддержка с++	исключений
	CRunner * pSelf = static_cast<CRunner *> (pData);
	_ASSERTE(pSelf);
	CDebugContext* pContext = pSelf->m_pDebugContext;
	_ASSERTE(pContext);
	
	int jmp_ret = setjmp(pContext->m_JmpMark);
	if (jmp_ret!=-1){
		IOutputLink * pOutputLink = i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputBuild);

		LOG_PROTECT_CODE_BEGIN
		//--------------------------------------------------------------
		// Поиск объекта вывода информации на экран
		if (!pOutputLink)
			throw exception(Format("Can't find object '%d' for complete operation.",IOutputLink::idOutputBuild).c_str());
		pOutputLink->StartSession();
		
		_ASSERTE(pSelf->m_pProjectLink->GetExePath().c_str());
		_ASSERTE(pSelf->m_pProjectLink->GetParams().c_str());

		int exit_code;
		pOutputLink->Print(Format("------ program started, path: %s ------\n",pSelf->m_pProjectLink->GetExePath().c_str()).c_str());

		pContext->WaitForEvent();
		ERunnerExitCodes ret_code = pContext->Run(pSelf->m_pProjectLink->GetParams().c_str(),exit_code);
		pOutputLink->Print(Format("\n debug process finished. process' exit code: %d, main returned: %d\n",ret_code,exit_code).c_str());
		_ASSERTE(_CrtCheckMemory());

		LOG_PROTECT_CODE_END
		
		//Оборачиваем в еще один try, мало ли, может указатели на интерфейсы уже не рабочие?
		LOG_PROTECT_CODE_BEGIN
		if (pOutputLink)
			pOutputLink->StopSession();
		LOG_PROTECT_CODE_END
	}
	else{
		pContext->SendMessage(WM_OUTPUT_PRINT,(WPARAM)"\n debug process has been interrupted.\n");
	}

	m_CritSect.Lock();	//Блокируем многопоточность
	
	//Последний try - завершение 
	LOG_PROTECT_CODE_BEGIN
		
	CloseHandle(pSelf->m_hCurrentRunThread);//Закрываем декскриптор на самого себя
	InterlockedExchange((LONG*)&pSelf->m_hCurrentRunThread,NULL);
	_ASSERTE(pContext==pSelf->m_pDebugContext);
	InterlockedExchange((LONG*)&pSelf->m_pDebugContext,NULL);	
	InterlockedExchange((LONG*)&pSelf->m_pProjectLink,NULL);	
	LOG_PROTECT_CODE_END

	m_CritSect.Unlock();	//Разблокируем многопоточность	
	pContext->SelfFree();	//Здесь есть SendMessage - поэтому во избежании DeadLock выносим за критическую секцию

	return 0;	
}
//------------------------------------------------------------------------
DWORD WINAPI CRunner::RunExternalThread(LPVOID pData)
{
	Init_Thread_EH();	//поддержка с++	исключений
	CRunner * pSelf = static_cast<CRunner *> (pData);

	IOutputLink * pOutputLink = i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputBuild);
	IOutputLink * pScreenLink = i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputScreen);

	LOG_PROTECT_CODE_BEGIN
	//--------------------------------------------------------------
	// Поиск объекта вывода информации на экран
	if (!pOutputLink)
		throw exception(Format("Can't find object '%d' for complete operation.",IOutputLink::idOutputBuild).c_str());

	if (!pScreenLink )
		throw exception(Format("Can't find object '%d' for complete operation.",(IOutputLink::idOutputScreen)).c_str());
	pOutputLink->StartSession();
	pScreenLink->StartSession();
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

	//CompilerName
	string_smart strCommandLine(	find_full_filepath(szExecuterName).frames_wrap_on('\"','\"').c_str(),
								" \"",pSelf ->m_pProjectLink->GetExePath().c_str(),"\" ",pSelf->m_pProjectLink->GetParams().c_str());
	string_smart strWorkDir=extract_filedir(pSelf ->m_pProjectLink->GetExePath().c_str());

	pSelf->m_hCurrentRunProcess=NULL;
	if (!CreateProcess(NULL,strCommandLine.buffer(),NULL,NULL,true,NORMAL_PRIORITY_CLASS,NULL,strWorkDir.c_str(),&si,&pi)){
		DWORD dwErr  = GetLastError();
		CloseHandle(hWrite);
		CloseHandle(hRead);
		//if (dwErr==ERROR_FILE_NOT_FOUND)
			guiMsgAttention(0,Format(string_res(IDS_CANT_EXECUTE_ss, HRESOURCES).c_str(),szExecuterName,string_err(dwErr).c_str()).c_str());
		throw exception(Format("Can't create process.  Cause: '%s'",string_err(dwErr).c_str()).c_str());
	}
	pSelf->m_hCurrentRunProcess=pi.hProcess;
	CloseHandle(hWrite);	

	pOutputLink->Print(Format("------ program started, path: %s ------\n",pSelf->m_pProjectLink->GetExePath().c_str()).c_str());
	
	//---------------------------------------------------------------
	//Читаем результаты
	char  szBuff[256];DWORD nRead;
	while (ReadFile(hRead,szBuff,255,&nRead,NULL)){
		szBuff[nRead]=0;	
		pScreenLink->Print(szBuff);
	}

	CloseHandle(hRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	LOG_PROTECT_CODE_END
	pSelf->m_hCurrentRunProcess=NULL;
	
	//Оборачиваем в еще один try, мало ли, может указатели на интерфейсы уже не рабочие?
	LOG_PROTECT_CODE_BEGIN

	if (pOutputLink)
		pOutputLink->StopSession();
	if (pScreenLink)
		pScreenLink->StopSession();

	LOG_PROTECT_CODE_END

	CloseHandle(pSelf->m_hCurrentRunThread);//Закрываем декскриптор на самого себя
	InterlockedExchange((LONG*)&pSelf->m_hCurrentRunThread,NULL);
	InterlockedExchange((LONG*)&pSelf->m_pProjectLink,NULL);
 
	return 0;	
}
//------------------------------------------------------------------------

