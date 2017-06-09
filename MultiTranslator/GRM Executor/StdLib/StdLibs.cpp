/********************************************************************
    created:    2002/10/02
    created:    2:10:2002   19:12
    filename:   $(Projects)\sources\grm executor\library\library.cpp
    file path:  $(Projects)\sources\grm executor\library
    file base:  library
    file ext:   cpp
    author:     Fadeyev R.V.
    
    purpose:    
*********************************************************************/

#include "StdAfx.h"
#include "StdLibs.h"

#include <map>	//Не удалять
#include <AppUtils.h>
#include <MiscUtils.h>
#include <AppMessages.h>

namespace std_libs
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
	bool text_container::store (cstr szFileName)
	{
		if (!m_bModified) return true;
		file_std OutputFile;
		
		if (!OutputFile.open(szFileName,"wt"))
			return false;
		
		for (list_cont::iterator i=m_Container.begin();i!=m_Container.end();++i)
			OutputFile<<(*i).m_strText.c_str();
		OutputFile.close();
		m_bModified=false;
		return true;
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
}