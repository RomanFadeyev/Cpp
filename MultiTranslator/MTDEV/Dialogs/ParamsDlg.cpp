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


#include "ParamsDlg.h"

using namespace base_utils;
using namespace mtdev_lib;

//Создание переходника для объекта
static i_objects::object_thunk< CdlgParams > ParamsDlg(__MSG_PREFIX__);


//////////////////////////////////////////////////////////////////////////
// CdlgParams 

CdlgParams::CdlgParams()
{
	m_bShowQuestionCheck=true;
	m_bNotShowInFuture=false;
}
//------------------------------------------------------------------------
bool CdlgParams::Run(string_smart & strParams,string_smart & strBrowseInitialDir,bool & bShowInFuture)
{
	IRunParametersPtr spRunParameters(__uuidof(RunParameters));
	spRunParameters->DontShowDialogAgain=!bShowInFuture;
	spRunParameters->InitialDir=strBrowseInitialDir.c_str();
	string_list lst; lst.load_from_cmd_line(strParams.c_str());
	for (size_t i=0; i<lst.items().count();i++)
		spRunParameters->AddParam(lst.items()[i]);

	bool res = spRunParameters->Show()!=FALSE;

	if (res)	{
		strBrowseInitialDir=spRunParameters->InitialDir;
		bShowInFuture = spRunParameters->DontShowDialogAgain==FALSE;
		strParams.clear();
		for (int i=0; i<spRunParameters->ParamCount;i++)
			strParams.cat((i!=0) ? " \"" : "\"").cat(spRunParameters->GetParam(i)).cat('\"');
	}
	return true;
}