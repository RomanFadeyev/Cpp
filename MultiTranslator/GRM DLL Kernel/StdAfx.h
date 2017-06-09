// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if !defined(AFX_STDAFX_H__B0C4C933_3D33_11D4_9C08_0000E8D9F1D7__INCLUDED_)
#define AFX_STDAFX_H__B0C4C933_3D33_11D4_9C08_0000E8D9F1D7__INCLUDED_

#pragma once

#define _GRM_KRNL_			// ќбозначаем марку проекта, нужно будет дл€ GrmKrnlExports.h
#define BOOST_REGEX_STATIC_LINK

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <atlbase.h>
#include <atlcom.h>

#include <list>
#include <algorithm>
#include <vector>

#define _CRTDBG_MAP_ALLOC //Extended memory debug information

#ifdef _DEBUG
	#define __DIAGNOSTIC__
#endif

#include <BaseUtils.h> //Ёто ускор€ет компил€цию
#include "GrmKrnlExports.h"

#endif // !defined(AFX_STDAFX_H__B0C4C933_3D33_11D4_9C08_0000E8D9F1D7__INCLUDED_)
