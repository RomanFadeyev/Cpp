/********************************************************************
	created:	2003/05/31
	created:	31:5:2003   23:30
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lexanalyzer.cpp
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lexanalyzer
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include <Stack>
#include "LexAnalyzer.h"
#include "LexFunctions.h"
#include "EWQueue.h"


//////////////////////////////////////////////////////////////////////////
// class: CLexVariant
//////////////////////////////////////////////////////////////////////////
ILexSource & CLexVariant::lex_func::GetLexSource() const	{
	return CLexAnalyzer::CurLexSource();
}
ILexList& CLexVariant::lex_func::GetLexList() const	{
	return CLexAnalyzer::CurLexList();
}
//------------------------------------------------------------------------
ILexTable&	CLexVariant::lex_func::GetLexTables()	const{
	return CLexAnalyzer::CurLexTables();
}
//------------------------------------------------------------------------
CLexVariant::~CLexVariant()
{
	delete m_pEW;
}
//------------------------------------------------------------------------
CLexVariant::CLexVariant(const CLexVariant & src):m_pEW(NULL)
{
	*this  = src;
}
//------------------------------------------------------------------------
CLexVariant::CLexVariant (char_t ch, int Next_Hub, const err_w_container & EW,
						  lex_func* F1/*=NULL*/, lex_func* F2/*=NULL*/, lex_func* F3/*=NULL*/)
{
	m_LexicChar.Set(ch,litera::TAG_NONE);	//¬ данном случае на неинтересен этот признак
	if (F1!=NULL)	m_Functions.push_back(*F1);
	if (F2!=NULL)	m_Functions.push_back(*F2);
	if (F3!=NULL)	m_Functions.push_back(*F3);

	m_nNextHub=Next_Hub;
	m_pEW = EW.copy();
}
//------------------------------------------------------------------------
CLexVariant::CLexVariant (lex_tag Tag,int Next_Hub, const err_w_container & EW,
						  lex_func* F1/*=NULL*/, lex_func* F2/*=NULL*/, lex_func* F3/*=NULL*/)
{
	m_LexicChar.Set(0,Tag);
	if (F1!=NULL)	m_Functions.push_back(*F1);
	if (F2!=NULL)	m_Functions.push_back(*F2);
	if (F3!=NULL)	m_Functions.push_back(*F3);
	m_nNextHub=Next_Hub;
	m_pEW = EW.copy();
}
//------------------------------------------------------------------------
CLexVariant::CLexVariant(const CLexAutomat* pNextLAU1, const CLexAutomat* pNextLAU2/*=NULL*/, const CLexAutomat* pNextLAU3/*=NULL*/)
{
	_ASSERT(pNextLAU1);
	m_LexicChar.Set(0,litera::TAG_ANY);
	m_NextLAU.push_back(pNextLAU1);
	if (pNextLAU2)	m_NextLAU.push_back(pNextLAU2);
	if (pNextLAU3)	m_NextLAU.push_back(pNextLAU3);
	m_pEW = NULL;
	m_nNextHub=-1;
}
//------------------------------------------------------------------------
CLexVariant & CLexVariant::operator=(const CLexVariant& src)
{
	delete m_pEW;
	if (src.m_pEW)
		m_pEW=src.m_pEW->copy();
	else
		m_pEW=NULL;

	m_NextLAU	= src.m_NextLAU;
	m_Functions = src.m_Functions;
	m_LexicChar = src.m_LexicChar;
	m_nNextHub	= src.m_nNextHub;
	return *this;
} 
//------------------------------------------------------------------------
bool CLexVariant::PassToHub(const structs::litera & l)const
{
	for (list_automats::const_iterator i = m_NextLAU.begin(); i!=m_NextLAU.end(); i++)
		if ((*i)->GetHub(0).TryTranslate(l)) 
			return true;
	return false;
}
//------------------------------------------------------------------------
void CLexVariant::InvokeFunctions() const
{
	for (list_func::const_iterator i = m_Functions.begin(); i!=m_Functions.end();++i)
		(*i).invoke(m_LexicChar);
}
//------------------------------------------------------------------------
void CLexVariant::RaiseErrWarns() const
{
	if (m_pEW)
		m_pEW->raise(CLexAnalyzer::CurLexSource().GetCurLocation());
}


