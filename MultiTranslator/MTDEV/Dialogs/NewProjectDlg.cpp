/********************************************************************
	created:	2002/03/03
	created:	3:3:2002   12:38
	filename: 	$(Projects)\Sources\MTDEv\Dialogs\NewProjectDlg.cpp
	file path:	$(Projects)\Sources\MTDEv\Dialogs

	file base:	NewProjectDlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Display a dialog to allow the user to select a file path.
*********************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "NewProjectDlg.h"
#include <GuiService.h>

#include "Names.h"

//Создание переходника для объекта
static i_objects::object_thunk< CdlgNewProject > CdlgNewProject(__MSG_PREFIX__);

//Список всех доступных	мастеров
typedef	vector <CdlgNewProject::CProjectWizard*> vector_wizards;
static	vector_wizards g_ProjectWizards;


/////////////////////////////////////////////////////////////////////////////
// CdlgNewProject::CProjectWizardDefault

CdlgNewProject::CProjectWizardDefault::CProjectWizardDefault()
{
	m_strTitle=string_res(IDS_PROJECT_WZD_DEF_TITLE,HRESOURCES).c_str();
	m_hIcon	= LoadIcon(HRESOURCES,MAKEINTRESOURCE(IDI_PROJECT_EMPTY));
}

CdlgNewProject::CProjectWizardDefault::~CProjectWizardDefault()
{
	DestroyIcon(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////
// CdlgNewProject::CProjectWizardSimple

CdlgNewProject::CProjectWizardSimple::CProjectWizardSimple()
{
	m_strTitle=string_res(IDS_PROJECT_WZD_SIMPLE_TITLE,HRESOURCES).c_str();
	m_hIcon	= LoadIcon(HRESOURCES,MAKEINTRESOURCE(IDI_PROJECT_SIMPLE));
}

CdlgNewProject::CProjectWizardSimple::~CProjectWizardSimple()
{
	DestroyIcon(m_hIcon);
}

#define	TXT_SIMPLE_GRAMMATIC "// Grammatic source file.\n\nint main()\n{\n\treturn 0;\n}"

bool CdlgNewProject::CProjectWizardSimple::Run(IProjectDocLink* pIface)
{
	string_smart strFileName=change_fileext(pIface->GetPath(),EXT_SOURCES);

	//Проверка на существование	такого же файла
	if (is_file_exist(strFileName)){
		guiMsgAttention(0,Format(string_res(IDS_FILE_ALREADY_EXISTS_s,HRESOURCES).c_str(),
			strFileName.c_str()).c_str());	
		return false;
	}
	//Открытие
	FILE *stream=fopen(strFileName.c_str(),"w");
	if (!stream){
		guiMsgAttention(0,Format(		string_res(IDS_CANT_WRITE_TO_FILE_s,HRESOURCES).c_str(),
			strFileName.c_str()).c_str());	
		return false;
	}
	//Запись		
	fwrite(	TXT_SIMPLE_GRAMMATIC, sizeof( char ), strlen(TXT_SIMPLE_GRAMMATIC),	stream );
	fclose(stream);
	pIface->AddFile(strFileName.c_str());
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CdlgNewProject dialog
CdlgNewProject::CdlgNewProject()
{
}

bool CdlgNewProject::Run(string_adapter szProjectNameDef, 
						string_adapter szProjectDirDef,	
						OUT string_smart & out_strProjectName,
						OUT string_smart & out_strProjectDir,
						INewProjectWizardLink* & pWizardLink
						)
{
	LOG_PROTECT_CODE_BEGIN

	mtdev_lib::INewProjectPtr spNewProjectPtr(__uuidof(mtdev_lib::NewProject));
	
	//Общие настройки
	spNewProjectPtr->DefaultDir= szProjectDirDef.c_str();
	spNewProjectPtr->ProjectName =szProjectNameDef.c_str();

	//Заполнение списка мастеров 
	m_mapRegisteredWizards.clear();
	for	(vector_wizards::iterator i=g_ProjectWizards.begin();i!=g_ProjectWizards.end();++i)
	{
		CProjectWizard & wz(*(*i));
		m_mapRegisteredWizards[spNewProjectPtr->AddWizard(wz.GetTitle(),"",(OLE_HANDLE)wz.GetImage())]=&wz;
	}


	if 	(spNewProjectPtr->Show()==FALSE)
		return false;

	out_strProjectName= spNewProjectPtr->ProjectName; 
	out_strProjectDir = spNewProjectPtr->DefaultDir;

	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_FALSE(pDocMngrLink);
	pDocMngrLink->SetRecentlyDir(out_strProjectDir);

	pWizardLink = m_mapRegisteredWizards[spNewProjectPtr->SelectedWizard];
	return true;

	LOG_PROTECT_CODE_END
	return false;
}

void CdlgNewProject::OnRunApp()
{
	//Заранее определенный и всегда	присутсвующий мастер по	умолчанию
	g_ProjectWizards.push_back(new CProjectWizardDefault);
	g_ProjectWizards.push_back(new CProjectWizardSimple);
	//Поиск	других мастеров
}

void CdlgNewProject::OnStopApp()
{
	for	(vector_wizards::iterator i= g_ProjectWizards.begin();i!=g_ProjectWizards.end();++i)
		delete (*i);
	g_ProjectWizards.clear();
}
