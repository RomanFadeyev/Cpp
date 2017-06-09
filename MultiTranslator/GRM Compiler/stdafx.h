// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#include <CrtDbg.h>

#include <stdlib.h>

#include <BaseUtils.h>

#pragma warning(disable:4244 4018 4267)

#include <atlbase.h>
#include <atlcom.h>

#include <shlwapi.h>
#pragma comment (lib,"shlwapi")
#define _CRTDBG_MAP_ALLOC //Extended memory debug information

#ifdef _DEBUG
	#define __DIAGNOSTIC__
#endif

#include <BaseUtils.h> //Ёто ускор€ет компил€цию
#include <GrmStructs.h> //Ёто ускор€ет компил€цию

//------------------------------------------------------------------------
#ifdef __SHOW_PERFORMANCE__
	#define TIME_REPORT(text,time_val) out_streams::outs<<text<<((double)(time_val)/CLOCKS_PER_SEC)
#else
	#define TIME_REPORT(text,time_val) 
#endif

inline const char * ConvertCompilerString(const char* szText)
{
/*	_ASSERT(*szText=='\"');
	if (*szText=='\"')
		return szText+1;
*/	
	return szText;
}

