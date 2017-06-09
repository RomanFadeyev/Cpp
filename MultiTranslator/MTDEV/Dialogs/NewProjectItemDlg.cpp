/********************************************************************
	created:	2004/12/17
	created:	17:12:2004   16:52
	filename: 	x:\multitranslator\sources\mtdev\dialogs\newprojectitemdlg.cpp
	file path:	x:\multitranslator\sources\mtdev\dialogs
	file base:	newprojectitemdlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "NewProjectItemDlg.h"

//Создание переходника для объекта
static i_objects::object_thunk<CdlgNewProjectItem> dlgNewProjectItem(__MSG_PREFIX__);

CdlgNewProjectItem::CdlgNewProjectItem(void)
{
}

CdlgNewProjectItem::~CdlgNewProjectItem(void)
{
}

IDocumentLink * CdlgNewProjectItem::Run(IProjectDocLink * pProjectLink)
{
	LOG_PROTECT_CODE_BEGIN
	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_NULL(pDocMngrLink);

	mtdev_lib::INewProjectItemPtr spNewProjectItemPtr(__uuidof(mtdev_lib::NewProjectItem));
	spNewProjectItemPtr->InitialDir=pDocMngrLink->GetInitialDir().c_str();
	if (spNewProjectItemPtr->Show()==FALSE)
		return NULL;


	string_smart strFilename = (cstr)spNewProjectItemPtr->DocumentPath;
	if (extract_fileext(strFilename).isempty())
		strFilename.cat(EXT_SOURCES);

	if (is_file_exist(strFilename))
	{
		pDocMngrLink->SetRecentlyDir(extract_filedir(strFilename)); //Путь запоминаем
		guiMsgError(MAIN_HWND(),FormatRes(IDS_FILE_ALREADY_EXISTS_s,HRESOURCES,strFilename.c_str()));
		return NULL;
	}

	//Создаем пустой файл
	if (!save_file(strFilename,""))
	{
		guiMsgError(MAIN_HWND(),FormatRes(IDS_CANT_CREATE_FILE_s,HRESOURCES,strFilename.c_str()));
		return NULL;
	}

	pDocMngrLink->SetRecentlyDir(extract_filedir(strFilename));
	pProjectLink->AddFile(strFilename.c_str());
	return pDocMngrLink->OpenDocument(strFilename.c_str());

	LOG_PROTECT_CODE_END
	return false;
}