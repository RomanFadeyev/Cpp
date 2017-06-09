/********************************************************************
	created:	2004/10/18
	created:	18:10:2004   21:58
	filename: 	x:\multitranslator\sources\grm compiler\semantic analysis\lexruletable.cpp
	file path:	x:\multitranslator\sources\grm compiler\semantic analysis
	file base:	lexruletable
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include "LexRuleTable.h"

CLexRuleTable::CLexRuleTable(void)
{
}
//------------------------------------------------------------------------
CLexRuleTable::~CLexRuleTable(void)
{
}
//------------------------------------------------------------------------
bool CLexRuleTable::RegisterRule(cstr szName, size_t & nIndex, bool isOptional/*=false*/)
{
	_ASSERT(szName);

	if (std::find(m_lstRules.begin(),m_lstRules.end(),szName)!=m_lstRules.end())
		return false;

	m_lstRules.push_back(szName);
	nIndex=m_lstRules.size()-1;
	return true;
}
//------------------------------------------------------------------------
cstr CLexRuleTable::GetRuleName(size_t nIndex) const
{
	_ASSERT(nIndex<m_lstRules.size());
	return m_lstRules[nIndex].c_str();
}
//------------------------------------------------------------------------
bool CLexRuleTable::FindRule(cstr szName) const
{
	return std::find(m_lstRules.begin(),m_lstRules.end(),szName)!=m_lstRules.end();
}