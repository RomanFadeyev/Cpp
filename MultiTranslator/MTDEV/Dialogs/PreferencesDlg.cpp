/********************************************************************
	created:	2002/08/21
	created:	21:8:2002   13:49
	filename: 	$(Projects)\sources\mtdev\dialogs\paramsdlg.cpp
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	paramsdlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Диалог ввода параметров перед запуском модуля
*********************************************************************/

#include "stdafx.h"
#include <BaseUtils.h>
#include "PreferencesDlg.h"

using namespace base_utils;
using namespace mtdev_lib;

//Создание переходника для объекта
static i_objects::object_thunk< CdlgPreferences > ParamsDlg(__MSG_PREFIX__);


//////////////////////////////////////////////////////////////////////////
// CdlgPreferences 

CdlgPreferences::CdlgPreferences()
{
}
//------------------------------------------------------------------------
bool CdlgPreferences::Run()
{
	LOG_PROTECT_CODE_BEGIN

	if (m_spPreferences){
	}
	else {
		guiProcessMessages();
		CWaitCursor cursor;

		_com_error err(m_spPreferences.CreateInstance(__uuidof(mtdev_lib::Preferences)));
		if (!SUCCEEDED(err.Error()))
		{
			LOG_EXCEPTION(err.ErrorMessage(),&err);

			cursor.Restore();
			guiMsgError(0,FormatRes(IDS_CANT_PERFORM_OPERATION_s,HRESOURCES,err.ErrorMessage()).c_str());
			return false;
		}
	}

	m_spPreferences->DataProvider = singleton_cache::GetStorageLink();
	m_spPreferences->DocumentTypeProvider = singleton_cache::GetSourceDocStylerLink();

	if (m_spPreferences->Show()){
		if (singleton_cache::GetSourceDocStylerLink()->Modified)
			singleton_cache::GetSourceDocStylerLink()->SaveChanges();
			
		return true;
	}
	else
		return false;
	
	LOG_PROTECT_CODE_END

	return false;
}