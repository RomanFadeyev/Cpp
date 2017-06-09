/********************************************************************
	created:	2004/01/21
	created:	21:1:2004   22:57
	filename: 	$(Projects)\sources\grm crt lib\crtsystem.cpp
	file path:	$(Projects)\sources\grm crt lib
	file base:	crtsystem
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include "StdAfx.h"
#include <BaseUtils.h>
#include "GrmCrt.h"
#include "tchar.h"

using namespace base_utils;

//------------------------------------------------------------------------
// global functions for crt system
//------------------------------------------------------------------------

#define REG(Name,Comment) registrator reg_##Name(Name,_T("System"), #Name, Comment)

namespace crt_system
{

#ifdef Assert
#undef Assert
#endif

	EXPORT bool WINAPI Assert(bool bCondition)
	{
		if (!bCondition)
			throw exception("Assertion failed");
		return bCondition;
	}
	REG(Assert,"Checks whether given expression is true");

	EXPORT bool WINAPI LaunchApp(cstr szAppName, cstr szCommandLine, int nOptions)
	{
		//Открываем процесс
		STARTUPINFO si={0};
		si.cb			= sizeof(si);
		si.dwFlags		= STARTF_USESHOWWINDOW;
		si.wShowWindow	= SW_NORMAL;
		PROCESS_INFORMATION pi;

		string_smart strCommandLine("\"",szAppName,"\" ",szCommandLine);
		//Запускать нужно обязательно с текущей директорией проекта
		if (!CreateProcess(NULL,strCommandLine.buffer(),NULL,NULL,false,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi))
			return  false;

		return true;
	}
	REG(LaunchApp,"Launches specified application. If succeeds, returns true.");

	EXPORT bool WINAPI LaunchAppAndWait(cstr szAppName, cstr szCommandLine, int nOptions, int & nExitCode)
	{
		//Открываем процесс
		STARTUPINFO si={0};
		si.cb			= sizeof(si);
		si.dwFlags		= STARTF_USESHOWWINDOW;
		si.wShowWindow	= SW_NORMAL;
		PROCESS_INFORMATION pi;

		string_smart strCommandLine("\"",szAppName,"\" ",szCommandLine);
		//Запускать компилятор нужно обязательно с текущей директорией проекта
		if (!CreateProcess(NULL,strCommandLine.buffer(),NULL,NULL,false,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi))
			return  false;

		WaitForSingleObject(pi.hProcess,INFINITE);
		DWORD dwExitCode;
		GetExitCodeProcess(pi.hProcess,&dwExitCode);
		nExitCode = dwExitCode;

		return true;
	}
	REG(LaunchAppAndWait,"Launches specified application and wait until it finishes. If succeedes, returns true.");
};
