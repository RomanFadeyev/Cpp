/********************************************************************
	created:	2003/06/10
	created:	10:6:2003   19:30
	filename: 	$(Projects)\sources\grm compiler\sntxlist.h
	file path:	$(Projects)\sources\grm compiler
	file base:	sntxlist
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#pragma once
#include "BaseStructs.h"

class CSntxList:public ISntxList
{
protected:
	vector<structs::grm_term> m_lstElements;
public:
	CSntxList(void);
	~CSntxList(void);

	override void Add(const structs::grm_term & aItem){
		m_lstElements.push_back(aItem);
	}

	override void EraseFrom(size_t nPos){
		_ASSERT(nPos<=m_lstElements.size());
		m_lstElements.erase(m_lstElements.begin()+nPos,m_lstElements.end());
	}

	override structs::grm_term & GetAt(size_t nIndex) {
		_ASSERT(nIndex<GetCount());
		return m_lstElements[nIndex];
	}

	override structs::grm_term & GetBack() {
		return m_lstElements.back();
	}

	override size_t	GetCount()	const{
		return m_lstElements.size();
	}

	override void	Reserve(size_t nSize){
		m_lstElements.reserve(nSize);
	}
	
	void Clear() {m_lstElements.clear();};
};