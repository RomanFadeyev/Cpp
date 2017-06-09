/********************************************************************
	created:	2002/10/02
	created:	2:10:2002   18:14
	filename: 	$(Projects)\sources\grm executor\grmexec.cpp
	file path:	$(Projects)\sources\grm executor
	file base:	grmexec
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Defines the entry point for the console application.
*********************************************************************/

#include "stdafx.h"
#include <ExitCodes.h>	//Общий для всех проектов заголовок
#include <AppUtils.h>
#include <GrmExecExports.h>

#include "grmexec.h"
#include "Interpreter.h"
#include "DLLSupport.h"
 
using namespace std;
using namespace out_streams;
using namespace grmexec_exports;

//------------------------------------------------------------------------
// Вспомогательная функция запуска. iExitCode = значение возврата функции main
static ERunnerExitCodes RunInternal(int argc, cstr argv[], int & iExitCode, CInterpreter & Interpreter)
{
	_ASSERTE(argc>=1);
	if (!LoadGrammsDLL())
		return (exit_codes::recCANT_LOAD_LIB	/*==2*/);

#ifdef _DEBUG
	output("It's DEBUG version.\n\n");
#endif
	ERunnerExitCodes ret_code = Interpreter.Run(argc,argv,iExitCode);
	output("Program done.\n");
	return ret_code;
}

//реализация интерфейса IGrmKrnl
class CGrmExec: public IGrmExec
{	
private:
	int m_nRefCnt;
	GRM_HANDLE GetSymbolID();
public:
	CInterpreter m_Interpreter;

	override HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {
		return E_NOTIMPL;
	}
	override ULONG   STDMETHODCALLTYPE AddRef(void) {
		m_nRefCnt++; return m_nRefCnt;
	}
	override ULONG   STDMETHODCALLTYPE Release(void){
		m_nRefCnt--; if (m_nRefCnt==0) delete this; return m_nRefCnt;
	}
	CGrmExec(): m_nRefCnt(0){
	}

	//Прототипы	функций

	//Загрузка исполняемого	файла
	override	ERunnerExitCodes STDMETHODCALLTYPE Load(cstr szFileName);
	//Запуск выполнения. Только	после Load!
	override	ERunnerExitCodes STDMETHODCALLTYPE Run(cstr szParams,int & iExitCode);
	//Назначение функции отображения процесса разбора. Является передатчиком в другую DLL
	override	bool STDMETHODCALLTYPE SetParseCallBck(const grmkrnl_exports::parse_info_functor & Func);
	//Назначение функции отображения процесса генерации текста. Является передатчиком в другую DLL
	override	bool STDMETHODCALLTYPE SetTextGenCallBck(const grmkrnl_exports::gen_info_functor  & Func);
	//Изменения	потока вывода
	override	void STDMETHODCALLTYPE SetExternalMessager(const out_streams::stream_functor & Func);
	//Запуск выполнения	c отладочной информацией. Только после Load!
	override	ERunnerExitCodes STDMETHODCALLTYPE Debug(cstr szParams,	int	& iExitCode, const step_functor & CallbackFunc);
	//Проверка указанной точки останова	на возможность там остановится
	override	bool STDMETHODCALLTYPE CheckBreakPointOnUsed(cstr szModulePath, size_t dwLine);
	//Получение информации о именах всех доступных объектов указанного типа. Возвр строка, каждый элемент в которой отделяется '\n'
	override	string_local_alloc STDMETHODCALLTYPE GetObjectNames(get_oi_filter filter);
	//Получение информации о указанном объекте в данном контексте отладки (пока только для переменных)
	override	object_info* STDMETHODCALLTYPE GetObjectInfo(cstr szObjectName, get_oi_result & result);
	//Освобождение полученного объекта с информацией
	override	void STDMETHODCALLTYPE ReleaseObjectInfo(object_info* pInfo) ;
};

