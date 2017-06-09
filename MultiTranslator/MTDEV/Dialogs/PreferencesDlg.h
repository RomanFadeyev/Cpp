/********************************************************************
	created:	2004/07/08
	created:	8:7:2004   23:38
	filename: 	X:\MultiTranslator\sources\mtdev\dialogs\preferencesdlg.h
	file path:	X:\MultiTranslator\sources\mtdev\dialogs
	file base:	preferencesdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once
#include "Resource.h"
#include "BaseStructs.h"
#include "CObjectEx.h"

class CdlgPreferences : public CCObjectEx, public IPreferencesLink
{	
protected:
	mtdev_lib::IPreferencesPtr m_spPreferences;
	//Интерфейс (ISetParamsLink)
	override bool Run();

		
	override void OnStopApp()		{
		//Обязательно нужно закрыть до разрушения объектов приложения
		if (m_spPreferences) m_spPreferences.Release();
	}
public:
	CdlgPreferences();   // standard constructor
};
