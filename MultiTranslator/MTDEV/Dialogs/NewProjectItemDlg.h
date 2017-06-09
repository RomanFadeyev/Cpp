/********************************************************************
	created:	2004/12/17
	created:	17:12:2004   16:49
	filename: 	x:\multitranslator\sources\mtdev\dialogs\newprojectitemdlg.h
	file path:	x:\multitranslator\sources\mtdev\dialogs
	file base:	newprojectitemdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Display a dialog to create new project item
*********************************************************************/
#pragma once

#include "CObjectEx.h" //Один из родителей
#include "BaseStructs.h"

class CdlgNewProjectItem : public CCObjectEx, public INewDocumentWizardLink
{
public:
	override IDocumentLink * Run(IProjectDocLink * pProjectLink);

	CdlgNewProjectItem(void);
	~CdlgNewProjectItem(void);
};
