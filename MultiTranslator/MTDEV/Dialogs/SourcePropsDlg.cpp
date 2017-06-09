/********************************************************************
	created:	2002/09/18
	created:	18:9:2002   18:00
	filename: 	c:\SourceDocs\multitranslator.v2.5\sources\mtdev\dialogs\sourcepropsdlg.cpp
	file path:	c:\SourceDocs\multitranslator.v2.5\sources\mtdev\dialogs
	file base:	sourcepropsdlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Диалог для отображения/редактирования свойств документа исходных текстов
*********************************************************************/

#include "stdafx.h"
#include "SourcePropsDlg.h"

#include "Names.h"
#include "BaseStructs.h"

using namespace i_objects;
using namespace mtdev_lib;

//Создание переходника для объекта
static i_objects::object_thunk< CdlgSourceProps> SourceProps(__MSG_PREFIX__);

//------------------------------------------------------------------------
CdlgSourceProps::CdlgSourceProps()
{
}
//------------------------------------------------------------------------
CdlgSourceProps::~CdlgSourceProps()
{
}
//------------------------------------------------------------------------
bool CdlgSourceProps::Run(cstr szFileName)
{ 
	ISourceDocumentSettingsPtr spSourceDocSettings(__uuidof(SourceDocumentSettings));

	//Структура - обертка
	struct GeneralInfoImpl : public SGeneralInfo {
		GeneralInfoImpl(const cstr szName, const cstr szPath, DATE fLastModified){
			this->Name=_bstr_t(szName).Detach();
			this->Path=_bstr_t(szPath).Detach();
			this->LastModified = fLastModified;
		}
		~GeneralInfoImpl(){
			SysFreeString(this->Name);
			SysFreeString(this->Path);
		}
	};


	LOG_PROTECT_CODE_BEGIN
	//Основные характеристики
	base_utils::time mtime;
	get_file_time(szFileName,NULL,NULL,&mtime.lvalue());

	spSourceDocSettings->GeneralInfo=
		GeneralInfoImpl(extract_filename(szFileName).c_str(),
						szFileName,
						COleDateTime(mtime.get_time()));

	return spSourceDocSettings->Show()!=FALSE;
	LOG_PROTECT_CODE_END

	return false;
}
//------------------------------------------------------------------------
