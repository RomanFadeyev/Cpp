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
// CdlgProjectProps dialog
class CdlgProjectProps : public CCObjectEx,public IProjPropsLink
{
// Construction
public:
	CdlgProjectProps();   
		
	//��������� � ������ �������
	override bool Run(IProjectDocLink * pProjectDocLink);//������ �������
};

