// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9ACBD543_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_)
#define AFX_STDAFX_H__9ACBD543_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_

#pragma once

#define _GRM_EXEC_

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#include <list>
#include <algorithm>
#include <vector>

#include <shlwapi.h>
#pragma comment (lib,"shlwapi")


#pragma warning (disable : 4800)
#define _CRTDBG_MAP_ALLOC //Extended memory debug information

#include <BaseUtils.h> //Ёто ускор€ет компил€цию

#endif // !defined(AFX_STDAFX_H__9ACBD543_2EF2_11D4_8F94_D8F7A0ADBB89__INCLUDED_)
