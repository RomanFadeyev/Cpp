/********************************************************************
	created:	2003/06/13
	created:	13:6:2003   17:34
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\typestable.cpp
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	typestable
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "TypesTable.h"

//------------------------------------------------------------------------
CTypesTable::CTypesTable()
{
	m_lstTypes.reserve(32);
}
//------------------------------------------------------------------------
CTypesTable::~CTypesTable()
{

}
//------------------------------------------------------------------------
bool CTypesTable::CreateSimple(cstr szTypeName, object::type VarType)
{
	_ASSERT(szTypeName);
	if (FindType(szTypeName))
		return false;

	type_simple NewType(szTypeName,VarType,m_lstTypes.size());
	m_lstTypes.push_back( //Добавляем в карту, и указатель добавленного значения запоминаем в массиве
		&(*m_mapTypes.insert(m_mapTypes.begin(), map_type::value_type(szTypeName,NewType))).second);
	
	_ASSERT(m_mapTypes.size()==m_lstTypes.size());
	return true;
}
//------------------------------------------------------------------------
bool CTypesTable::CreateStructure(cstr szTypeName)
{
	_ASSERT(szTypeName);
	
	if (FindType(szTypeName))
		return false;
	
	type_struct NewType(szTypeName,m_lstTypes.size());
	m_lstTypes.push_back( //Добавляем в карту, и указатель добавленного значения запоминаем в массиве
		&(*m_mapTypes.insert(m_mapTypes.begin(), map_type::value_type(szTypeName,NewType))).second);
	
	_ASSERT(m_mapTypes.size()==m_lstTypes.size());
	return true;
}
//------------------------------------------------------------------------
bool CTypesTable::FindType(cstr szTypeName) const
{
	structs::type* pType;
	return FindType(szTypeName,pType);
}
//------------------------------------------------------------------------
bool CTypesTable::FindType(cstr szTypeName,const structs::type*& pType) const
{
	_ASSERT(szTypeName);
	map_type::const_iterator i = m_mapTypes.find(szTypeName);
	if (i==m_mapTypes.end())
		return false;
	
	pType = &(*i).second;
	return true;
}
//------------------------------------------------------------------------
bool CTypesTable::FindStdType(const object::type nVarType,const structs::type*& pType) const
{
	for (list_type::const_iterator i= m_lstTypes.begin(); i!=m_lstTypes.end();++i)
	{
		pType = (*i);
		if ((pType->IsSimple()) && (pType->GetType() == nVarType))
			return true;
	}
	pType=NULL;
	return false;
}
//------------------------------------------------------------------------
bool CTypesTable::AddFieldToLastStructure(SSntxID &ID)
{
	_ASSERT(m_lstTypes.size()>0);
	structs::type* pStruct = m_lstTypes.back();
	_ASSERT(pStruct->IsStruct());
	return pStruct->AddField(ID); 
}
//------------------------------------------------------------------------
const structs::type& CTypesTable::GetAt(size_t nIndex)	const
{
	_ASSERT(nIndex<GetCount());

	structs::type* pType = m_lstTypes[nIndex];
	_ASSERTE( _CrtIsValidPointer( pType, sizeof(structs::type), true));
	return *pType;
}
