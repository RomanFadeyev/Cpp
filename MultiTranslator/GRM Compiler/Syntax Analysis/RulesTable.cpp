/********************************************************************
	created:	2003/06/14
	created:	14:6:2003   20:04
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\rulestable.cpp
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	rulestable
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include <algorithm>
#include "RulesTable.h"


CRulesTable::CRulesTable(){
}

CRulesTable::~CRulesTable(){
}
//------------------------------------------------------------------------
bool CRulesTable::RegisterRule(cstr szName, size_t & nIndex, bool isOptional/*=false*/)
{
	_ASSERT(szName);

	if (std::find(m_lstRules.begin(),m_lstRules.end(),szName)!=m_lstRules.end())
		return false;

	m_lstRules.push_back(szName);
	nIndex=m_lstRules.size()-1;
	return true;
}
//------------------------------------------------------------------------
cstr CRulesTable::GetRuleName(size_t nIndex) const
{
	_ASSERT(nIndex<m_lstRules.size());
	return m_lstRules[nIndex].c_str();
}
