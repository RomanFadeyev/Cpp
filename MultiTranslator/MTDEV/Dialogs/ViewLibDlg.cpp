/********************************************************************
	created:	2003/07/14
	created:	14:7:2003   11:31
	filename: 	$(Projects)\sources\mtdev\dialogs\viewlibdlg.cpp
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	viewlibdlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	definition of view crt library dialog
*********************************************************************/

#include "stdafx.h"
#include <ModuleNames.h>
#include <MiscUtils.h>
#include "ViewLibDlg.h"

using namespace base_utils;
using namespace grmcrtlib_exports;
using namespace mtdev_lib;

//Создание переходника для объекта
static i_objects::object_thunk< CdlgViewLib > dlgViewLib(__MSG_PREFIX__);

/////////////////////////////////////////////////////////////////////////////
// CdlgViewLib dialog

CdlgViewLib::CdlgViewLib()
{
}

//------------------------------------------------------------------------
bool CdlgViewLib::Run()
{ 
	if (!CreateInterfaceInstance(m_spViewLibrary,__uuidof(ViewCrtLibrary),module_names::szMTDevLibName))
		return false;
	m_spViewLibrary->Notifier = this;

	typedef vector<lib_data> lib_list;
	lib_list  m_lstLibs;

	//Заносим в список встроенные в язык функции, которые поддерживаются 
	//самим интерпретатором
	m_lstLibs.push_back(lib_data("Built-in functions","Built-in functions"));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Input/output","void print(x1,x2,x3...)","Prints any number of specified values on the screen."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Input/output","void write(x1,x2,x3...)","Writes any number of specified values to the file, opened with DestFileName."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Input/output","string GetText()","Returns current text from the source file according to state of syntax analyzer. May be used only in the scopes of rule, variant or term."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Input/output","int GetFilePos()","Returns current position of marker in the DestFileName."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Input/output","void InsertAfter(int Pos, string Text)","Inserts just behind the specified position in the DestFileName the specified text."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Input/output","void InsertAfter(int Pos, string Text)","Inserts before a specified position in the DestFileName the specified text."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Input/output","void SaveAllFiles()","Flushes all buffers for opened files."));

	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Translation","bool Parse()","Executes the translation process."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Translation","void IgnoreBlock(string Begin, string End)","Tells syntax analyzer to ignore the text block that begins and ends with the specified text."));
	m_lstLibs.back().m_lstFuncs.push_back(lib_data::func_data("Translation","void ReplaceBlock(string str1, string str2)","Tells syntax analyzer to replace the text str1 to str2."));

	string_list lstAdditionalDirs;
	//Ишем основные библиотеки
	string_smart strFileName;

	lib_data LibData;
	if (find_full_filepath(module_names::szCrtLibName,lstAdditionalDirs,strFileName))
		if (ObserveLibrary(strFileName.c_str(),LibData))
			m_lstLibs.push_back(LibData);

	//Поиск в других директориях

	/*
	string_smart strDirectory(include_backslash(szDirectory));
	string_smart strWildMask(strDirectory.c_str(),_T("*."),szHiLightExtension);
	_finddata_t	FindData;
	long FileSpec;

	if ((FileSpec=_findfirst(strWildMask.c_str(),&FindData))!=-1){
	ReadProperties(string_smart(strDirectory.c_str(),FindData.name).c_str());
	while (_findnext(FileSpec,&FindData)!=-1)
	ReadProperties(string_smart(strDirectory.c_str(),FindData.name).c_str());
	_findclose(FileSpec);
	}
	*/

	//Анализируем все, что насобирали
	for (size_t it=0; it<m_lstLibs.size();it++)
		AddLibrary(m_lstLibs[it]);

	LOG_PROTECT_CODE_BEGIN
		bool bRes = m_spViewLibrary->Show()!=FALSE;
		m_spViewLibrary.Release();
		return bRes;
	LOG_PROTECT_CODE_END;

	return false; //Выход по исключению
}
//------------------------------------------------------------------------
bool CdlgViewLib::ObserveLibrary(cstr szFilePath, OUT lib_data & LData)
{

	dll_loader library(szFilePath);
	if (!library)
		return false;

	LOG_PROTECT_CODE_BEGIN
	misc_utils::version_info ver_info(szFilePath);
	LData.m_strLibName=szFilePath;
	LData.m_strLibComment =	ver_info.get_FileDescription().isempty() 
								? extract_filename(szFilePath).c_str() 
								: ver_info.get_FileDescription().c_str();

	//Библиотека имеет не тот формат
	if (!LData.load(szFilePath))
		return false;
	
	//Данные в список функций добавляем в самом конце
	LData.m_hIcon= LoadIcon(library.handle(),_T("ICON_LIBRARY"));

	return true;
	LOG_PROTECT_CODE_END
	return false;
}
//------------------------------------------------------------------------
bool CdlgViewLib::AddLibrary(const lib_data & LData)
{
	string_smart s= (LData.m_strLibComment.isempty()) 
		? extract_filename(LData.m_strLibName.c_str())
		: LData.m_strLibComment.c_str();

	OLE_HANDLE hLib = m_spViewLibrary->AddLibrary(s.c_str(), LData.m_strLibName.c_str(), (OLE_HANDLE)LData.m_hIcon);
	
	if (!hLib)
		return false;

	//Добавляем функции
	for (size_t it = 0; it<LData.m_lstFuncs.size(); it++)
	{
		m_spViewLibrary->AddFunction(hLib,
			LData.m_lstFuncs[it].m_strCategory.c_str(),
			LData.m_lstFuncs[it].m_strSignature.c_str(),
			LData.m_lstFuncs[it].m_strDescription.c_str());
	}
	
	return true;
}
//------------------------------------------------------------------------
HRESULT __stdcall CdlgViewLib::raw_OnOpenLibrary (BSTR FileName )
{
	_bstr_t strFileName = FileName;
	lib_data LibData;

	if (ObserveLibrary(strFileName,LibData))
		if (AddLibrary(LibData))
			return S_OK;

	return E_FAIL;
}