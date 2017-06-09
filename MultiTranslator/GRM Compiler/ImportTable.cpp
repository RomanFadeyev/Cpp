#include "StdAfx.h"
#include <GrmCrtLibExports.h>
#include "ImportTable.h"

//using namespace grmcrtlib_exports;

CImportTable::CImportTable(void)
{
}

CImportTable::~CImportTable(void)
{
}

void CImportTable::AddFuncRef(const func_data & cdata, OUT size_t & nID)
{
	for (nID=0; nID<m_lstUsingFuncs.size(); nID++)
		if (m_lstUsingFuncs[nID]==cdata)
			return;

	nID=m_lstUsingFuncs.size();
	m_lstUsingFuncs.push_back(cdata);
	return;
}


bool CImportTable::AddLibary(cstr szPath)
{
	if (m_Manager.register_library(szPath,string_list()))
		if ((!m_Manager.find_func(szPath,grmcrtlib_exports::szf_GetFirstFunc)) ||
			(!m_Manager.find_func(szPath,grmcrtlib_exports::szf_GetNextFunc)))
			m_Manager.unregister_library(szPath);
		else
			return true;

	return false;
}

CImportTable::find_function_result CImportTable::FindFunction(cstr szFuncName, OUT func_data & fdata,OUT size_t & nID) const
{
	grmcrtlib_exports::pf_GetFirstFunc pGetFirst;
	grmcrtlib_exports::pf_GetNextFunc	pGetNext;

	for (dll_manager::iterator i=m_Manager.begin();i!=m_Manager.end();++i){
		m_Manager.find_func(*i,grmcrtlib_exports::szf_GetFirstFunc,pGetFirst);
		m_Manager.find_func(*i,grmcrtlib_exports::szf_GetNextFunc,pGetNext);
		_ASSERT(pGetFirst);
		_ASSERT(pGetNext);
	
		for (HANDLE h = pGetFirst(); h; ){
			h = pGetNext(h,fdata);
			if (strcmp(fdata.m_szFuncName,szFuncName)==0)
			{
				//_ASSERT(strcmpi(fdata.m_szLibName,*i)==0);

				dll_function<FARPROC> Func(fdata.m_szLibName,szFuncName);
				if (!Func) 
					return ffrHasNoEntryPoint;

				const_cast<CImportTable*>(this)->AddFuncRef(fdata,nID);
				return ffrOK;
			}
		}
	}

	return ffrHasNoFunction;
}

bool CImportTable::AddFunction(cstr szLibName, cstr szFuncName, const func_data & fdata, OUT size_t & nID)
{
	func_data fd = fdata;
	strcpy(fd.m_szFuncName,szFuncName);
	strcpy(fd.m_szLibName,szLibName);
	nID=m_lstUsingFuncs.size();

	m_lstUsingFuncs.push_back(fd);
	return true;
}

size_t	CImportTable::GetCount() const
{
	return m_lstUsingFuncs.size();
}

const CImportTable::func_data & CImportTable::GetAt(size_t nIndex) const
{
	return m_lstUsingFuncs[nIndex];
}
