/********************************************************************
	created:	2003/12/30
	created:	30:12:2003   9:30
	filename: 	$(Projects)\sources\grm compiler\langgentable.cpp
	file path:	$(Projects)\sources\grm compiler
	file base:	langgentable
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Language Generators (plug-ins) list. Implementation
*********************************************************************/

#include "StdAfx.h"
#include <ComUtil.h>
#include "LangGenTable.h"


//using namespace grmcrtlib_exports;

//------------------------------------------------------------------------
CLangGenTable::CLangGenTable(string_adapter szOutputFilePath)
{
	m_strOutputDir=extract_filedir(szOutputFilePath);
	m_strOuputPath=szOutputFilePath;
}
//------------------------------------------------------------------------
CLangGenTable::~CLangGenTable()
{
	for (list_init::const_iterator i = m_lstGenerators.begin(); i!=m_lstGenerators.end(); ++i)
		(*i).iface()->End();
}
//------------------------------------------------------------------------
bool CLangGenTable::AddLibary(cstr szPath)
{
	inializator Init;
	if (Init.open(szPath)){
		_bstr_t strName,strDesc;
		Init.iface()->GetProperties(strName.GetAddress(),strDesc.GetAddress());

		string_smart strWorkDir(include_backslash(m_strOutputDir),"Generators\\",include_backslash((cstr)strName));
		force_dirs(strWorkDir);

		Init.iface()->Begin(_bstr_t(strWorkDir.c_str()),_bstr_t(m_strOuputPath.c_str()));

		m_lstGenerators.push_back(Init);
		return true;
	}

	return false;
}
//------------------------------------------------------------------------
void CLangGenTable::OnGrmItem(cstr szRuleName,cstr szVariantName, cstr szItemName,size_t nItemID) const
{
	for (list_init::const_iterator i = m_lstGenerators.begin(); i!=m_lstGenerators.end(); ++i)
		(*i).iface()->OnGrmItem(_bstr_t(szRuleName),
								_bstr_t(szVariantName),
								_bstr_t(szItemName),
								nItemID);
}