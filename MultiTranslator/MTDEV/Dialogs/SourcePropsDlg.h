/********************************************************************
	created:	2002/09/18
	created:	18:9:2002   18:02
	filename: 	$(Projects)\sources\mtdev\dialogs\sourcepropsdlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	sourcepropsdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Диалог для отображения/редактирования свойств документа исходных текстов
*********************************************************************/
#pragma once

#include <ExtControls.h>
#include "CObjectEx.h" //Один из родителей
#include "BaseStructs.h"

class CdlgSourceProps : public CCObjectEx,public ISourcePropsLink
{
public:
	CdlgSourceProps();
	virtual ~CdlgSourceProps();

protected:

	//Интерфейс к нашему объекту
	override bool Run(cstr szFileName);//Запуск диалога
};
