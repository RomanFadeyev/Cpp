// SntxTables.cpp: implementation of the CSntxTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <list>

#include "SntxTables.h"

//------------------------------------------------------------------------
//- CSntxTable::id_array
//------------------------------------------------------------------------
bool CSntxTable::id_array::AddID(const SSntxID & ID)
{
	m_lstIDs.push_back(ID);
	m_mapIDs[ID.Get_Str_Text()]=m_lstIDs.size()-1;
	return true;
}

bool CSntxTable::id_array::Find(const string_std& strName,  SSntxID & ResID, size_t &nIndex) const
{
	if (!GetIndex(strName,nIndex))
		return false;

	m_lstIDs[nIndex].m_nReferencesCount++;
	ResID = m_lstIDs[nIndex];
	return true;
}

bool CSntxTable::id_array::GetIndex(const string_std& strName,  size_t & nIndex) const
{
	map_ID::const_iterator it = m_mapIDs.find(strName);
	if (it==m_mapIDs.end())
		return false;
	
	nIndex=(*it).second;
	return true;
}

bool CSntxTable::id_array::GetUnreferencedFirst(SSntxID & ResID) const
{
	for (m_nUnrfrIndex=0;m_nUnrfrIndex<m_lstIDs.size();m_nUnrfrIndex++)	
		if (m_lstIDs[m_nUnrfrIndex].m_nReferencesCount<=1) // 1 first reference by declaring
		{
			ResID=m_lstIDs[m_nUnrfrIndex];
			m_nUnrfrIndex++;
			return true;
		}
	return false;
}

bool CSntxTable::id_array::GetUnreferencedNext(SSntxID & ResID) const
{
	for (;m_nUnrfrIndex<m_lstIDs.size();m_nUnrfrIndex++)
		if (m_lstIDs[m_nUnrfrIndex].m_nReferencesCount<=1)
		{
			ResID=m_lstIDs[m_nUnrfrIndex];
			m_nUnrfrIndex++;
			return true;
		}
	return false;
}

//------------------------------------------------------------------------
//- CSntxTable
//------------------------------------------------------------------------

CSntxTable::CSntxTable()
{
	Clear();
}

CSntxTable::~CSntxTable()
{
#ifdef _DEBUG
/*	if (CErrorSemaphore::GetExitCode()==NORMAL_EXIT)
	{
		_ASSERT(m_lstLevels.size()==1);
		_ASSERT(m_nCurrentLevel==0);
	}*/
#endif
	m_lstLevels.clear();
}
//------------------------------------------------------------------------
void  CSntxTable::Clear()
{
	m_lstLevels.clear();
	m_lstLevels.push_back(id_array());	//¬сегда должен быть один базовый уровень
	m_nCurrentLevel=0;
	m_pCurStruct=NULL;
}
//------------------------------------------------------------------------
bool CSntxTable::Find(const string_std& strName) const
{
	SSntxID ID;	return Find(strName,ID);
}
//------------------------------------------------------------------------
bool CSntxTable::Find(const string_std& strName,  size_t &nLevel) const
{
	SSntxID ID;	return Find(strName,ID,nLevel);
}
//------------------------------------------------------------------------
bool CSntxTable::Find(const string_std& strName,  SSntxID & dest) const
{
	size_t nLevel; return Find(strName,dest,nLevel);
}
//------------------------------------------------------------------------
bool CSntxTable::Find(const string_std& strName,  SSntxID & dest,size_t & nLevel) const
{
	size_t nIndex; return Find(strName,dest,nLevel,nIndex);
}
//------------------------------------------------------------------------
bool CSntxTable::Find(const string_std& strName,  SSntxID & dest, size_t & Level,size_t & Index) const
{
	if (m_pCurStruct)
		return  m_pCurStruct->FindField(strName,dest,Index);
	
	Level=m_nCurrentLevel;
	for (list_levels::const_reverse_iterator i = m_lstLevels.rbegin(); i!=m_lstLevels.rend();++i,--Level)
	{
		const id_array & T=(*i);
		if (T.Find(strName,dest,Index))
			return true;
	}
	return false;
}
//------------------------------------------------------------------------
bool CSntxTable::GetUnreferencedFirst(SSntxID & dest) const
{
	_ASSERT(m_pCurStruct==NULL);
	_ASSERT(m_lstLevels.size()>0);
	return m_lstLevels.back().GetUnreferencedFirst(dest);
}
//------------------------------------------------------------------------
bool CSntxTable::GetUnreferencedNext(SSntxID & dest) const
{
	_ASSERT(m_pCurStruct==NULL);
	_ASSERT(m_lstLevels.size()>0);
	return m_lstLevels.back().GetUnreferencedNext(dest);
}
//------------------------------------------------------------------------
bool CSntxTable::RegisterID(const SSntxID &ID)
{
	size_t nLevel, nIndex;
	return RegisterID(ID,nLevel,nIndex);
}
//------------------------------------------------------------------------
bool CSntxTable::RegisterID(const SSntxID &ID, size_t & Level, size_t & Index)
{
	_ASSERT(m_lstLevels.size()>0);
	
	if (m_lstLevels.back().Find(ID.Get_Str_Text()))
		return false;

	m_lstLevels.back().AddID(ID);

	Level=m_nCurrentLevel;
	Index=m_lstLevels.back().GetSize()-1;

	return true;
}
//------------------------------------------------------------------------
void CSntxTable::IncLevel()
{
	m_lstLevels.push_back(id_array());
	m_nCurrentLevel++;	
}
//------------------------------------------------------------------------
void CSntxTable::DecLevel()
{
	_ASSERT(!m_lstLevels.empty());
	m_lstLevels.pop_back();
	_ASSERT(m_nCurrentLevel);
	m_nCurrentLevel--;
}
//------------------------------------------------------------------------
bool CSntxTable::SetStructCurrent(structs::type * Struct)
{
	m_pCurStruct=Struct;
	return true;
}
