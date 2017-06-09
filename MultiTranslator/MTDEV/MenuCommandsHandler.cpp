/********************************************************************
	created:	2002/09/19
	created:	19:9:2002   8:53
	filename: 	$(Projects)\sources\mtdev\menucommandshandler.cpp
	file path:	$(Projects)\sources\mtdev
	file base:	menucommandshandler
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Обработчик ряда команд общего характера, не привязанных к 
				какому-то определенному классу
*********************************************************************/

#include "stdafx.h"

#include "Resource.h"
#include "EditionRestrictions.h" //Должен идти обязательно после Resoirces.h
#include <MFCUtils.h>
#include "MenuCommandsHandler.h"
#include ".\menucommandshandler.h"

//Создание переходника для объекта
static i_objects::object_thunk<CMenuCommandsHandler> MenuCommandsHandler(__MSG_PREFIX__);

IMPLEMENT_DYNAMIC(CMenuCommandsHandler, CCmdTarget)
CMenuCommandsHandler::CMenuCommandsHandler()
{
}
//------------------------------------------------------------------------
CMenuCommandsHandler::~CMenuCommandsHandler()
{
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMenuCommandsHandler, CCmdTarget)
    ON_COMMAND(ID_APP_ABOUT, cmdOnAppAbout)
    ON_COMMAND(ID_HELP_TIP_OF_THE_DAY, cmdOnTipOfTheDay)
	ON_COMMAND(ID_WINDOW_MANAGER, cmdOnWindowManager)
	ON_COMMAND(ID_HELP_KEYBOARDMAP,	cmdOnHelpKeyboardmap)
	ON_COMMAND(ID_TOOLS_VIEW_LIBRARIES,	cmdOnViewLibraries)
	ON_COMMAND(ID_TOOLS_GRAMMATICAL_INFERENCE,	cmdOnGrammaticalInference)
	ON_COMMAND(ID_HELP, cmdOnHelp)
	ON_UPDATE_COMMAND_UI(ID_HELP, cmdupOnHelp)
	ON_COMMAND(ID_TOOLS_OPTIONS, cmdOnToolsOptions)
	ON_COMMAND(ID_TOOLS_VIEW_KNOWLEDGE_BASE, cmdOnViewKnowledgeBase)
	ON_COMMAND(ID_TOOLS_IMPORT_GRAMMAR, cmdOnImportRules)
END_MESSAGE_MAP()
//------------------------------------------------------------------------
void CMenuCommandsHandler::OnCreateAppMainWnd()
{
	//Получаем указатель на MainFrame-интерфейс
	IMainFrameLink * pMainFrame=i_objects::GetInterface<IMainFrameLink>();
	ASSERT_RETURN_VOID(pMainFrame);
	pMainFrame->IncludeToCmdMsgMap(this);
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnAppAbout() 
{
	i_objects::GetInterface<IAboutLink>()->Run();
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnTipOfTheDay() 
{
	i_objects::GetInterface<ITipOfTheDayLink>()->Run();
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnWindowManager() 
{
	CBCGMDIFrameWnd *pFrame = dynamic_cast<CBCGMDIFrameWnd*>(AfxGetMainWnd());
	ASSERT_RETURN_VOID(pFrame);
	pFrame->ShowWindowsDialog();
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnHelpKeyboardmap() 
{
	CFrameWnd *pFrame = dynamic_cast<CFrameWnd*>(AfxGetMainWnd());
	ASSERT_RETURN_VOID(pFrame);

	CBCGKeyMapDlg dlg (pFrame, TRUE /* Enable	Print */);
	dlg.DoModal	();
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnViewLibraries() 
{
	IViewLibLink * pViewLibLink=i_objects::GetInterface<IViewLibLink>();
	pViewLibLink->Run();
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnViewKnowledgeBase()
{
	BEGIN_LIMITED_EDITION
	IKnowledgeBaseLink * pLink=i_objects::GetInterface<IKnowledgeBaseLink>();
	pLink->ShowRegistry();
	END_LIMITED_EDITION
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnGrammaticalInference() 
{
	BEGIN_LIMITED_EDITION
	guiProcessMessages();
	IGrammaticalInferenceLink * pGrammaticalInference=i_objects::GetInterface<IGrammaticalInferenceLink>();
	pGrammaticalInference->Run();
	END_LIMITED_EDITION
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnHelp()
{
	ShellExecute(MAIN_HWND(),"open",
				 string_smart(ApplicationData::APP_DIR,"mtdev.mht").c_str(),
				 NULL,NULL,SW_SHOWNORMAL);
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdupOnHelp(CCmdUI *pCmdUI)
{
	//Временно. Нужно дорабатывать до нормального help
	pCmdUI->Enable(is_file_exist(string_smart(ApplicationData::APP_DIR,"mtdev.mht")));
}
//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnToolsOptions()
{
	//BEGIN_LIMITED_EDITION
	IPreferencesLink * pPreferences=i_objects::GetInterface<IPreferencesLink>();
	pPreferences->Run();
	//END_LIMITED_EDITION
}

//------------------------------------------------------------------------
void CMenuCommandsHandler::cmdOnImportRules()
{
	IImportGrammarLink* pImportLink = i_objects::GetInterface<IImportGrammarLink>();
	if (pImportLink)
		pImportLink->Run();
}
