/********************************************************************
	created:	2003/07/09
	created:	9:7:2003   8:59
	filename: 	$(Projects)\sources\grm crt lib\crtcontainer.h
	file path:	$(Projects)\sources\grm crt lib
	file base:	crtcontainer
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once



namespace crt_containers
{
	// ласс, реализущий текстовое хранилище с возможностью вставки и 
	//удалени€
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

		size_t get_end()		{return m_nID;}
		string_smart get_text()	const;

		//¬ставл€ет указанное значение в позицию nPos. 
		//ѕараметр bBeforeAfter=true вставл€ет текст перед указанной позицией
		size_t insert(variant val, size_t nPos, bool bBefore);
		size_t insert(variant val, cstr szMarker, bool bBefore);
		size_t append(variant val);

		void   clear(){m_Container.clear(); m_nID=0;}
	};
}