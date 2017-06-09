/********************************************************************
	created:	2002/08/16
	created:	16:8:2002   10:18
	filename: 	$(Projects)\sources\mtdev\compile utils\LangGenRegistry.cpp
	file path:	$(Projects)\sources\mtdev\compile utils
	file base:	LangGenRegistry
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Implementation of executer 
*********************************************************************/
#include "stdafx.h"
#include <GUIService.h>

#include "Resource.h"
#include "LangGenRegistry.h"
#include <ModuleNames.h>

using namespace i_objects;
using namespace grm_lang_gen_exports;

//Создание переходника для объекта
static i_objects::object_thunk<CLangGenRegistry> LangGenRegistry(__MSG_PREFIX__);

//////////////////////////////////////////////////////////////////////
// CLangGenRegistry class
//////////////////////////////////////////////////////////////////////

CLangGenRegistry::CLangGenRegistry(): m_Scanned(false)
{
}
//------------------------------------------------------------------------
CLangGenRegistry::~CLangGenRegistry()
{
}
//------------------------------------------------------------------------
void CLangGenRegistry::ReadProperties(cstr szFilePath)
{
	LOG_PROTECT_CODE_BEGIN
	grm_lang_gen_exports::inializator Init;


	//Проверяем, чтобы не было повторений
	for (lst_lang_gen::iterator i = m_LangGenLst.begin(); i!=m_LangGenLst.end(); ++i)
		if ((*i).GetPath().cmpi(szFilePath))
			return;

	string_smart strFileName = extract_filenameext(szFilePath);
	_ASSERT(!strFileName.isempty());

	try
	{
		if (Init.open(szFilePath))
		{
			BSTR bstrDesc, bstrName;
			if SUCCEEDED(Init.iface()->GetProperties(&bstrName,&bstrDesc))
			{
				_bstr_t strDesc(bstrDesc,false);
				_bstr_t strName(bstrName,false);
				m_LangGenLst.push_back(lang_gen_item_impl(szFilePath,strName,strDesc));
			}
		}
	}
	catch (exception & e) {
		guiMsgError(0,FormatResCSTR(IDS_CODEGENERATOR_CAUSED_ERROR_ss,HRESOURCES,strFileName.c_str(),e.what()));
	}
	catch (...) {
		guiMsgError(0,FormatResCSTR(IDS_CODEGENERATOR_CAUSED_ERROR_ss,HRESOURCES,strFileName.c_str(),"undefined error"));
	}
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CLangGenRegistry::ScanDirectory(cstr szDirectory)
{
	string_smart strDirectory(include_backslash(szDirectory));
	string_smart strWildMask(strDirectory.c_str(),_T("*."),"dll");
	
	_finddata_t	FindData;
	long		FileSpec;

	if ((FileSpec=_findfirst(strWildMask.c_str(),&FindData))!=-1)
	{
		ReadProperties(string_smart (strDirectory.c_str(),FindData.name).c_str());
		while (_findnext(FileSpec,&FindData)!=-1){
			ReadProperties(string_smart (strDirectory.c_str(),FindData.name).c_str());
		}
		
		_findclose(FileSpec);
	}
}
//------------------------------------------------------------------------
void CLangGenRegistry::Scan()
{
	LOG_PROTECT_CODE_BEGIN
	m_LangGenLst.clear();

	//В реестре
	registry rgs = singleton_cache::GetApplicationLink()->GetAppSteadyRegistryKey();

	if (rgs.open(rgs.key(),szProfilePlugins_CodeGenerators))
	{
		string_smart strValueName(1024);
		for (DWORD i =0; ;i++)
		{
			DWORD length = strValueName.buffer_size(); 

			if (ERROR_SUCCESS != RegEnumValue(rgs.key(), i, strValueName.buffer(), &length, NULL, NULL, NULL,NULL))
				break;

			CLangGenRegistry::ReadProperties(strValueName.c_str());
		}
	}

	//В директории программы
	string_smart strDirectory(ApplicationData::APP_DIR,
							include_backslash(module_names::szFolderLangGen).c_str());
	ScanDirectory(strDirectory.c_str());
	
	//В доп директории
	strDirectory = get_env_variable(module_names::szEnvPath);
	if (!strDirectory.isempty())
		ScanDirectory(include_backslash(strDirectory.c_str()).cat(module_names::szFolderLangGen).c_str());

	m_Scanned=true;
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CLangGenRegistry::GetList(lst_lang_gen  & aList) const
{
	if (!m_Scanned)
		const_cast<CLangGenRegistry*> (this)-> Scan(); //???
	aList = m_LangGenLst;
}
//------------------------------------------------------------------------
bool CLangGenRegistry::FindByName(const string_adapter strName, OUT lang_gen_item & Item) const
{
	lst_lang_gen aList;
	GetList(aList);
	for (lst_lang_gen::iterator i = aList.begin(); i!=aList.end(); ++i)
		if ((*i).GetName().cmpi(strName)){
			Item = (*i);
			return true;
		}
	return false;
}