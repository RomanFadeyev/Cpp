// FunctionsTable.cpp: implementation of the CFunctionsTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FunctionsTable.h"

//------------------------------------------------------------------------
CFunctionsTable::CFunctionsTable()
{
}

CFunctionsTable::~CFunctionsTable()
{
}
//------------------------------------------------------------------------
bool CFunctionsTable::CreateFunction(cstr szFuncName, const structs::type * pTypeReturn, size_t &FuncID)
{
	_ASSERT(szFuncName);
	_ASSERT(pTypeReturn);

	if (FindFunc(szFuncName,FuncID))	//Такая функция уже зарегестрирована
		return false;

	FuncID=m_lstFuncs.size();
	m_lstFuncs.push_back(func_data(szFuncName,pTypeReturn));
	m_mapFuncs[szFuncName]=FuncID;
	return true;
}
//------------------------------------------------------------------------
bool CFunctionsTable::AddParamToLastFunction(SSntxID &ID)
{
	_ASSERT(m_lstFuncs.size()>0);
	func_data & data = m_lstFuncs.back();

//	if (ParamsTable[LastIndex].Find(ID.Lexeme))
//		return false;	// параметер уже существует
	data.m_lstParams.push_back(ID);
	return true;
}
//------------------------------------------------------------------------
size_t CFunctionsTable::GetParamCount(size_t nFuncID) const
{
	_ASSERT(nFuncID<m_lstFuncs.size());
	return m_lstFuncs[nFuncID].m_lstParams.size();
}
//------------------------------------------------------------------------
const SSntxID & CFunctionsTable::GetParam(size_t nFuncID, size_t nParamIndex) const
{
	_ASSERT(nFuncID<m_lstFuncs.size());
	const func_data & data = m_lstFuncs[nFuncID];
	_ASSERT(nParamIndex<data.m_lstParams.size());
	return data.m_lstParams[nParamIndex];
}
//------------------------------------------------------------------------
const structs::type * CFunctionsTable::GetRetType(size_t nFuncID) const
{
	_ASSERT(nFuncID<m_lstFuncs.size());
	const func_data & data = m_lstFuncs[nFuncID];
	return data.m_pRetType;
}
//------------------------------------------------------------------------
bool CFunctionsTable::FindFunc(cstr szFuncName,size_t &FuncID) const
{
	_ASSERT(szFuncName);
	map_func::const_iterator i=m_mapFuncs.find(szFuncName);
	if (i==m_mapFuncs.end())
		return false;

	FuncID=(*i).second;
	return true;
}
//------------------------------------------------------------------------
bool CFunctionsTable::FindFunc(cstr szFuncName, OUT func_data & fdata) const
{
	size_t nID;
	if (!FindFunc(szFuncName,nID))
		return false;
	
	fdata = m_lstFuncs[nID];
	return true;
}