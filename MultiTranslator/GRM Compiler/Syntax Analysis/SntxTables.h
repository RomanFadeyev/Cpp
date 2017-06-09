/********************************************************************
	created:	2003/06/13
	created:	13:6:2003   18:15
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\sntxtables.h
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	sntxtables
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_SNTXTABLES_H__07AA0C46_0559_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_SNTXTABLES_H__07AA0C46_0559_11D4_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include <list>
#include "BaseStructs.h"

class CSntxTable: public ISntxTable
{
private:
	struct id_array
	{
	private:
		mutable size_t m_nUnrfrIndex;
		typedef vector<SSntxID>			list_ID;
		typedef map<string_std,size_t>	map_ID;
		
		list_ID m_lstIDs;
		map_ID  m_mapIDs; // Искать можно по имени. ТОлько в этом случае имя должно существовать всегда
	public:
		id_array():m_nUnrfrIndex(0){
		}
		bool AddID(const SSntxID & ID);
		
		size_t GetSize() const						{return m_lstIDs.size();}
		const SSntxID & Get(size_t Index) const		{return m_lstIDs[Index];}
		
		bool Find(const string_std& strName) const					{return Find(strName,SSntxID());}
		bool Find(const string_std& strName, SSntxID & ResID) const	{size_t i; return Find(strName,ResID,i);}
		bool Find(const string_std& strName, SSntxID & ResID, size_t &nIndex) const;
		bool GetIndex(const string_std& strName, size_t & nIndex) const;
		bool GetUnreferencedFirst(SSntxID & ResID) const;
		bool GetUnreferencedNext(SSntxID & ResID) const;
	};

	typedef	list<id_array>	list_levels; //В данном случае выгоднее пользоваться List, т.к. для vector возможны большие перераспрделения памяти
	long					m_nCurrentLevel; // only for DEBUG
	list_levels				m_lstLevels;	 // Список массивов объявлненных переменных по уровням вложенности
	structs::type*			m_pCurStruct; // Переменная, указывающая, что мы находимя в данный момент внутри структуры
public:
	CSntxTable();
	virtual ~CSntxTable();
	
	override bool RegisterID (const SSntxID & ID);	// return false if ID already exists
	override bool RegisterID (const SSntxID &ID, size_t & Level, size_t & Index);
	override void IncLevel();
	override void DecLevel(); 

	override bool Find(const string_std& strName) const;	
	override bool Find(const string_std& strName, size_t &Level) const;
	override bool Find(const string_std& strName, SSntxID & dest) const;
	override bool Find(const string_std& strName, SSntxID & dest, size_t &Level) const;
	override bool Find(const string_std& strName, SSntxID & dest, size_t & Level, size_t & Index) const;
	
	override bool GetUnreferencedFirst(SSntxID & dest) const;
	override bool GetUnreferencedNext(SSntxID & dest) const;

	override bool SetStructCurrent(structs::type * Struct);
	override structs::type*	GetStructCurrent()	{return m_pCurStruct;}
	override bool ClearStructCurrent()			{m_pCurStruct=NULL;return true;}
	
	override size_t GetLevel() const				{return m_nCurrentLevel;}

	void  Clear();
};


#endif // !defined(AFX_SNTXTABLES_H__07AA0C46_0559_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
