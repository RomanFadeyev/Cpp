/********************************************************************
	created:	2003/06/17
	created:	17:6:2003   23:08
	filename: 	$(Projects)\sources\grm compiler\importtable.h
	file path:	$(Projects)\sources\grm compiler
	file base:	importtable
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include "BaseStructs.h"

class CImportTable:public IImportTable
{
private:
	dll_manager m_Manager;
	
	typedef vector<func_data> func_use_list;
	func_use_list m_lstUsingFuncs;
	
	void AddFuncRef(const func_data& cdata, OUT size_t & nID);
public:
	CImportTable(void);
	~CImportTable(void);

	override bool AddLibary(cstr szPath);
	override find_function_result FindFunction(cstr szFuncName, OUT func_data & fdata, OUT size_t & nID) const;
	override bool AddFunction(cstr szLibName, cstr szFuncName, const func_data & fdata, OUT size_t & nID);

	override size_t	GetCount()	const;
	override const func_data & GetAt(size_t)const;
};
