/********************************************************************
	created:	2003/05/28
	created:	28:5:2003   20:31
	filename: 	$(Projects)\sources\grm crt lib\grmcrt\crtcontainer.cpp
	file path:	$(Projects)\sources\grm crt lib\grmcrt
	file base:	crtcontainer
	file ext:	cpp
	author:		Fadeyev R.V,
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include <map>	//Не удалять
#include <list>
#include <BaseUtils.h>
#include "grmcrt.h"
#include "CrtContainer.h"

#include "tchar.h"

#define REG(Name,Comment) registrator reg_##Name(Name,_T("Data Processing"), #Name, Comment)

using namespace base_utils;

namespace crt_containers
{
	
    //------------------------------------------------------------------------
    //-- text_container
    //------------------------------------------------------------------------
	text_container::text_container():m_nID(0)
	{
		m_bModified=false;
	}
	//------------------------------------------------------------------------
	text_container::~text_container()
	{
		clear();
	}
	//------------------------------------------------------------------------
	string_smart text_container::get_text() const 
	{
		string_smart s;
		for (list_cont::const_iterator i=m_Container.begin();i!=m_Container.end();++i)
			s.cat((*i).m_strText.c_str());
		return s;
	}
	//------------------------------------------------------------------------
	size_t text_container::insert(cstr szText, list_cont::iterator i, bool bBefore)
	{
		m_bModified=true;	
		
		if (bBefore)
			m_Container.insert(i,text_entry(szText,m_nID));
		else if (i==m_Container.end())
			m_Container.push_back(text_entry(szText,m_nID));
		else
			m_Container.insert(++i,text_entry(szText,m_nID));
		
		return m_nID++;
	}
	//------------------------------------------------------------------------
	size_t text_container::insert(variant val, size_t nPos, bool bBefore)
	{
		if (m_Container.size()==0)
			return append(val);

		for (list_cont::iterator i=m_Container.begin();i!=m_Container.end();++i)
			if ((*i).m_nID==nPos)
				return insert(val.as_cstr(),i,bBefore);
				
		return append(val);
	}
	//------------------------------------------------------------------------
	size_t text_container::insert(variant val, cstr szMarker, bool bBefore)
	{
		for (list_cont::iterator i=m_Container.begin();i!=m_Container.end();++i)
			if ((*i).m_strText==szMarker)
				return insert(val,i,bBefore);
		
		return append(val);
	}
	//------------------------------------------------------------------------
	size_t text_container::append(variant val)
	{
		return insert(val.as_cstr(),m_Container.end(),false);
	}

	//--------------------------------------------------------------------
	//Класс, обеспечивающий поддержку карт в языке
	class crt_map{
	private:
		typedef map<string_std, variant> map_values;

		map_values m_Map;
	public:
		typedef map_values::iterator iterator;

		iterator begin(){
			return m_Map.begin();
		}

		iterator end(){
			return m_Map.end();
		}

		~crt_map(){};
		//Добавляет в указанную карту указанное значение по ключу
		bool add(cstr szKey, variant & Data){
			m_Map[szKey]=Data;
			return true;
		}

		//В указанной карте по указанному ключу ищет значение
		bool lookup(cstr szKey, variant & Result){
			map_values::iterator j = m_Map.find(szKey);
			if (j==m_Map.end())
				return false;

			Result=m_Map[szKey];
			return true;
		}
		size_t size()const{
			return m_Map.size();
		}
	};
};

//------------------------------------------------------------------------
// global functions for crt map
//------------------------------------------------------------------------

typedef map<string_std, crt_containers::crt_map> map_maps;
map_maps gMaps;

//Создает новую карту с указанным названием
EXPORT bool WINAPI MapCreate(cstr szMapName)
{
	map_maps::iterator i= gMaps.find(szMapName);
	//Такая карта уже существует
	if (i!=gMaps.end()){
		return false;
	}
	gMaps[szMapName]=crt_containers::crt_map();
	return true;
}
REG(MapCreate,"Creates a map which allows to store value binded to the key");

//Создает новую карту с указанным названием
EXPORT bool WINAPI CreateMap(cstr szMapName)
{
	return MapCreate(szMapName);
}
REG(CreateMap,"Deprecated, see MapCreate");


//Добавляет в указанную карту значение по указанному ключу
//Не передавайте Data по ссылке
EXPORT bool WINAPI MapAddPair(cstr szMapName, cstr szKey, variant Data)
{
	map_maps::iterator i= gMaps.find(szMapName);
	//Ищем карту
	if (i==gMaps.end())
		return false;

	crt_containers::crt_map & Map=(*i).second;
	bool bRes = Map.add(szKey,Data);
	finalize_variant(Data); //Из-за передачи по значению
	return bRes;
}
REG(MapAddPair,"Adds to indicated map a new pair <Key,Value>. The map must be created by calling the function CreateMap");

//Добавляет в указанную карту значение по указанному ключу
//Не передавайте Data по ссылке
EXPORT bool WINAPI AddToMap(cstr szMapName, cstr szKey, variant Data)
{
	return MapAddPair(szMapName,szKey,Data);
}
REG(AddToMap,"Deprecated, see MapAddPair");


//Ищет в указанной карте значение по указанному ключу
EXPORT bool WINAPI MapLookup(cstr szMapName, cstr szKey, variant & Result)
{
	map_maps::iterator i= gMaps.find(szMapName);
	//Ищем карту
	if (i==gMaps.end()){
		return false;
	}
	crt_containers::crt_map & Map=(*i).second;
	
	return Map.lookup(szKey,Result);
}
REG(MapLookup,"Searches value in a map guided by a key");


//Ищет в указанной карте значение по указанному ключу
EXPORT bool WINAPI LookupInMap(cstr szMapName, cstr szKey, variant & Result)
{
	return MapLookup(szMapName,szKey,Result);
}
REG(LookupInMap,"Deprecated, See MapLookup");

//Возвращает количество записей в карте
EXPORT int WINAPI MapGetPairCount(cstr szMapName)
{
	map_maps::iterator i= gMaps.find(szMapName);
	//Ищем карту
	if (i==gMaps.end()){
		return -1;
	}
	crt_containers::crt_map & Map=(*i).second;

	return (int)Map.size();
}
REG(MapGetPairCount,"Returns count of pairs <Key,Value> in the specified map");

//Возвращает количество записей в карте
EXPORT cstr WINAPI MapGetKeyAt(cstr szMapName, int nIndex)
{
	STRING_IMPL("");
	map_maps::iterator i= gMaps.find(szMapName);
	//Ищем карту
	if (i==gMaps.end()){
		return s.c_str();
	}
	crt_containers::crt_map & Map=(*i).second;

	for (crt_containers::crt_map::iterator i = Map.begin(); i!=Map.end(); ++i,nIndex--)
		if (nIndex==0){
			s=(*i).first.c_str();
		}

	return s.c_str();
}
REG(MapGetKeyAt,"Returns key at the specified position in the map");

//Возвращает количество записей в карте
EXPORT cstr WINAPI MapGetValueAt(cstr szMapName, int nIndex)
{
	STRING_IMPL("");
	map_maps::iterator i= gMaps.find(szMapName);
	//Ищем карту
	if (i==gMaps.end()){
		return s.c_str();
	}
	crt_containers::crt_map & Map=(*i).second;

	for (crt_containers::crt_map::iterator i = Map.begin(); i!=Map.end(); ++i,nIndex--)
		if (nIndex==0){
			s=(*i).first.c_str();
		}

	return s.c_str();
}
REG(MapGetValueAt,"Returns value at the specified position in the map");
