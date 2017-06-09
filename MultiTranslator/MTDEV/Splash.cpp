/********************************************************************
	created:	2002/05/05
	created:	5:5:2002   9:22
	filename: 	$(Projects)\sources\mtdev\splash.cpp
	file path:	$(Projects)\sources\mtdev

	file base:	splash
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реализация заставки
*********************************************************************/

#include "stdafx.h"  
#include <BaseUtils.h>

#include "resource.h"  
#include "Splash.h"  
#include "Names.h"

using namespace i_objects;

//Создание переходника для объекта
static i_objects::object_thunk< CSplashWnd  > SplashObject(__MSG_PREFIX__);

//-------------------------------------------------------------------------
CSplashWnd::CSplashWnd()
{
	m_Enabled=true; 
}
//-------------------------------------------------------------------------
CSplashWnd::~CSplashWnd()
{
}
//-------------------------------------------------------------------------
void CSplashWnd::OnCreateApp()
{
	CCommandLineInfo cmdInfo;
	AfxGetApp()->ParseCommandLine(cmdInfo);
	
	string_list lstCmdLineInfo;
	lstCmdLineInfo.load_from_cmd_line(GetCommandLine());

	//Сначала убеждаемся, что показывать заставку можно
	if ((cmdInfo.m_bShowSplash) && (!((ApplicationData::APP_AUTOMATED || ApplicationData::APP_EMBEDDED))) &&
		(cmdInfo.m_nShellCommand != CCommandLineInfo::AppRegister) && 
		(cmdInfo.m_nShellCommand != CCommandLineInfo::AppUnregister)&&
		(lstCmdLineInfo.items().index_of(_T("/ns"))==-1)){
			_com_error err(m_spSplashScreen.CreateInstance(__uuidof(mtdev_lib::SplashScreen)));
			if (SUCCEEDED(err.Error()))
				ShowSplashScreen();
		}
}
//------------------------------------------------------------------------- 
void CSplashWnd::OnRunApp()
{
	//Закрывать приходится при создании окна, хотя можно было бы и 
	//попозже, но там глючки не приятные
	HideSplashScreen();
	if (m_spSplashScreen)
		m_spSplashScreen.Release();
}
//-------------------------------------------------------------------------
void CSplashWnd::ShowSplashScreen()
{
	//#ifndef _DEBUG
	if (m_spSplashScreen){
		m_nStart = clock();
		m_spSplashScreen->Show();
	}
	//#endif
}
//-------------------------------------------------------------------------
void CSplashWnd::HideSplashScreen()
{ 
	if (m_spSplashScreen){
		#ifndef _DEBUG
		clock_t duration = (clock()-m_nStart) / CLOCKS_PER_SEC;
		//Как минимум, нужно показывать заставку 3 секунды
		if ((duration<2) && (duration>=0))
			Sleep((2-duration)*1000);
		#endif
		m_spSplashScreen->Hide();
	}
}
//-------------------------------------------------------------------------