//////////////////////////////////////////////////////////////////////////
// class: LexHub
//////////////////////////////////////////////////////////////////////////
CLexHub::CLexHub()
{

}
//------------------------------------------------------------------------
CLexHub::CLexHub(const CLexHub &src)
{
	*this=src;
} 
//------------------------------------------------------------------------  
CLexHub::CLexHub (const CLexVariant & src)
{
	AddVariant(src);
}
CLexHub::CLexHub (const CLexVariant & src1,const CLexVariant & src2)
{
	AddVariant(src1); AddVariant(src2);
}
CLexHub::CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3)
{
	AddVariant(src1); AddVariant(src2); AddVariant(src3); 
}
CLexHub::CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3, const CLexVariant & src4)
{
	AddVariant(src1); AddVariant(src2); AddVariant(src3); AddVariant(src4); 
}
CLexHub::CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3, const CLexVariant & src4,const CLexVariant & src5)
{
	AddVariant(src1); AddVariant(src2); AddVariant(src3); AddVariant(src4); AddVariant(src5); 
}
CLexHub::CLexHub (const CLexVariant & src1,const CLexVariant & src2, const CLexVariant & src3, const CLexVariant & src4,const CLexVariant & src5, const CLexVariant & src6)
{
	AddVariant(src1); AddVariant(src2); AddVariant(src3); AddVariant(src4); AddVariant(src5); AddVariant(src6);
}
//------------------------------------------------------------------------
CLexHub::~CLexHub()
{

}
//------------------------------------------------------------------------
CLexHub & CLexHub::operator=(const CLexHub& src)
{
	m_lstVariants = src.m_lstVariants;
	return *this;
} 
//------------------------------------------------------------------------ 
void CLexHub::AddVariant(const CLexVariant & src)
{
	m_lstVariants.push_back(src);
}
//------------------------------------------------------------------------
// ¬ообще Search осуществл€ет приоритетный поиск - до первого совпадени€
// но относительно ANOTHER_HUB она пересматривает все вырианты до попадани€ TryTranslate(..) == true
int  CLexHub::Search(const structs::litera & C) const
{
	for (list_variant::const_iterator i = m_lstVariants.begin(); i!=m_lstVariants.end(); ++i)
	{
		if ((*i).IsProper(C))
		{	
			if ((*i).IsLinkToRule())
			{
				if ((*i).PassToHub(C))
					return EXIT_TRUE;
			}
			else
			{
				(*i).RaiseErrWarns();
				(*i).InvokeFunctions();

				return (*i).NextHub();
			}
		}
	}
	return EXIT_FALSE;
} 
//------------------------------------------------------------------------
bool CLexHub::TryTranslate(const structs::litera & C) const
{
	for (list_variant::const_iterator i = m_lstVariants.begin(); i!=m_lstVariants.end(); ++i)
		if ((*i).IsProper(C))
			if ((*i).IsLinkToRule())	
				return (*i).PassToHub(C);
			else								
				return true;
				
	return false;
}

//////////////////////////////////////////////////////////////////////
// class CLexAutomat
//////////////////////////////////////////////////////////////////////
bool CLexAutomat::Translate(const ILexSource & LexSource) const
{
	int nNextHub=0;
	for (structs::litera C = LexSource.GetCurSymbol();;)
	{
		CLexFunctions::BeginTranslate();
		nNextHub=GetHub(nNextHub).Search(C);
		if (nNextHub==EXIT_TRUE)	return true;
		if (nNextHub==EXIT_FALSE)	return false;
		CLexFunctions::EndTranslate(C.Name);
		if (C.Name==0)
			return false;
		LexSource.MoveForward();
		C=LexSource.GetCurSymbol();
	}
	return true;
}
//////////////////////////////////////////////////////////////////////
// class CLexAnalyzer
//////////////////////////////////////////////////////////////////////

ILexSource * CLexAnalyzer::m_pCurLexSource;
ILexList*	 CLexAnalyzer::m_pCurLexList;
ILexTable*	 CLexAnalyzer::m_pCurLexTables;
CLexAnalyzer::CLexAnalyzer()
{
}
//------------------------------------------------------------------------
CLexAnalyzer::~CLexAnalyzer()
{

}
//------------------------------------------------------------------------
CLexAutomat& CLexAnalyzer::AddAutomat(const CLexAutomat & LA)
{
	m_lstAutomats.push_back(LA);
	return (*m_lstAutomats.rbegin());
}
//------------------------------------------------------------------------
CLexAutomat& CLexAnalyzer::AddAutomat(cstr szName)
{
	m_lstAutomats.push_back(CLexAutomat(szName));
	return (*m_lstAutomats.rbegin());
}

