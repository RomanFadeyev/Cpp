/********************************************************************
	created:	2002/05/04
	created:	4:5:2002   23:22
	filename: 	$(Projects)\sources\mtdev\builder.cpp
	file path:	$(Projects)\sources\mtdev

	file base:	builder
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include <ShlWapi.h>
#include <GUIService.h>
#include <GrmcExports.h>	//Здесь коды возврата grmexec.exe
#include <ModuleNames.h>

#include "Resource.h"
#include "Builder.h"

using namespace i_objects;
using namespace module_names;
using namespace grmc_exports;

//Создание переходника для объекта
static i_objects::object_thunk<CBuilder> Builder(__MSG_PREFIX__);

#define WM_SELF_DESTROY WM_USER+100
#define WM_SELF_FREE WM_SELF_DESTROY+1

typedef CBuilder::CBuildContext CBDC;
BEGIN_MESSAGE_MAP(CBDC, CWnd)
	ON_MESSAGE(WM_SELF_DESTROY,msgOnSelfDestroy)
	ON_MESSAGE(WM_SELF_FREE,msgOnSelfFree)
END_MESSAGE_MAP()
//------------------------------------------------------------------------
CBuilder::CBuildContext::CBuildContext(CBuilder* pBuilderLink):
					m_pBuilderLink(pBuilderLink)
{
	_ASSERTE(AfxGetThread());
	
	if (!__super::CreateEx(0,AfxRegisterWndClass(0),NULL, WS_POPUP, 0,0,0,0, 0,NULL))
		_ASSERTE(false);
}
//------------------------------------------------------------------------
CBuilder::CBuildContext::~CBuildContext(){
	SendMessage(WM_SELF_DESTROY,0, 0);
}
//------------------------------------------------------------------------
//уничтожение самого себя. уничтожение производится в критической секции
void CBuilder::CBuildContext::SelfFree(DWORD exit_code)
{	
	if (!this) return;
	if ((m_pBuilderLink) && (m_pBuilderLink->m_pBuildContext)){
		_ASSERTE(m_pBuilderLink->m_pBuildContext==this);
		InterlockedExchange((LONG*)&m_pBuilderLink->m_pBuildContext,NULL);	
	}

	_ASSERTE(m_hWnd);
	SendMessage(WM_SELF_FREE,exit_code,0);
}
//------------------------------------------------------------------------
LRESULT CBuilder::CBuildContext::msgOnSelfDestroy(WPARAM wParam, LPARAM lParam){
	return DestroyWindow();
}
//------------------------------------------------------------------------
LRESULT CBuilder::CBuildContext::msgOnSelfFree(WPARAM wParam, LPARAM lParam){
	//CallBack выполняется в родном потоке!
	if (m_pBuilderLink->m_pfCallBk.assigned())
		m_pBuilderLink->m_pfCallBk(wParam==cecOK);
	delete this;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/// CBuilder::IBuilderLink
//////////////////////////////////////////////////////////////////////////
void CBuilder::Build(IProjectDocLink * pProjectLink,functor_build pfCallBk/*=functor_build()*/)
{
	if (IsBusy()){
		ASSERT(false);
		return;
	}
	
	m_pProjectLink				= pProjectLink;
	m_pfCallBk		   			= pfCallBk;
	m_hCurrentRunProcess		= NULL;
	m_pBuildContext	= new CBuildContext(this);
	DWORD ThreadID;
	m_hCurrentBuildThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CompilerThread,this,0,&ThreadID);
}
//------------------------------------------------------------------------
void CBuilder::Stop()
{
	ASSERT_RETURN_VOID(IsBusy());
	if (TerminateProcess(m_hCurrentRunProcess,grmc_exports::cecFORCED_TERMINATION))
		m_hCurrentRunProcess=NULL;
	else{
		TRACE("CBuilder::Stop: Can't stop running process");
		//Что ж, будем останавливать поток
		if (!TerminateThread(m_hCurrentBuildThread,-1))
			TRACE("CBuilder::Stop: Can't stop running build thread");
		CloseHandle(m_hCurrentBuildThread);
		m_hCurrentBuildThread=NULL;
		if (m_pBuildContext)
			m_pBuildContext->SelfFree(grmc_exports::cecFORCED_TERMINATION);
	}
}
//------------------------------------------------------------------------
bool CBuilder::IsBusy() 
{
	if (m_hCurrentBuildThread==NULL) return false;
	//DWORD dwExitCode=0;

	//GetExitCodeThread(m_hCurrentBuildThread,&dwExitCode);
	//return dwExitCode==STILL_ACTIVE;
	return true;
}
//------------------------------------------------------------------------
void CBuilder::OnCloseQueryAppMainWnd(bool & CanClose)
{
	if (IsBusy()){ 
		if (guiMsgConfirm(0,Format(string_res(IDS_CLOSE_APP_BUILD_IN_PROGRESS,HRESOURCES).c_str()).c_str()))
			Stop();
		else
			CanClose=false;
	}
}
//------------------------------------------------------------------------
DWORD WINAPI CBuilder::CompilerThread(LPVOID pData)
{
	Init_Thread_EH();	//поддержка с++	исключений
	CBuilder * pSelf = static_cast <CBuilder *> (pData);

	IOutputLink * pBuildOutputLink = i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputBuild);
	DWORD dwExitCode = /*ECompilerCodes::*/cecINIT_FAILED; //Код завершения. t

	LOG_PROTECT_CODE_BEGIN
	//--------------------------------------------------------------
	// Поиск объекта вывода информации на экран
	if (!pBuildOutputLink) 
		throw exception(Format("Can't find object '%d' for complete operation.",IOutputLink::idOutputBuild).c_str());
	pBuildOutputLink->StartSession(true);
		
	//Составляем MAK-файл
	string_smart		strMAK; // "{\n" 
	string_smart	strMAKFileName(change_fileext(pSelf->m_pProjectLink->GetPath().c_str(),EXT_MAKE));	
	
	//Source Files
	string_list sources;
	pSelf->m_pProjectLink->GetFiles(sources,IProjectDocLink::fileSource);
	strMAK.append(TAG_PROJECT_SOURCES,chT_LBRACKET,NULL);
	for (size_t i = 0; i<sources.items().count(); i++)
		strMAK.append(chT_SPACE, sources.items()[i], chT_ENDL, NULL);
	strMAK.cat(chT_RBRACKET);

	//Output Directory
	strMAK.append(TAG_PROJECT_OUTPUT_DIR,chT_LBRACKET, chT_SPACE, 
				  pSelf->m_pProjectLink->GetOutputDirectoryFull().c_str(), chT_ENDL,chT_RBRACKET,NULL);
	strMAK.append(TAG_PROJECT_INTERMEDIATE_DIR,chT_LBRACKET, chT_SPACE, 
				  pSelf->m_pProjectLink->GetIntermediateDirectoryFull().c_str(), chT_ENDL,chT_RBRACKET,NULL);

	//Ouput File
	strMAK.append(TAG_PROJECT_OUTPUT,chT_LBRACKET, chT_SPACE, 
				  pSelf->m_pProjectLink->GetExePath().c_str(), chT_ENDL,chT_RBRACKET,NULL);
	
	//Switches
	strMAK.append(TAG_PROJECT_SWITCHES,chT_LBRACKET,chT_SPACE,
				  pSelf->m_pProjectLink->GetSwitches().c_str(),chT_ENDL,chT_RBRACKET,NULL);

	//InputTranslationDir
	strMAK.append(TAG_PROJECT_L_IN_DIR,chT_LBRACKET,chT_SPACE,
		pSelf->m_pProjectLink->GetInputTranslationDir().c_str(),chT_ENDL,chT_RBRACKET,NULL);

	//OutputTranslationDir
	strMAK.append(TAG_PROJECT_L_OUT_DIR,chT_LBRACKET,chT_SPACE,
		pSelf->m_pProjectLink->GetOutputTranslationDir().c_str(),chT_ENDL,chT_RBRACKET,NULL);

	//Libraries 
	string_list libs;
	pSelf->m_pProjectLink->GetLibs(libs);
	strMAK.append(TAG_PROJECT_LIBRARIES,chT_LBRACKET,NULL);
	for (size_t i = 0; i<libs.items().count(); i++)
		strMAK.append(chT_SPACE, extract_filenameext(libs.items()[i]).c_str(), chT_ENDL, NULL);
	strMAK.cat(chT_RBRACKET);

	//Info File
	if (!pSelf->m_pProjectLink->GetCompilerInfoFilename().isempty())
		strMAK.append(TAG_PROJECT_INFO,chT_LBRACKET,chT_SPACE,
			pSelf->m_pProjectLink->GetCompilerInfoFilename().c_str(),chT_ENDL,chT_RBRACKET,NULL);

	//Code Generators
	ILangGenRegistryLink * pLangRegLink = i_objects::GetInterface<ILangGenRegistryLink>();

	string_list lstGens;
	pSelf->m_pProjectLink->GetLangGens(lstGens);
	strMAK.append(TAG_PROJECT_LANG_GEN,chT_LBRACKET);
	for	(size_t i=0;i<lstGens.items().count();++i){
		ILangGenRegistryLink::lang_gen_item GenItem;
		if (pLangRegLink->FindByName(lstGens.items()[i],GenItem))
			strMAK.append(chT_SPACE, GenItem.GetPath().c_str(), chT_ENDL);
		else if (!guiMsgConfirm(0,FormatRes(IDS_CANT_FIND_GENERATOR_s,HRESOURCES,lstGens.items()[i]).c_str()))
			throw exception(Format("Can't find generator '%s' which is set for build.",lstGens.items()[i]).c_str());
	}
	strMAK.cat(chT_RBRACKET);

	IProjectDocLink::list_build_step  lstPreBuildSteps;
	IProjectDocLink::list_build_step  lstPostBuildSteps;
	pSelf->m_pProjectLink->GetBuildSteps(lstPreBuildSteps,lstPostBuildSteps);
	
	//Сохраняем список PreBuildSteps
	if (lstPreBuildSteps.size()!=0)
	{
		strMAK.append(TAG_PROJECT_PREBUILD_STEPS,chT_LBRACKET);
		int index =1;
		for (IProjectDocLink::list_build_step::iterator i = lstPreBuildSteps.begin();
			i!=lstPreBuildSteps.end();++i,index++)
		{
			strMAK.append(chT_SPACE,chT_SPACE,chT_DQUOTE,
				(*i).m_strDescription.c_str(),"\" | \"",
				(*i).m_strCommandLine.c_str(),chT_DQUOTE,chT_ENDL);
		}
		strMAK.cat(chT_RBRACKET);
	}

	//Сохраняем список PostBuildSteps
	if (lstPostBuildSteps.size()!=0)
	{
		strMAK.append(TAG_PROJECT_POSTBUILD_STEPS,chT_LBRACKET);
		int index =1;
		for (IProjectDocLink::list_build_step::iterator i = lstPostBuildSteps.begin();
			i!=lstPostBuildSteps.end();++i,index++)
		{
			strMAK.append(chT_SPACE,chT_SPACE,chT_DQUOTE,
				(*i).m_strDescription.c_str(),"\" | \"",
				(*i).m_strCommandLine.c_str(),chT_DQUOTE,chT_ENDL);
		}
		strMAK.cat(chT_RBRACKET);
	}

	if (!save_file(strMAKFileName.c_str(),strMAK.c_str())){
		string_smart strMessage = Format(string_res(IDS_CANT_WRITE_TO_FILE_s,HRESOURCES).c_str(),strMAKFileName.c_str());
		guiMsgAttention(0,strMessage.c_str());
		throw exception(strMessage.c_str());
	}
	
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
	
	//CommandLine
	string_smart strCommandLine(	find_full_filepath(szCompilerName).frames_wrap_on('\"','\"').c_str(),
								" ","@\"",strMAKFileName.c_str(),"\"");

	//Запускать компилятор нужно обязательно с текущей директорией проекта
	string_smart strCurrentDirectory = extract_filedir(pSelf->m_pProjectLink->GetPath());
	if (!CreateProcess(NULL,strCommandLine.buffer(),NULL,NULL,true,NORMAL_PRIORITY_CLASS,NULL,strCurrentDirectory.c_str(),&si,&pi)){
		DWORD dwErr  = GetLastError();
		CloseHandle(hWrite);
		CloseHandle(hRead);
		//if (dwErr==ERROR_FILE_NOT_FOUND)
			guiMsgAttention(0,Format(string_res(IDS_CANT_EXECUTE_ss, HRESOURCES).c_str(),szCompilerName,string_err(dwErr).c_str()).c_str());
		throw exception(Format("Can't create process.  Cause: '%s'",string_err(dwErr).c_str()).c_str());
	}
	pSelf->m_hCurrentRunProcess=pi.hProcess; //pi.hThread;//pi.hProcess;
	CloseHandle(hWrite);	
	pBuildOutputLink->Print(Format("---------- Build started: Project: %s ----------\n",pSelf->m_pProjectLink->GetName().c_str()).c_str());

	//---------------------------------------------------------------
	//Читаем результаты
	char  szBuff[256];DWORD nRead;
	while (ReadFile(hRead,szBuff,255,&nRead,NULL)){
		szBuff[nRead]=0;
		OemToCharBuff(szBuff,szBuff,255);
		pBuildOutputLink->Print(szBuff);
	}
	GetExitCodeProcess(pi.hProcess,&dwExitCode);
	CloseHandle(hRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	LOG_PROTECT_CODE_END

	//Оборачиваем в еще один try, мало ли, может указатели на интерфейсы уже не рабочие?
	LOG_PROTECT_CODE_BEGIN

	if (dwExitCode==grmc_exports::cecOK)
		pBuildOutputLink->Print("\n---------------------- Done ----------------------");
	else if (dwExitCode==grmc_exports::cecFORCED_TERMINATION)
		pBuildOutputLink->Print("\n------------- Build has been terminated ----------");

	//Закрываем сессию у объекта вывода
	if (pBuildOutputLink)
		pBuildOutputLink->StopSession();
	//---------------------------------------------------------------

	CloseHandle(pSelf->m_hCurrentBuildThread);//Закрываем декскриптор на самого себя
	InterlockedExchange((LONG*)&pSelf->m_hCurrentBuildThread,NULL);
	
	pSelf->m_pBuildContext->SelfFree(dwExitCode);

	LOG_PROTECT_CODE_END

	return 0;	
}
//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
/// CBuilder
//////////////////////////////////////////////////////////////////////////
CBuilder::CBuilder():CCObjectEx(), m_hCurrentRunProcess(NULL),m_hCurrentBuildThread(NULL)
{
}
//------------------------------------------------------------------------
CBuilder::~CBuilder()
{
	if (IsBusy())
		Stop();
}
//------------------------------------------------------------------------
