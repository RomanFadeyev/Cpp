/********************************************************************
	created:	2002/10/19
	created:	19:10:2002   9:08
	filename: 	$(Projects)\sources\grmcexports.h
	file path:	$(Projects)\sources
	file base:	grmcexports
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#include <BaseUtils.h>
#include <OutStreams.h>
#include <ExitCodes.h>

#pragma	once
using namespace	base_utils;
using namespace	exit_codes;

#ifdef _GRM_C_
  #ifdef _USRDLL 
	#pragma	message("\n --- Exporting Grmc functions... ---\n")  
	#define	GRM_C_DLL_EXPORT	 _declspec(dllexport) 
  #else
	#define	GRM_C_DLL_EXPORT
  #endif
#else
  #define GRM_C_DLL_EXPORT  _declspec(dllimport)
#endif

namespace grmc_exports
{

	//Коды возврата компилятора
	enum exit_code
	{
		cecOK					=  0,	//Все нормально
		cecINIT_FAILED			= -1,	//Не удалось по нормальному инициализироваться
		cecCOMPILE_FAILED		= -2,	//Код содержит ошибки
		cecLINK_FAILED			= -3,	//Не удалось собрать ликовщиком
		cecUSER_BREAK			= -4,	//Прервано пользователем
		cecFORCED_TERMINATION	= -255	//Принудительное завершение
	};

	//Составляющие makе-файла
	const cstr TAG_PROJECT_SOURCES  =  "Source Files:";
	const cstr TAG_PROJECT_SWITCHES =  "Switches:";
	const cstr TAG_PROJECT_LIBRARIES=  "Libraries:"; 
	const cstr TAG_PROJECT_OUTPUT   =  "Output:";
	const cstr TAG_PROJECT_L_IN_DIR   =  "Language Input Direction:";
	const cstr TAG_PROJECT_L_OUT_DIR  =  "Language Output Direction:";
	const cstr TAG_PROJECT_PREBUILD_STEPS	=  "PreBuildSteps:";
	const cstr TAG_PROJECT_POSTBUILD_STEPS	=  "PostBuildSteps:";
	const cstr TAG_PROJECT_LANG_GEN =  "LanguageGenerators:";
	const cstr TAG_PROJECT_INFO		=  "Info:";
	const cstr TAG_PROJECT_OUTPUT_DIR  =  "Output Directory:";
	const cstr TAG_PROJECT_INTERMEDIATE_DIR  =  "Intermediate Directory:";

	//Ключи компилятора
    const_str szSwitch_SFX 			= _T("/SFX");
    const_str szSwitch_DEBUG_INFO 	= _T("/DI");
    const_str szSwitch_OPTIMIZATION	= _T("/OP");
    const_str szSwitch_COMPRESS		= _T("/PACK");
	const_str szSwitch_SYNTAXCHECK	= _T("/SYNTAXCHECK");

	//Версия используемого формата сборки. В дальнейшем нужно для виртуальной 
	//машины
	const size_t nAssembleVersion = 2624; //2.6.2.4.

	//Таблица ключевых слов
	const_str pszKeywords [] = {
		#include "GRM Compiler\\DefineKeyWords.h"
	};

	//Таблица зарезервированных функций
	const_str pszReservedFuncs [] = {
		#include "GRM Compiler\\DefineReservedFunc.h"
	};

	//Таблица зарезервированных переменных
	const_str pszReservedVars [] = {
		#include "GRM Compiler\\DefineReservedVars.h"
	};

	const bool bCaseInsensivity = false;

	//Прототипы	указателей на функции
	//Названия функций
	//Прототипы	функций

	extern "C"
	{
	}
}		
