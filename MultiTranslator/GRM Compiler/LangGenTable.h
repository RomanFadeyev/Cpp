/********************************************************************
	created:	2003/06/17
	created:	17:6:2003   23:08
	filename: 	$(Projects)\sources\grm compiler\LangGentable.h
	file path:	$(Projects)\sources\grm compiler
	file base:	LangGentable
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Language Generators (plug-ins) list. Declaration
*********************************************************************/

#pragma once

#include "BaseStructs.h"
#include <GrmLangGenExports.h>

using namespace grm_lang_gen_exports;
 
class CLangGenTable:public ILangGenTable
{
private:
	typedef list<inializator> list_init;
	list_init		m_lstGenerators;
	string_smart	m_strOutputDir;
	string_smart	m_strOuputPath;
public:
	CLangGenTable(string_adapter szOutputFilePath);
	~CLangGenTable();

	override bool AddLibary(cstr szPath);
	override void OnGrmItem(cstr szRuleName,cstr szVariantName, cstr szItemName,size_t nItemID) const;
};
