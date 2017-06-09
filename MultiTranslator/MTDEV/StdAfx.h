// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F685084B_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
#define _ATL_APARTMENT_THREADED 
#define AFX_STDAFX_H__F685084B_7AA4_11D3_A74B_0090274409AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000 

//#include <xdebug>

#define _WIN32_WINNT 0x0501 //Для  GDI+
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#define INCLUDE_DEPRECATED_FEATURES	//Scinilla
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxadv.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _BCGCONTROLBAR_STATIC_
#include <bcgcb.h>          // BCG Control Bar

#include <atlbase.h>
#include <atlcom.h>
[ importidl(MTDev.idl) ];

#import "MTDevLib\Output\mtcg.dll" rename_namespace("mtdev_lib")
#import "MTDevSD\Output\mtsd.dll" rename_namespace("mtdev_sd")

#ifdef _DEBUG
//#import "Debug\mtdev.tlb" rename_namespace("mtdev_server")
#else
//#import "Release\mtdev.tlb" rename_namespace("mtdev_server")
#endif

#ifdef _DEBUG
	#include "Debug\mtcg.tlh"
#else
	#include "Release\mtcg.tlh"
#endif

#ifdef _DEBUG
	#include "Debug\mtsd.tlh"
#else
	#include "Release\mtsd.tlh"
#endif

//Добавление в precompiled этих файлов должно ускорить компиляцию
#include <exception>
#include <algorithm>
#include <list>
#include <vector>
#include <tree.h>

#include <IObject.h>	// concept objects
#include <BaseUtils.h>
#include <Functors.h>
#include <MiscUtils.h>
#include <AppUtils.h>
#include <ExtControls.h>
#include <GUIService.h>  

//Базовые классы, часто используются, но редко меняются
//#include "CobjectEx.h"	
#include "MTDevDialog_B.h"	

//Это должно ускорить компиляцию
#include "Names.h"
#include "BaseStructs.h"

#include "Resource.h"

//Этот макрос определяет класс расширенной кнопки
#define EXTEND_BUTTON CBCGButton
//#define EXTEND_BUTTON CxShadeButton

//Этот макрос устанавливает свойства кнопки CxShadeButton. Рекомендуется
//использовать его, а не устанавливать свойства напрямую 
#define SET_BUTTON_VIEW(Btn)  Btn.m_nFlatStyle = CBCGButton::BUTTONSTYLE_SEMIFLAT;
//#define SET_BUTTON_VIEW(CxShadeButton_Btn)  CxShadeButton_Btn.SetShade(SHS_DIAGSHADE);\
//  CxShadeButton_Btn.SetFlat(true);

#define MAX_MRU_PROJECTS    16      // Не больше 16 !!!
#define MAX_MRU_FILES       16

#define MAX_MRU_ITEMS		16	//Можно использовать как нейтральное понятие для сохранения различных списков

//Набор заранее определнных констант
#define FLAT_TABS       true
#define HIDE_NOTABS     true
#define DEFAULT_TAB_BORDER_SIZE	3

#pragma warning( disable : 4786) //identifier was truncated to '255' characters in the browser information
#pragma warning( disable : 4788)
#pragma warning( disable : 4355) //using "this"in member intialization list

#define _CRTDBG_MAP_ALLOC //Extended memory debug information
//---------

//Получение HWND главного окна
inline HWND MAIN_HWND()
{
	if (!AfxGetMainWnd())
		return 0;
	else
		return AfxGetMainWnd()->m_hWnd;
}


//Получение HWND главного окна в OLE-формате
inline OLE_HANDLE OLE_MAIN_HWND()
{
	if (!AfxGetMainWnd())
		return 0;
	else
		return (OLE_HANDLE)AfxGetMainWnd()->m_hWnd;
}

//Сервисная функция по созданию COM-объекта
template <class T>
bool CreateInterfaceInstance(_com_ptr_t <T> & aOwner,const CLSID& rclsid, cstr szLibName)
{
	_com_error err(aOwner.CreateInstance(rclsid));
	if (!SUCCEEDED(err.Error()))
	{
		LOG_EXCEPTION(err.ErrorMessage(),&err);
		guiMsgError(0,FormatRes(IDS_CANT_LOAD_INSTALLED_FILE_s,HRESOURCES,cstr_safe(szLibName)).c_str());
		return false;
	}
	return true;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F685084B_7AA4_11D3_A74B_0090274409AC__INCLUDED_)
