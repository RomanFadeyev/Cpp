/********************************************************************
	created:	2002/09/18
	created:	18:9:2002   18:02
	filename: 	$(Projects)\sources\mtdev\dialogs\sourcepropsdlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	sourcepropsdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	������ ��� �����������/�������������� ������� ��������� �������� �������
*********************************************************************/
#pragma once

#include <ExtControls.h>
#include "CObjectEx.h" //���� �� ���������
#include "BaseStructs.h"

class CdlgSourceProps : public CCObjectEx,public ISourcePropsLink
{
public:
	CdlgSourceProps();
	virtual ~CdlgSourceProps();

protected:

	//��������� � ������ �������
	override bool Run(cstr szFileName);//������ �������
};
