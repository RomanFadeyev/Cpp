// FunctionsTable.h: interface for the CFunctionsTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTIONSTABLE_H__A356BE40_293A_11D4_9C08_0000E8D9F1D7__INCLUDED_)
#define AFX_FUNCTIONSTABLE_H__A356BE40_293A_11D4_9C08_0000E8D9F1D7__INCLUDED_

#pragma once
#include "BaseStructs.h"

class CFunctionsTable  
{
public:
	struct func_data
	{
		typedef vector<SSntxID>	list_args;

		const structs::type *	m_pRetType;
		string_std				m_strFuncName;
		list_args				m_lstParams;

		func_data(): m_pRetType(NULL){
		}

		func_data(cstr szFuncName,const structs::type *pRetType): m_strFuncName(szFuncName),m_pRetType(pRetType){
		}
    };

private:

	typedef vector<func_data> list_func;
	typedef map<string_ptr,size_t> map_func;

	list_func	m_lstFuncs;
	map_func	m_mapFuncs;
public:
	CFunctionsTable();
	virtual ~CFunctionsTable();

	bool	AddParamToLastFunction(SSntxID &ID);
	
	size_t	GetParamCount(size_t nFuncID) const;	

	const SSntxID & GetParam(size_t nFuncID, size_t nParamIndex) const;
	const structs::type * GetRetType(size_t nFuncID) const;

	bool CreateFunction(cstr szFuncName, const structs::type * pTypeReturn, size_t &FuncID);
	bool FindFunc(cstr szFunctionName,size_t &FuncID) const;
	bool FindFunc(cstr szFunctionName, OUT func_data & fdata) const;
	
	void Clear(){
		m_mapFuncs.clear();
		m_lstFuncs.clear();
	}
};

#endif // !defined(AFX_FUNCTIONSTABLE_H__A356BE40_293A_11D4_9C08_0000E8D9F1D7__INCLUDED_)
