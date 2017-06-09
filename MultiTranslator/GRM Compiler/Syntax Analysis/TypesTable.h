/********************************************************************
	created:	2003/06/13
	created:	13:6:2003   15:06
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\typestable.h
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	typestable
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_TYPESTABLE_H__7E78A7E2_15FE_11D4_8F94_E7434D7C0C94__INCLUDED_)
#define AFX_TYPESTABLE_H__7E78A7E2_15FE_11D4_8F94_E7434D7C0C94__INCLUDED_

#pragma once

#include "BaseStructs.h"

class CTypesTable: public ITypeTable
{
private:
	//Обработка structs::type для простого типа
	class type_simple:public structs::type
	{
	public:
		type_simple(cstr szTypeName,object::type VarType, size_t nIndex): structs::type(szTypeName)
		{
			m_nClass=T_SIMPLE;
			//Если строка, то у нее свой режим работы 
			m_nMemSize = (oneof(VarType,object::vtSTRING,object::vtVARIANT)) ? INT_MAX :  object::size_of(VarType); 
			m_nID=nIndex;	//Индекс в таблице
			m_nType=VarType;
		}
	};
	//Обработка structs::type для структуры
	class type_struct: public structs::type
	{
	public:
		type_struct(cstr szTypeName,size_t nIndex): structs::type(szTypeName) {
			m_nClass =T_STRUCTURE;
			m_nID = nIndex;	//Индекс в таблице
		}
	};

	//А карте элементы неперемиещаемы, так как она построена на основе
	//списка, поэтому в  list_type мы делаем указатель на данные
	typedef map<string_ptr,structs::type>	map_type;
	typedef vector<structs::type*>			list_type;
	
	list_type m_lstTypes;
	map_type  m_mapTypes;
public:
	CTypesTable();
	virtual ~CTypesTable();

	override bool CreateSimple(cstr szTypeName, object::type VarType); 
	override bool CreateStructure(cstr szTypeName);
	override bool AddFieldToLastStructure(SSntxID &ID);

	override bool FindType(cstr szTypeName,const structs::type*& TypePtr) const;
	override bool FindType(cstr szTypeName) const;
	override bool FindStdType(const object::type nVarType,const structs::type*& pType)const;

	override size_t GetCount() const	{return m_lstTypes.size();}
	override const structs::type& GetAt(size_t nIndex)	const;
};

#endif // !defined(AFX_TYPESTABLE_H__7E78A7E2_15FE_11D4_8F94_E7434D7C0C94__INCLUDED_)
