/********************************************************************
	created:	2003/06/14
	created:	14:6:2003   20:03
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\rulestable.h
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	rulestable
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_RULESTABLE_H__B0C4C925_3D33_11D4_9C08_0000E8D9F1D7__INCLUDED_)
#define AFX_RULESTABLE_H__B0C4C925_3D33_11D4_9C08_0000E8D9F1D7__INCLUDED_

#pragma once

#include "BaseStructs.h"

class CRulesTable  
{
private:
	typedef vector<string_std>	list_rule;
	list_rule	m_lstRules;
public:
	CRulesTable();
	virtual ~CRulesTable();

	bool RegisterRule(cstr szName, size_t & nIndex, bool isOptional=false);
	cstr GetRuleName(size_t Index) const;
};

#endif // !defined(AFX_RULESTABLE_H__B0C4C925_3D33_11D4_9C08_0000E8D9F1D7__INCLUDED_)
