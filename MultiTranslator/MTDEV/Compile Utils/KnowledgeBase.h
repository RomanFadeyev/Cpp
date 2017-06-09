/********************************************************************
	created:	2002/03/22
	created:	22:3:2002   23:03
	filename: 	$(Projects)\Sources\MTDEv\KnowledgeBase.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	KnowledgeBase
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Разборщик текста грамматик
*********************************************************************/

#if !defined(AFX_KnowledgeBase_H__3E4D234F_E659_45CB_B8F2_9F80CD1EAEBB__INCLUDED_)
#define AFX_KnowledgeBase_H__3E4D234F_E659_45CB_B8F2_9F80CD1EAEBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CObjectEx.h>
#include <BaseStructs.h>

class CKnowledgeBase: public CCObjectEx, public IKnowledgeBaseLink
{
public:
	CKnowledgeBase();
	virtual ~CKnowledgeBase();
private:
protected:
	//Наследники от IKnowledgeBase
	override bool Add (IProjectDocLink * pProjectLink);
	override bool ShowRegistry();
};

#endif // !defined(AFX_KnowledgeBase_H__3E4D234F_E659_45CB_B8F2_9F80CD1EAEBB__INCLUDED_)
