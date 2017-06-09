#include "stdafx.h"
#include "FindComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CToolbarComboBox message handlers

BOOL CToolbarComboBoxButton::CToolbarComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		CFrameWnd* pMainFrame = GetTopLevelFrame ();
		if (pMainFrame != NULL)
		{
			switch (pMsg->wParam)
			{
			case VK_ESCAPE:
				pMainFrame->SetFocus ();
				return TRUE;
			case VK_RETURN:
				pMainFrame->PostMessage (WM_COMMAND, m_nCmd, (LPARAM) GetSafeHwnd ());
				return TRUE;
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}


