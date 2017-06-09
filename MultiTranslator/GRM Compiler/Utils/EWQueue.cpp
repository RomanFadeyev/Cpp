// EWQueue.cpp: implementation of the CEWQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "EWQueue.h"

namespace errors
{
	ew_enumerator main_enumerator;
	ew_pos_corrector main_pos_corrector;

	//--------------------------------------------------------------------
	error::error(): m_nCode(errNone), m_bPositioned(false),m_bPrinted(false),m_nClassType(ctError)
	{
	}
	//--------------------------------------------------------------------
	error::~error()
	{
	}
	//--------------------------------------------------------------------
	cstr error::translate(char_t ch)
	{
		static char_t s[2]= {'\x00','\x00'};
		if (ch=='\n') return "\\n";
		if (ch=='\t') return "\\t";
			
		s[0]=ch; return s;
	}
	//--------------------------------------------------------------------
	void error::make(err_code code, const string_smart & s, const structs::text_location & pos)
	{
		m_nCode = code;
		m_bPositioned=true;
		
		m_strText=Format("'%s'<%d,%d> : %s C%d : %s",
				pos.m_strFileName.c_str(),pos.m_Pos.m_nLine+1,pos.m_Pos.m_nCol+1,
				class_name(),m_nCode, s.c_str());
		main_enumerator.add(*this);
	}
	//--------------------------------------------------------------------
	void error::make(err_code code, const string_smart & s)
	{
		m_nCode = code;
		m_bPositioned=true;
		string_smart strClassName = class_name(); strClassName.capitalize();
		m_strText=Format("%s C%d : %s",strClassName.c_str(),m_nCode, s.c_str());
		main_enumerator.add(*this);
	}
	//--------------------------------------------------------------------
	void ew_enumerator::add(const error & err, bool bPrintAtOnce)
	{
		m_lstErrors.push_back(err);
		
		if (bPrintAtOnce){
			list_error::reverse_iterator i = m_lstErrors.rbegin();
			out_streams::outs<<"\n";
			(*i).output(out_streams::outs);
			(*i).m_bPrinted=true;
		}
	}

	size_t ew_enumerator::count_ew() const
	{
		return m_lstErrors.size();
	}
	size_t ew_enumerator::count_e() const
	{
		size_t result=0;
		for(list_error::const_iterator i=m_lstErrors.begin(); i!=m_lstErrors.end();++i)
			if ((*i).is_error()) result++;
		return result;
	}
	size_t ew_enumerator::count_w() const
	{
		size_t result=0;
		for(list_error::const_iterator i=m_lstErrors.begin(); i!=m_lstErrors.end();++i)
			if ((*i).is_warning()) result++;
		return result;
	}

	void ew_enumerator::clear()
	{
		m_lstErrors.clear();
	}
	void ew_enumerator::clear_e()
	{
		size_t result=0;
		for(list_error::iterator i=m_lstErrors.begin(); i!=m_lstErrors.end();)
			if ((*i).is_error()) {
				m_lstErrors.erase(i);
				i=m_lstErrors.begin();
			}
			else
				++i;
	}
	void ew_enumerator::clear_w()
	{
		size_t result=0;
		for(list_error::iterator i=m_lstErrors.begin(); i!=m_lstErrors.end();)
			if ((*i).is_warning()) {
				m_lstErrors.erase(i);
				i=m_lstErrors.begin();
			}
			else
				++i;
	}

	size_t ew_enumerator::output_ew(out_streams::out_stream  & stream) 
	{
		size_t result = count_ew();
		for(list_error::const_iterator i=m_lstErrors.begin(); i!=m_lstErrors.end();++i)
			if (!(*i).m_bPrinted){
				stream<<"\n"; (*i).output(stream); 
			}
		return result;
	}
	size_t ew_enumerator::output_e (out_streams::out_stream & stream) 
	{
		size_t result = count_e();
		stream<<"\n";
		for(list_error::const_iterator i=m_lstErrors.begin(); i!=m_lstErrors.end();++i)
			if (((*i).is_error()) && (!(*i).m_bPrinted)) {
				stream<<"\n"; (*i).output(stream); 
			}
		return result;
	}
	size_t ew_enumerator::output_w (out_streams::out_stream & stream) 
	{
		size_t result = count_w();
		stream<<"\n";
		for(list_error::const_iterator i=m_lstErrors.begin(); i!=m_lstErrors.end();++i)
			if (((*i).is_warning()) && (!(*i).m_bPrinted)) {
				stream<<"\n"; (*i).output(stream); 
			}
		return result;
	}

	void ew_pos_corrector::AddPosition(string_adapter strFile, size_t nLine, string_adapter strMappedFile, size_t nMappedLine)
	{
		list_mapped_pos & lst = m_mapFiles[strFile.c_str()];
		lst.push_back(mapped_position(nLine,strMappedFile,nMappedLine));
	}
	
	const ew_pos_corrector::list_mapped_pos & ew_pos_corrector::GetMappedPositionsForFile(string_adapter strFile) const
	{
		list_mapped_pos & lst = const_cast<ew_pos_corrector*>(this)->m_mapFiles[strFile.c_str()];
		return lst;
	}

	void ew_pos_corrector::GetMappedPosition(string_adapter strFile, size_t nLine, string_smart & strMappedFile, size_t & nMappedLine)
	{
		list_mapped_pos & lst = m_mapFiles[strFile.c_str()];

		//Возвращаемые значения по умолчанию, обязательно должны быть промежуточные
		string_smart strOutFileName=strMappedFile;
		size_t nOutLine=nLine;

		const mapped_position * pPosition = lst.GetMappedPosition(nLine);
		if (pPosition!=NULL){
			strOutFileName = pPosition->m_strMappedFileName.c_str();
			nOutLine = pPosition->map_from(nLine);
		}

		//Компируем результаты
		strMappedFile = strOutFileName;
		nMappedLine =nOutLine;
	}
};


