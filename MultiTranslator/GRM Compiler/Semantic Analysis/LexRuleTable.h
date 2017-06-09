/********************************************************************
	created:	2004/10/18
	created:	18:10:2004   21:58
	filename: 	x:\multitranslator\sources\grm compiler\semantic analysis\lexruletable.h
	file path:	x:\multitranslator\sources\grm compiler\semantic analysis
	file base:	lexruletable
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

class CLexRuleTable
{
private:
	typedef vector<string_std>	list_rule;
	list_rule	m_lstRules;
public:
	CLexRuleTable(void);
	~CLexRuleTable(void);

	bool RegisterRule(cstr szName, size_t & nIndex, bool isOptional=false);
	cstr GetRuleName(size_t Index) const;
	bool FindRule(cstr szName) const;
};
