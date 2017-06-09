/********************************************************************
	created:	2002/09/19
	created:	19:9:2002   8:55
	filename: 	$(Projects)\sources\mtdev\menucommandshandler.h
	file path:	$(Projects)\sources\mtdev
	file base:	menucommandshandler
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Обработчик ряда команд общего характера, не привязанных к 
				какому-то определенному классу

*********************************************************************/

#pragma once

#include "Names.h"
#include "BaseStructs.h"
#include "CobjectEx.h"	//Один из родителей

class CMenuCommandsHandler : public CCmdTarget,public CCObjectEx
{
	DECLARE_DYNAMIC(CMenuCommandsHandler)

public:
	CMenuCommandsHandler();
	virtual ~CMenuCommandsHandler();

protected:
	// From CCObjectEx
	override void OnCreateAppMainWnd();

	DECLARE_MESSAGE_MAP()
	afx_msg void cmdOnAppAbout();
	afx_msg void cmdOnTipOfTheDay();
	afx_msg void cmdOnWindowManager();
	afx_msg void cmdOnHelpKeyboardmap();
	afx_msg void cmdOnViewLibraries();
	afx_msg void cmdOnGrammaticalInference();
	afx_msg void cmdOnHelp();
	afx_msg void cmdupOnHelp(CCmdUI *pCmdUI);
	afx_msg void cmdOnToolsOptions();
	afx_msg void cmdOnViewKnowledgeBase();
	afx_msg void cmdOnImportRules();
};


