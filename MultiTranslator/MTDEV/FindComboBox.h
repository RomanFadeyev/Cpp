#pragma once
#include "Resource.h"

class CToolbarComboBoxButton : public CBCGToolbarComboBoxButton
{
protected:
	//-- class -----------------------------------------------------------
	class CToolbarComboBox : public CComboBox
	{
		UINT m_nCmd;
	public:
		CToolbarComboBox(UINT nCmd): m_nCmd(nCmd){
		}

		virtual BOOL PreTranslateMessage(MSG* pMsg);
	};
	//--------------------------------------------------------------------
	virtual CComboBox* CreateCombo (CWnd* pWndParent, const CRect& rect) {
		CToolbarComboBox* pWndCombo = new CToolbarComboBox(m_nID);
		if (!pWndCombo->Create (m_dwStyle, rect, pWndParent, m_nID))
		{
			delete pWndCombo;
			return NULL;
		}
		return pWndCombo;
	}
public:
	CToolbarComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0):
	  CBCGToolbarComboBoxButton (uiID,iImage,dwStyle, iWidth){
	 }
};

//------------------------------------------------------------------------
class CFindComboBoxButton : public CToolbarComboBoxButton
{	
public:
	CFindComboBoxButton(): CToolbarComboBoxButton(ID_EDIT_FIND_COMBO,
		CImageHash::GetImageOfCommand (ID_EDIT_FIND, FALSE),CBS_DROPDOWN){
		}

};