//------------------------------------------------------------------------
//Изменения потока вывода
void CGrmExec::SetExternalMessager(const out_streams::stream_functor & Func)
{
	SetStreamOut(Func);
}
//------------------------------------------------------------------------
//Назначение функции отображения процесса парсинга
bool CGrmExec::SetParseCallBck(const grmkrnl_exports::parse_info_functor  & Func){
	if (!LoadGrammsDLL())
		return false;
	(pIGrmKrnl->SetParseCallBck)(Func);
	return true;
}
//------------------------------------------------------------------------
//Назначение функции для мониторинга процесса генерации текста (после парсинга)
bool CGrmExec::SetTextGenCallBck(const grmkrnl_exports::gen_info_functor  & Func){
	if (!LoadGrammsDLL())
		return false;
	(pIGrmKrnl->SetTextGenCallBck)(Func);
	return true;
}
//------------------------------------------------------------------------
//Загрузка указанного файла
ERunnerExitCodes CGrmExec::Load(cstr szFileName)
{
	_ASSERTE(szFileName);
	if (!LoadGrammsDLL())
		return (exit_codes::recCANT_LOAD_LIB	/*==2*/);

	return m_Interpreter.Load(szFileName);
}
//------------------------------------------------------------------------
ERunnerExitCodes CGrmExec::Run(cstr szParams, int & iExitCode)
{
	_ASSERTE(szParams);
	string_list strParams; strParams.load_from_cmd_line(szParams);
	raw_buffer<cstr> buf(strParams.items().count()+1);
	buf[0]=m_Interpreter.LoadedModuleName();
	for (size_t i=1; i<buf.size();i++)
		buf[i]=strParams.items()[i-1];
	
	m_Interpreter.SetStepCallback(grmexec_exports::step_functor());
	return ::RunInternal(buf.size(),buf.ptr(),iExitCode,m_Interpreter);
}
//------------------------------------------------------------------------
ERunnerExitCodes CGrmExec::Debug(cstr szParams, int &	iExitCode, const grmexec_exports::step_functor & fCallbackFunc)
{
	_ASSERTE(szParams);
	string_list strParams; strParams.load_from_cmd_line(szParams);
	raw_buffer<cstr> buf(strParams.items().count()+1);

	buf[0]=m_Interpreter.LoadedModuleName();
	for (size_t i=1; i<buf.size();i++)
		buf[i]=strParams.items()[i-1];
	
	m_Interpreter.SetStepCallback(fCallbackFunc);
	return ::RunInternal(buf.size(),buf.ptr(),iExitCode,m_Interpreter);
}
//------------------------------------------------------------------------
bool CGrmExec::CheckBreakPointOnUsed(cstr szModulePath, size_t dwLine)
{
	_ASSERTE(m_Interpreter.IsReady());
	_ASSERTE(szModulePath);
	return m_Interpreter.CheckBreakPointOnUsed(szModulePath,dwLine);
}
//------------------------------------------------------------------------
string_local_alloc CGrmExec::GetObjectNames(get_oi_filter filter)
{
	return m_Interpreter.GetObjectNames(filter);
}
//------------------------------------------------------------------------
grmexec_exports::object_info* CGrmExec::GetObjectInfo(cstr szObjectName,grmexec_exports::get_oi_result& result)
{
	_ASSERTE(szObjectName);
	return m_Interpreter.GetObjectInfo(szObjectName,result);
}
//------------------------------------------------------------------------
void CGrmExec::ReleaseObjectInfo(object_info* pInfo)
{
	delete pInfo;
}


//------------------------------------------------------------------------
grmexec_exports::IGrmExec* WINAPI grmexec_exports::GetGrmExecInterface()
{
	return new CGrmExec;
}
//------------------------------------------------------------------------


//Переходник для обратных функций. Внутренняя реализация использует 
//более сложный тип обработчика, однако нам нужен простейший stdcall-
//совместимый обработчик, поэтому пишем переходник
struct ParseCallBackThunk {
	grmexec_exports::pf_OnItemParseCallBack m_pExternalCallBack;

	ParseCallBackThunk (grmexec_exports::pf_OnItemParseCallBack pCallBack): m_pExternalCallBack(pCallBack) {}

	void OnInternalCallBack(gen_info_packet & data){
		m_pExternalCallBack(data.dwItemID,data.szText,NULL);
	}
};


