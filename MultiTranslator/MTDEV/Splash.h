/********************************************************************
	created:	2002/01/07
	created:	7:1:2002   17:10
	filename: 	$(Projects)\Sources\MTDEv\Splash.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	Splash
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Splash Screen. (inherited from CCObject)
*********************************************************************/

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

#include "CObjectEx.h"

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class
class CSplashWnd : public CCObjectEx
{
protected:
	bool	m_Enabled;
	clock_t m_nStart;
	mtdev_lib::ISplashScreenPtr m_spSplashScreen;

	//наследники от CCObjectEx
	override void OnCreateApp();
	override void OnRunApp();

	void ShowSplashScreen();
	void HideSplashScreen();

public:
	CSplashWnd();
	~CSplashWnd();
};


#endif
