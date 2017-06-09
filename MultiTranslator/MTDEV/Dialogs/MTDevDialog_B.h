/********************************************************************
	created:	2002/12/05
	created:	5:12:2002   10:26
	filename: 	$(Projects)\sources\mtdev\dialogs\mtdevdialog_b.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	mtdevdialog_b
	file ext:	h
	author:		Fadeyev
	
	purpose:	Ѕазовый диалог дл€ всех диалогов в данном проекте
*********************************************************************/

#pragma once
#include <extcontrols.h>

class CdlgDialog_B :public CDialogEx
{
protected:
	override BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	CdlgDialog_B(UINT nIDTemplate, CWnd* pParentWnd = NULL, UINT nAcceleratorID=-1);
	virtual ~CdlgDialog_B(void);
};
