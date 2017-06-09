// SilverRing.h: interface for the CSilverRing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SILVERRING_H__8F52F04A_6A39_47AA_9EFF_41BF62319E8B__INCLUDED_)
#define AFX_SILVERRING_H__8F52F04A_6A39_47AA_9EFF_41BF62319E8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSilverRing : public CBCGVisualManagerXP
{
	DECLARE_DYNCREATE(CSilverRing)

public:
	CSilverRing();
	virtual ~CSilverRing();

	virtual void OnFillButtonInterior (CDC* pDC,
		CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state);
	virtual void OnDrawButtonBorder (CDC* pDC,
		CBCGToolbarButton* pButton, CRect rect, CBCGVisualManager::BCGBUTTON_STATE state);

	virtual CSize GetButtonExtraBorder () const
	{
		return CSize (10, 10);
	}
};

#endif // !defined(AFX_SILVERRING_H__8F52F04A_6A39_47AA_9EFF_41BF62319E8B__INCLUDED_)
