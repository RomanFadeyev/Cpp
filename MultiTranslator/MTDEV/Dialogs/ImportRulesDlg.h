/********************************************************************
	created:	2002/03/10
	created:	10:3:2002   1:13
	filename: 	$(Projects)\Sources\MTDEv\Dialogs\ProjectPropsDlg.h
	file path:	$(Projects)\Sources\MTDEv\Dialogs

	file base:	ProjectPropsDlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	������ ��� �����������/�������������� ������� �������
				���������� ��� ����������� ������ �� ���������� mtdev.dll
*********************************************************************/

#pragma once

#include "CObjectEx.h" //���� �� ���������
#include "BaseStructs.h"

/////////////////////////////////////////////////////////////////////////////
// CdlgImportRules dialog
class CdlgImportRules : public CCObjectEx,public IImportGrammarLink
{
// Construction
public:
	CdlgImportRules();   
		
	//��������� � ������ �������
	override bool Run();//������ �������
};