//Запуск одной функцией. Для программ-клиентов, написанных на произволном языке
ERunnerExitCodes WINAPI grmexec_exports::Execute(cstr szFileName, 
												 cstr szParams, 
												 grmexec_exports::pf_OnItemParseCallBack pCallBack, 
												 int &iExitCode)
{
	CGrmExec exec;
	ERunnerExitCodes ret=exec.Load(szFileName);
	if (ret!=recOK)
		return ret;

	string_smart strFile = find_full_filepath(szFileName);
	string_smart strDir =  extract_filedir(strFile.c_str());
	SetCurrentDirectory(strDir.c_str());	//Очень важно установить текущую директорию
											//в рабочий каталог программы

	//Организуем обратную связь
	ParseCallBackThunk Thunk(pCallBack);
	if (pCallBack)
		exec.SetTextGenCallBck(gen_info_functor (&Thunk,ParseCallBackThunk::OnInternalCallBack));
	return exec.Run(szParams,iExitCode);
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifdef _USRDLL 
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
	//Инициализация 
	switch  (ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
		{	
			_RPT0(_CRT_WARN,"grmexec DLL: process attached");
			
			TCHAR s[_MAX_PATH];
			GetModuleFileName(NULL,s,sizeof(s));
		
			strcpy(ApplicationData::APP_DIR,extract_filedir(s).c_str());
			strcpy(ApplicationData::APP_LOG_FILENAME,change_fileext(s,".log").c_str());	
			strcpy(ApplicationData::APP_NAME,extract_filename(s).c_str());
			ApplicationData::APP_INSTANCE = (HINSTANCE) hModule;
			break;
		}
		case DLL_PROCESS_DETACH:
			_RPT0(_CRT_WARN,"grmexec DLL: process detached");
			base_utils::cleanup();
			_ASSERT(_CrtCheckMemory());
			break;
	}
    return TRUE;
}
#endif    
//------------------------------------------------------------------------
#ifdef _CONSOLE 
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[]) 
{
	ERunnerExitCodes ret_code;
	int				 exit_code;

    //отключаем буферизацию вывода, иначе при чтении через pipe у нас будут большие паузы
	setvbuf( stdout, NULL, _IONBF, 0 );

#ifdef _SFX_
	#pragma message(__MSG_PREFIX__"SFX mode")
	int nFirstPrm=0;	//В SFX файле нам не нужен параметр для указания входного файла
#else
	int nFirstPrm=1;	//в обычном режиме первым параметром всегда идет имя входного файла
#endif

	if (argc<=nFirstPrm)	//это всегда будет пропускаться в SFX
	{
		string_smart strProgramName = extract_filename(get_module_filename().c_str());

		out_streams::outs<< ApplicationData::APP_NAME << ", Version " << ApplicationData::APP_VERSION << out_streams::endl;
		out_streams::outs<< ApplicationData::APP_COPYRIGHT << ". All rights reserved"<<out_streams::endl;
		out_streams::outs<< out_streams::endl;
		out_streams::outs<< "Usage:\n\n";
		out_streams::outs<< "  "<<strProgramName<<" binary_file\n";
		out_streams::outs<< out_streams::endl;
		out_streams::outs<< "Example: \n";
		out_streams::outs<< "  "<<strProgramName<<" myprog.bin\n";
		return(exit_codes::recNEED_PARAMS/*==1*/);
	}

	cstr szSourceFileName=argv[1];

	//Загрузка и выполнение
	string_smart strFile = find_full_filepath(argv[nFirstPrm]);
	string_smart strDir =  extract_filedir(strFile.c_str());
	SetCurrentDirectory(strDir.c_str());	//Очень важно установить текущую директорию
											//в рабочий каталог программы

	CGrmExec exec;
	ret_code = exec.Load(strFile.c_str());
		
	if (ret_code==recOK)
		ret_code = ::RunInternal(argc-nFirstPrm,(cstr*)argv+nFirstPrm,exit_code,exec.m_Interpreter);
		
	//Если мы нормально выполнились, тогда код возврата можно взять из пользовательской функции main
	if (ret_code==recOK)
		ret_code = (ERunnerExitCodes)(exit_code);

	base_utils::cleanup();	//очистка внутренних данных библиотеки
	return ret_code;
}
#endif

