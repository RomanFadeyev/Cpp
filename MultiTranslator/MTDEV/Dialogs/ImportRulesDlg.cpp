/********************************************************************
	created:	2003/12/26
	created:	26:12:2003   23:00
	filename: 	$(Projects)\sources\mtdev\dialogs\projectpropsdlg.cpp
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	projectpropsdlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Диалог для отображения/редактирования свойств проекта
*********************************************************************/

#include "stdafx.h"
#include <GrmcExports.h>

#include "Resource.h"
#include "ImportRulesDlg.h"

#include "Names.h"
#include "BaseStructs.h"

using namespace i_objects;
using namespace grmc_exports;
using namespace mtdev_lib;
//Создание переходника для объекта
static i_objects::object_thunk< CdlgImportRules> ProjectProps(__MSG_PREFIX__);


/////////////////////////////////////////////////////////////////////////////
// CdlgImportRules dialog
//------------------------------------------------------------------------
CdlgImportRules::CdlgImportRules()
{
}
//------------------------------------------------------------------------
bool CdlgImportRules::Run()
{ 
	IImportGrammarsPtr spImportGrammars;

	//обязательно дополнительная область видимости
	{
		CWaitCursor cursor;
		_com_error err(spImportGrammars.CreateInstance(__uuidof(ImportGrammars)));
		if (!SUCCEEDED(err.Error()))
		{
			LOG_EXCEPTION(err.ErrorMessage(),&err);
			cursor.Restore();
			guiMsgError(0,FormatRes(IDS_CANT_PERFORM_OPERATION_s,HRESOURCES,err.ErrorMessage()).c_str());
			return false;
		}
	}
	
	LOG_PROTECT_CODE_BEGIN
	bool res = spImportGrammars->Show(singleton_cache::GetApplicationLink()->GetComInterface())!=FALSE;
	
	//Записывем измененные значения
	if (res) {
	}
	return res;
	LOG_PROTECT_CODE_END;
	
	return false; //Выход по исключению
}
