/********************************************************************
	created:	2003/09/23
	created:	23:9:2003   8:54
	filename: 	$(Projects)\sources\grm dll kernel\lexsource.h
	file path:	$(Projects)\sources\grm dll kernel
	file base:	lexsource
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_LEXSOURCE_H__09BFD4C6_F090_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_LEXSOURCE_H__09BFD4C6_F090_11D3_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include "BaseStructs.h"
#include <BaseUtils.h>
#include <list>
#include <vector>
#include <boost/regex.hpp>

using namespace base_utils;
using namespace std;
using namespace grmkrnl_structs;

class CLexSource
{
	//ќсновна€ структура дл€ хранени€ цепочки входного текста
	struct symbol_info{
		char_t	m_cValue;
		size_t	m_nLine;
		size_t	m_nPos;

		symbol_info(char_t cVal, size_t nLine, size_t nPos): 
		m_cValue(cVal), m_nLine(nLine), m_nPos(nPos)
		{}
	};

	//–асширенна€ структура дл€ описани€ одного элемента, нужна дл€ предварительной обработки данных
	enum action {ac_none, ac_replace, ac_remove};
	struct symbol_info_ex: public symbol_info {
		action	m_nAction;
		cstr	m_szData;	

		symbol_info_ex(char_t cVal, size_t nLine, size_t nPos): 
		symbol_info(cVal, nLine, nPos),m_nAction(ac_none),m_szData(NULL)
		{}
	};

	typedef grm_structs::litera litera;

public:
	typedef list<symbol_info> lst_symbols;
	typedef lst_symbols::iterator src_pos;
private:
	//--------------------------------------------------------------------
	struct ignore_block_data{
		string_std m_strStart;
		string_std m_strEnd;

		ignore_block_data (cstr szStart, cstr szEnd): m_strStart(szStart), m_strEnd(szEnd)
		{}
	};

	struct replace_block_data{
		string_std m_strOld;
		string_std m_strNew;

		replace_block_data (cstr szOld, cstr szNew): m_strOld(szOld), m_strNew(szNew)
		{}
	};

	struct state{
		src_pos		m_SourcePos;
		int			m_nOverEnd;
		state() {m_nOverEnd=0;}
	};
	
	//--------------------------------------------------------------------
	string_smart	m_strDelimiters;		// используетс€ for  isSolitaryWords
	bool			m_bIgnoreWhiteSpaces;
	bool			m_bSolitaryWords;
	bool			m_bIgnoreCase;

	state			m_CurrentState;
	state			m_PrevState;

	typedef vector<ignore_block_data> lst_i_blocks;
	typedef vector<replace_block_data> lst_r_blocks;
	lst_i_blocks	m_lstIgnore;
	lst_r_blocks	m_lstReplace;
	lst_symbols		m_lstSource;
	
	src_pos				m_PrevTerminalPos;	// используетс€ в цел€х оптимизации
	EStdTerminalClass	m_nPrevTerminalType;
	bool				m_bGetStdTerminalOK;

	string_cstr		 	m_strTerminalText;	// используетс€ дл€ хранени€ текста терминалов: констант, идентификаторов
	
	typedef map<string_std,bool> map_keywords;
	map_keywords		m_mapKeyWords;

	typedef map<string_std, boost::regex> map_regex;
	map_regex			m_mapRegex;

	class re_it: public std::list<char_t>::const_iterator 
	{
	public:
		lst_symbols::iterator m_it;	// pointer to node
	public:
		typedef char_t value_type;
		typedef char_t* pointer;
		typedef char_t & const_reference;
		typedef const_reference reference;
		typedef void * _Nodeptr;

		re_it(){
		}
		
		re_it(lst_symbols::iterator it)	: m_it(it){	
		}

		const_reference operator*() const{	
			return (*m_it).m_cValue;
		}

		pointer operator->() const {	
			return (&**this);
		}

		re_it& operator++()	{	
			++m_it;
			return (*this);
		}

		re_it operator++(int) {	// postincrement
			re_it _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		re_it& operator--()	{	// predecrement
			--m_it;
			return (*this);
		}

		re_it operator--(int) {	// postdecrement
			re_it _Tmp = *this;
			--*this;
			return (_Tmp);
		}

		bool operator==(const re_it& _Right) const	{	// test for iterator equality
			return (m_it == _Right.m_it);
		}

		bool operator!=(const re_it& _Right) const	{	// test for iterator inequality
			return (!(*this == _Right));
		}
		
		operator lst_symbols::iterator () const {
			return m_it;
		}
	};

	string_std			m_strFileName;		// ’ранит им€ загруженного файла
	void				GetSymbol(litera & symbol);	// 
	void				StepBack();
		
	grmkrnl_exports::parse_info_functor	m_ParseCallBckFunc;
	bool ParseNext(OUT string_smart & strResult, OUT ELexemeCLass & nLexClass, OUT EStdTerminalClass & nStdTermClass); //—ервисна€ функци€ дл€ Parse
public:
	CLexSource();
	virtual ~CLexSource();

	void		AddIgnoreBlock	(cstr szStart, cstr	szEnd);
	void		AddReplaceBlock	(cstr szOld, cstr szNew);
	bool		OpenFile		(cstr FileName);
	litera		GetNextChar();	//Ѕерем следующий в потоке символ (передвигаем маркер)
	litera		LookNextChar(); //ѕросматриваем, что за следуюший сивмол в потоке, но маркер не двигаем
	void		ResetPos();		//—брос автомата в исходную позицию

	void SetIgnoreWhiteSpaces	(bool bValue)		{m_bIgnoreWhiteSpaces=bValue;}
	void SetIgnoreCase			(bool bValue)		{m_bIgnoreCase=bValue;}
	void SetSolitaryWords		(bool bValue)		{m_bSolitaryWords=bValue;}
	void SetDelimiters			(cstr szDelimiters)	{m_strDelimiters=szDelimiters;}
	void SetCurrentPos			(src_pos pos)		{m_CurrentState.m_SourcePos=pos;}
	
	cstr GetTerminalText()		const				{return m_strTerminalText.c_str();}
	bool GetSolitaryWords()		const				{return m_bSolitaryWords;}
	bool GetIgnoreWhiteSpaces()	const				{return m_bIgnoreWhiteSpaces;}
	bool GetIgnoreCase()		const				{return m_bIgnoreCase;}
	bool isEnd()				const;

	//Returns current iterator
	src_pos GetCurrentPos()		const		{return m_CurrentState.m_SourcePos;	}
	//Returns current line
	long	GetCurLine()		const;
	//Returns current column 
	long	GetCurCol()			const;
	//Returns current position in text: Line, Column
	grm_structs::text_pos
		GetCurrentPosition()const		{return grm_structs::text_pos(GetCurLine(),GetCurCol());}
	//Returns current position in text: Line, Column, FileName
	grm_structs::text_location 
		GetCurrentLocation()const		{return grm_structs::text_location(m_strFileName,GetCurLine(),GetCurCol());}

	void	StoreState()				{
											m_PrevState=m_CurrentState;	}
	void	ReStoreState()				{
											m_CurrentState = m_PrevState;	}
	
	//текст относительно текущего положени€ маркера
	string_smart GetTextAhead(size_t nHowMany) const;
	string_smart GetTextBehind(size_t nHowMany) const;

	bool GetAndCompare(cstr LexemeName); // при удачном сравнении указатель в списке смещаетс€, иначе остаетс€ там, где был
	bool GetStandardTerminal(EStdTerminalClass TerminalType);
	bool GetRegExpTerminal(cstr szRegExp);
	bool GetKeyword(OUT string_smart & strResult);

	void SetParseCallBck(const grmkrnl_exports::parse_info_functor & Func);

	void AddKeyword(cstr szKeyword);

	//ѕолный разбор текста и выдача списка лексем, как 
	//анализатор это себе представл€ет (было впервые добавлено дл€ распознавани€ грамматик)
	bool Parse(OUT list<lexeme_ex> & LexLst);
};

#endif // !defined(AFX_LEXSOURCE_H__09BFD4C6_F090_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
