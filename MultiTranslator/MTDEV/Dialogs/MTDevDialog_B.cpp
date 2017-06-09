#include "stdafx.h"
#include "mtdevdialog_b.h"
#include <AppUtils.h>
#include <AppMessages.h>
//------------------------------------------------------------------------
CdlgDialog_B::CdlgDialog_B(UINT nIDTemplate, CWnd* pParentWnd, UINT nAcceleratorID):
	CDialogEx(nIDTemplate,pParentWnd,nAcceleratorID)
{
}
//------------------------------------------------------------------------
CdlgDialog_B::~CdlgDialog_B(void)
{
}
//------------------------------------------------------------------------
BOOL CdlgDialog_B::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LOG_PROTECT_CODE_BEGIN
	return __super::OnWndMsg(message,wParam,lParam,pResult);
	LOG_PROTECT_CODE_END(MSG_EI_LocateObj("CdlgDialog_B::OnWndMsg","<CdlgDialog_B>"));
	return false;
}

