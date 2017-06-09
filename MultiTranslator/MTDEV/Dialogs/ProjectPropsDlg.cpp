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
#include "ProjectPropsDlg.h"
#include "EditionRestrictions.h"

#include "Names.h"
#include "BaseStructs.h"

using namespace i_objects;
using namespace grmc_exports;
using namespace mtdev_lib;
//Создание переходника для объекта
static i_objects::object_thunk< CdlgProjectProps> ProjectProps(__MSG_PREFIX__);


/////////////////////////////////////////////////////////////////////////////
// CdlgProjectProps dialog
//------------------------------------------------------------------------
CdlgProjectProps::CdlgProjectProps()
{
}
//------------------------------------------------------------------------
bool CdlgProjectProps::Run(IProjectDocLink * pProjectDocLink)
{ 
	IProjectSettingsPtr spProjectSettings(__uuidof(ProjectSettings));

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


	//Основные характеристики
	base_utils::time mtime;
	get_file_time(pProjectDocLink->GetPath().c_str(),NULL,NULL,&mtime.lvalue());

	spProjectSettings->GeneralInfo=
		GeneralInfoImpl(pProjectDocLink->GetName().c_str(),
						pProjectDocLink->GetPath().c_str(),
						COleDateTime(mtime.get_time()));

	//Заметки, командная строка
	spProjectSettings->Notes=_bstr_t(pProjectDocLink->GetNotes().c_str());
	spProjectSettings->CommandLine = _bstr_t(pProjectDocLink->GetParams().c_str());

	//Директории 
	spProjectSettings->OutputDirectory = _bstr_t(pProjectDocLink->GetOutputDirectory().c_str());

	//Ключи компиляции
	string_smart strSwitches = pProjectDocLink->GetSwitches();
	SCompileSwitches switches;
	
	switches.SFX			= strSwitches.pos(szSwitch_SFX)!=-1;
	switches.Optimization	= strSwitches.pos(szSwitch_OPTIMIZATION)!=-1;
	switches.DebugInfo		= strSwitches.pos(szSwitch_DEBUG_INFO)!=-1;
	switches.Compression	= strSwitches.pos(szSwitch_COMPRESS)!=-1;
	spProjectSettings->CompileSwitches = switches;

	//InputTranslationDir + OutputTranslationDir
	spProjectSettings->InputTranslationDir = pProjectDocLink->GetInputTranslationDir().c_str();
	spProjectSettings->OutputTranslationDir = pProjectDocLink->GetOutputTranslationDir().c_str();
	string_list lstInputs,lstOutputs; 
	singleton_cache::GetParserLink()->FindTranslationDirections(*pProjectDocLink, lstInputs,lstOutputs);
	spProjectSettings->InputTranslationDirList = lstInputs.items().unite(';').c_str();
	spProjectSettings->OutputTranslationDirList = lstOutputs.items().unite(';').c_str();
	
	//Заполнение BuildSteps
	IProjectDocLink::list_build_step lstPreBuildSteps;
	IProjectDocLink::list_build_step lstPostBuildSteps;
	pProjectDocLink->GetBuildSteps(lstPreBuildSteps,lstPostBuildSteps);
	for (IProjectDocLink::list_build_step::iterator i = lstPreBuildSteps.begin();
		i!=lstPreBuildSteps.end();++i)
	{
		spProjectSettings->AddPreBuildStep((*i).m_strDescription.c_str(),
											(*i).m_strCommandLine.c_str());
	}
	pProjectDocLink->GetBuildSteps(lstPostBuildSteps,lstPostBuildSteps);
	for (IProjectDocLink::list_build_step::iterator i = lstPostBuildSteps.begin();
		i!=lstPostBuildSteps.end();++i)
	{
		spProjectSettings->AddPostBuildStep((*i).m_strDescription.c_str(),
											(*i).m_strCommandLine.c_str());
	}

	//Генераторы кода, список всех найденных Plug-Ins
	BEGIN_LIMITED_EDITION
	ILangGenRegistryLink * pLangRegLink = i_objects::GetInterface<ILangGenRegistryLink>();
	if (pLangRegLink){
		string_list lstGenChecked; pProjectDocLink->GetLangGens(lstGenChecked);

		ILangGenRegistryLink::lst_lang_gen GenList;
		pLangRegLink->GetList(GenList);
		for (ILangGenRegistryLink::lst_lang_gen::iterator i=GenList.begin(); i!=GenList.end();i++)
		{
			string_std strName((*i).GetName());

			spProjectSettings->CodeCenerators->Add(
					(*i).GetPath().c_str(),
					(*i).GetName().c_str(),
					(*i).GetDescription().c_str(),
					lstGenChecked.items().index_of(strName)!=-1
			);
		}
	}
	END_LIMITED_EDITION

	LOG_PROTECT_CODE_BEGIN
	bool res = spProjectSettings->Show()!=FALSE;

	//Записывем измененные значения
	if (res) {
		string_smart strSwitches;
		switches = spProjectSettings->CompileSwitches;
		if (switches.SFX)			strSwitches.append(szSwitch_SFX," ");
		if (switches.Optimization)	strSwitches.append(szSwitch_OPTIMIZATION," ");
		if (switches.DebugInfo)		strSwitches.append(szSwitch_DEBUG_INFO," ");
		if (switches.Compression)	strSwitches.append(szSwitch_COMPRESS," ");
		
		pProjectDocLink->SetSwitches(strSwitches.c_str());
		pProjectDocLink->SetNotes(_bstr_t(spProjectSettings->Notes));
		pProjectDocLink->SetParams(_bstr_t(spProjectSettings->CommandLine));

		//Директории 
		pProjectDocLink->SetOutputDirectory(_bstr_t(spProjectSettings->OutputDirectory));
		 
		//InputTranslationDir + OutputTranslationDir
		pProjectDocLink->SetInputTranslationDir(spProjectSettings->InputTranslationDir);
		pProjectDocLink->SetOutputTranslationDir(spProjectSettings->OutputTranslationDir);


		//Build Steps
		lstPreBuildSteps.clear();
		lstPostBuildSteps.clear();
		for (long i=0; i<spProjectSettings->PreBuildStepCount;++i){
			_bstr_t strDesc;
			_bstr_t strCmd;
			spProjectSettings->GetPreBuildStep(i,strDesc.GetAddress(),strCmd.GetAddress());
			lstPreBuildSteps.push_back(IProjectDocLink::build_step((cstr)strDesc,(cstr)strCmd));
		}
		for (long i=0; i<spProjectSettings->PostBuildStepCount;++i){
			_bstr_t strDesc;
			_bstr_t strCmd;
			spProjectSettings->GetPostBuildStep(i,strDesc.GetAddress(),strCmd.GetAddress());
			lstPostBuildSteps.push_back(IProjectDocLink::build_step((cstr)strDesc,(cstr)strCmd));
		}
		pProjectDocLink->SetBuildSteps(lstPreBuildSteps,lstPostBuildSteps);

		//Список генераторов
		_bstr_t bstrPath,bstrName,bstrDesc; VARIANT_BOOL bEnabled;
		string_list lstGenChecked;
		for (long i=0; i<spProjectSettings->CodeCenerators->Count;i++){
			spProjectSettings->CodeCenerators->GetItem(i,bstrPath.GetAddress(),bstrName.GetAddress(),bstrDesc.GetAddress(),&bEnabled);
			if (bEnabled){
				lstGenChecked.items().add(bstrName);
			}
		}
		pProjectDocLink->SetLangGens(lstGenChecked);
	}
	return res;
	LOG_PROTECT_CODE_END;
	
	return false; //Выход по исключению
}
