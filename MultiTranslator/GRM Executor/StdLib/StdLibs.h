/********************************************************************
	created:	2002/10/02
	created:	2:10:2002   18:55
	filename: 	$(Projects)\sources\grm executor\library\library.h
	file path:	$(Projects)\sources\grm executor\library
	file base:	library
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Набор классов для поддержки встренных функций
*********************************************************************/

#include <map>
#include <list>
#include <BaseUtils.h>

#pragma once

using namespace std;
using namespace base_utils;
//using namespace out_streams;

namespace std_libs
{
	
	//Класс, реализущий текстовое хранилище с возможностью вставки и 
	//удаления
	class text_container
	{
	private:
		struct text_entry{
			string_smart m_strText;
			size_t m_nID;
			text_entry(cstr szText,size_t nID): m_strText(szText),m_nID(nID){
			}
		};
		bool	m_bModified;
		size_t  m_nID;
		typedef list <text_entry> list_cont;
		list_cont m_Container;
		
		size_t insert(cstr szText, list_cont::iterator i, bool bBefore);		
	public:
		text_container();
		virtual ~text_container();
		bool store (cstr szFileName);
	
		size_t get_end()		{return (m_Container.size()==0) ? 0 : m_Container.back().m_nID;}
		
		//Вставляет указанное значение в позицию nPos. 
		//Параметр bBeforeAfter=true вставляет текст перед указанной позицией
		size_t insert(variant val, size_t nPos, bool bBefore);
		size_t insert(variant val, cstr szMarker, bool bBefore);
		size_t append(variant val);
		
		void   clear(){m_Container.clear(); m_nID=0;}
	};

	//Класс, для работы c внешними DLL
	class library_manager: public base_utils::dll_manager
	{
	};
}