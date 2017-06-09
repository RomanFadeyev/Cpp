// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _WIN32_WINNT  0x0500

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <Windows.h>
#include <Commdlg.h>

#ifdef _DEBUG
	#import "..\\..\\Debug\\MTDev.exe"
	#include "Debug\\MTDev.tlh"
#else
	#import "..\\..\\Release\\MTDev.exe"
	#include "Release\\MTDev.tlh"
#endif

