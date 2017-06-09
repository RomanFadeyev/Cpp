/********************************************************************
	created:	2003/06/11
	created:	11:6:2003   11:35
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lextables.h
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lextables
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_TABLES_H__FD4741AF_FF6D_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_TABLES_H__FD4741AF_FF6D_11D3_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

//#include "define.h"
#include "BaseStructs.h"

class CLexTable: public ILexTable
{
private:
	struct const_data{
		cstr	m_szName;
		variant m_value;
	};
	typedef std::vector<cstr>			lst_IDs;
	typedef std::map<string_ptr, size_t>map_IDs;
	lst_IDs m_lstIDs;
	map_IDs m_mapIDs;

	typedef std::vector<const_data>		lst_consts;
	typedef std::map<string_ptr, size_t>map_consts;
	lst_consts m_lstConsts;
	map_consts m_mapConsts[variant::vtSTRING+1]; //Ќе расшир€емо!!! Ќужно подумать

	typedef std::vector<cstr>			lst_words;
	typedef std::map<string_ptr, size_t>map_words;
	lst_words m_lstWords;
	map_words m_mapWords;

	typedef std::vector<cstr>			lst_operators;
	typedef std::map<string_ptr, size_t>map_operators;
	lst_operators m_lstOperators;
	map_operators m_mapOperators;

	base_utils::file_std m_DiagnosticFile;
	mutable structs::lexeme LastUsedObject;

	bool LookupWord(cstr szName,structs::lexeme & Lexeme)	const;	
	bool LookupOperator(cstr szName,structs::lexeme & Lexeme) const;
	bool LookupID(cstr szName,structs::lexeme & Lexeme) const;
	bool LookupConst(cstr szName,variant::type nType, structs::lexeme & Lexeme) const;
	
	void WriteToDiagnosticFile(cstr szText,...);
public:
	CLexTable(const IBuildContext & BuildContext, cstr szFileName);
	virtual ~CLexTable();

	//ILexTable
	override const structs::lexeme & AddID				(cstr src);
	override const structs::lexeme & AddConst			(cstr src,variant::type nType);
	override const structs::lexeme & GetReservedWord	(cstr src) const; 
	override const structs::lexeme & GetOperator		(cstr src) const; 
	
	override size_t	 GetIndex(ELexemeCLass Class, const cstr src) const;
	 
	override size_t		GetConstantCount()const {return m_lstConsts.size();}
	override const variant & GetConstant(size_t nIndex) const;
	override cstr			GetConstantName(size_t nIndex) const;
	
	ret_t	AddWords			(cstr,...); //она не статическа€, т.к. не должна быть доступна кому попало
	ret_t	AddOperators		(cstr,...); //она не статическа€, т.к. не должна быть доступна кому попало
	
	structs::lexeme &	FormLexeme(const ELexemeCLass &TableClass, cstr src) const;
	structs::lexeme &	FormLexeme(cstr src) const;
};

#endif // !defined(AFX_TABLES_H__FD4741AF_FF6D_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
