/********************************************************************
	created:	2002/08/21
	created:	21:8:2002   13:46
	filename: 	$(Projects)\sources\mtdev\dialogs\paramsdlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	paramsdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Диалог ввода параметров перед запуском модуля
*********************************************************************/

#pragma once
#include "Resource.h"
#include "BaseStructs.h"
#include "CObjectEx.h"

class CdlgParams : public CCObjectEx, public ISetParamsLink
{
protected:
	bool			m_bShowQuestionCheck;
	bool			m_bNotShowInFuture;
	string_smart	m_strParams;
	string_smart	m_strInitialDir;

	//Интерфейс (ISetParamsLink)
	override bool Run(string_smart & strParams,string_smart & strBrowseInitialDir,bool & bShowInFuture);
public:
	CdlgParams();   // standard constructor
};