//------------------------------------------------------------------------
bool CLexAnalyzer::Analyze(ILexSource & LexSource, ILexList & LexList, ILexTable & LexTables)
{
	//»нициируем статическую переменную, чтобы все наши  лексические единицы могли получить доступ к 
	//текущему источнику данных и проч
	//ѕри выходе из процедуры все m_pCur... автоматически сброс€тс€
	auto_set_type<ILexSource*>	_source_ptr  (m_pCurLexSource,&LexSource,NULL);
	auto_set_type<ILexList*>	_lex_list_ptr(m_pCurLexList,&LexList,NULL);
	auto_set_type<ILexTable*>	_lex_tbl_ptr (m_pCurLexTables,&LexTables,NULL);

	LexSource.MoveToBegin(); //«ачитываем первый символ
	
	structs::text_pos pos;	//ƒл€ проверки на бесконечный цикл
	int nSamePos=0;

	//- основной циикл разбора-----
	while ((!LexSource.IsEOF()) && (LexSource.GetCurSymbol().Name!=0)) 
	{
		//ѕровер€ем, есть ли текуща€ позици€ тоже самое, что и предудыщий раз
		if (pos == LexSource.GetCurPos())	nSamePos++;
		else								nSamePos=0;
		pos = LexSource.GetCurPos();

		//ѕоиск автомата дл€ разбора
		CLexAutomat* pLAU = NULL;
		for (list_automats::iterator i=m_lstAutomats.begin(); i!=m_lstAutomats.end(); i++)
			if ((*i).TryTranslate(LexSource))
			{
				pLAU = &(*i);

				CLexFunctions::Clear();	//«десь идет пр€мой вызов метода CLexFunctions
										//¬ообще мы используем три таких метода:	Clear, AddSymbol и EnableAdding

				ILexSource::state st = LexSource.StoreState();
				//≈сли разбор пройдет, цикл по поиску автомата разрываетс€
				//иначе пробуем другой автомат, пока не переберем все
				if (pLAU->Translate(LexSource))
					break;
				
				LexSource.RestoreState(st);

				//≈сли найдены ошибки - выходим (не очень красивое решение :(( )
				if (errors::is_errors())
					return false;

				pLAU = NULL; //Ёто означает, что автомата и не было, важно дл€ проверки ниже
			}

		if (!pLAU)
		{
			errors::e_illegal_symbol(LexSource.GetCurSymbol().Name,LexSource.GetCurLocation());
			return false;	/* Exit!!*/
		}
		
		//Ќе может быть так, чтобы nSamePos было столько раз одинм и тем же
		if (nSamePos>256)
		{
			errors::internal_error("Endless loop detected");
			return false;
		}
	} 

	//- проверка на размер программы -----
	if (LexList.GetCount()==0)
	{
		errors::e_source_empty(LexSource.GetFileName().c_str());
		return false;
	}
	
	//- проверка на парность скобок ------
	std::stack<char_t> bracket_stack;
	for (size_t i = 0; (i<LexList.GetCount()) && (!errors::is_errors()) ;i++)
	{
		const structs::lexeme & L=LexList.GetAt(i);
		if (!L.IsOperatorClass()) continue; //Ќам нужны только операторы дл€ проверки

		_ASSERT(L.GetSzText());
		switch (L.GetSzText()[0])
		{
		case '{':
		case '(':
		case '[':
			bracket_stack.push(L.GetSzText()[0]);
			break;
		case '}':
			if ((bracket_stack.empty()) || (bracket_stack.top()!='{'))
				errors::e_superfluous_right_brace(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));			
			else
				bracket_stack.pop();
			break;
		case ')':
			if ((bracket_stack.empty()) || (bracket_stack.top()!='('))
				errors::e_superfluous_right_bracket(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));			
			else
				bracket_stack.pop();
			break;
		case ']':
			if ((bracket_stack.empty()) || (bracket_stack.top()!='['))
				errors::e_superfluous_right_sbracket(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));
			else
				bracket_stack.pop();
			break;
		}
	}

	if (errors::is_errors())
		return false;
	
	//- проверка на парное кол-во скобок в конце текста------
	if (!bracket_stack.empty()) 
	{
		const structs::lexeme & L=LexList.GetAt(LexList.GetCount()-1);
		char_t ch=bracket_stack.top();
		switch (ch)
		{
		case '{':
			errors::e_superfluous_left_brace(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));
			return false;
		case '(':
			errors::e_superfluous_left_bracket(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));			
			return false;
		case '[':
			errors::e_superfluous_left_sbracket(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));
			return false;
		case '}':
			errors::e_superfluous_right_brace(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));
			return false;
		case ')':
			errors::e_superfluous_right_bracket(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));			
			return false;
		case ']':
			errors::e_superfluous_right_sbracket(structs::text_location(LexSource.GetFileName(),L.GetStartPos()));
			return false;
		default: 
			_ASSERT(false);
		}
	}

	return !errors::is_errors();
}
