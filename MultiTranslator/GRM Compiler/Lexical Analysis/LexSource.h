/********************************************************************
	created:	2003/05/31
	created:	31:5:2003   15:24
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lexsource.h
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lexsource
	file ext:	h
	author:		Fadeyev R.V,
	
	purpose:	
*********************************************************************/

#if !defined(AFX_LEXSOURCE_H__09BFD4C6_F090_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_LEXSOURCE_H__09BFD4C6_F090_11D3_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include "BaseStructs.h"
#include "EWQueue.h"
#include <vector>
 
class CLexSource:public ILexSource
{
public:
	typedef std::vector<CLexSource*> source_list;
	
protected:
	typedef	structs::litera::lex_tag attr;
	typedef structs::litera litera;

	//Описывает единицу символа с учетом его местоположения в модуле
	struct litera_info{
		typedef litera::lex_tag attr;
		char_t	m_chName;
		attr	m_nAttr;
		size_t	m_nLine;
		size_t	m_nCol;

		
		litera_info	(char_t	chName,	attr nAttr,	size_t nLine, size_t nCol):
			m_chName(chName),m_nAttr(nAttr), m_nLine(nLine),m_nCol(nCol){
		}
		structs::text_pos GetPos() const{
			return structs::text_pos(m_nLine,m_nCol);
		}
		litera	GetLitera() const{
			return litera(m_chName,m_nAttr);
		}
	};

	typedef std::vector<litera_info> list_symbols;
	
	string_std			m_strFileName;
	list_symbols		m_lstText;		//Непосредственно текст со всеми атрибутами

	mutable litera		CurrentSymbol;	//Для процесса разбора
	mutable size_t		m_nCurPos;
	
	const litera_info & GetCurSymbolInfo()	const {_ASSERT(!IsEOF()); return m_lstText[m_nCurPos];}
public:
	//Конструктор-деструктор
	CLexSource();
	virtual ~CLexSource();
	
	//Разбор
	bool	LoadFromString(string_adapter szFileName, string_smart & strData);
	
	//Наследники от ILexSource
	override bool	IsEOF()			const {return m_nCurPos>=m_lstText.size();}
	override void	MoveToBegin()	const {m_nCurPos=0;}
	override void	MoveBack()		const {_ASSERT(m_nCurPos>0); --m_nCurPos;}		
	override void	MoveForward()	const {_ASSERT(!IsEOF()); ++m_nCurPos;}
	override const litera	GetCurSymbol()	const {return GetCurSymbolInfo().GetLitera();}
	override const litera	GetNextSymbol()	const {return m_lstText[(m_nCurPos<m_lstText.size()-1) ? m_nCurPos+1 : m_nCurPos].GetLitera();}
	
	override state  StoreState() const  {return m_nCurPos;}
	override void   RestoreState(state s) {m_nCurPos = s;}
	override const string_std &			GetFileName()	const {return m_strFileName;} //Устарела
	override structs::text_pos		GetCurPos()		const {return GetCurSymbolInfo().GetPos();}
	override structs::text_location	GetCurLocation()const {return structs::text_location(GetFileName(),GetCurPos());}
};

#endif // !defined(AFX_LEXSOURCE_H__09BFD4C6_F090_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
