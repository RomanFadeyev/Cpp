#include "stdafx.h"
#include "BaseStructs.h"


cstr structs::lexeme::GetSzTypeName()	const
{
	return (pType) ? pType->GetTypeName() : "";
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern bool  UserBreakFlag=false;
extern char_t VALID_SYMBOLS_FOR_ID[]="qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_";
extern char_t VALID_SYMBOLS_FOR_CONST[]=".+-1234567890\"\'";

//------------------------------------------------------------------------
// structs::grm_term 
//------------------------------------------------------------------------
structs::grm_term::grm_term(const lexeme_group &LG, const sntx_error& Err,pf_sntx_func F/*=NULL*/,size_t Flags/*=R_NO_FLAGS*/):
	grm_term_t (LG,Flags)
{
	m_pFunc = F;
	SetError(Err);
}
//------------------------------------------------------------------------
structs::grm_term::grm_term(pf_sntx_func F/*=NULL*/):
	grm_term_t(GE_NULL,flagNone)
{
	m_pFunc=F;
}
//------------------------------------------------------------------------
structs::grm_term::grm_term(ETermClass nClass, pf_sntx_func F):
	grm_term_t(nClass,flagNone)
{
	m_pFunc=F;
}
//------------------------------------------------------------------------
// no Back - оптимизационный прием. ќзначает, что после распознанного слова при возникновении ошибки искать другие варианты не надо
// see define.h FORBID_BACK, PERMIT_BACK
structs::grm_term::grm_term(const structs::lexeme & Term,pf_sntx_func F/*=NULL*/, size_t Flags/*=R_NO_FLAGS*/):
	grm_term_t (Term,Flags)
{
	m_pFunc=F;
}
//------------------------------------------------------------------------
// no Back - оптимизационный прием. ќзначает, что после распознанного слова при возникновении ошибки искать другие варианты не надо
// see define.h FORBID_BACK, PERMIT_BACK
structs::grm_term::grm_term(const structs::lexeme & Term, const sntx_error& Err,pf_sntx_func F/*=NULL*/, size_t Flags/*=R_NO_FLAGS*/):
	grm_term_t (Term,Flags)
{
	m_pFunc=F;
	SetError(Err);
}
//------------------------------------------------------------------------
structs::grm_term::grm_term(cstr szNextHub,pf_sntx_func F/*=NULL*/,size_t Flags/*=R_NO_FLAGS*/):
	grm_term_t (szNextHub,Flags)
{	
	m_pFunc=F;
}
//------------------------------------------------------------------------
structs::grm_term::grm_term(ELexemeCLass clss, pf_sntx_func F/*=NULL*/,size_t Flags/*=R_NO_FLAGS*/):
	grm_term_t (clss,Flags)
{
	GetTerminal().Index=NO_INDEX;
	m_pFunc=F;
}
//------------------------------------------------------------------------
// пустой элемент. Ќужен в случае необходимости добавить несколько функций к одному элементу в правиле
// в этом случае и вставл€ютс€ пустые элементы
// первый параметр служит только дл€ отличи€ от других конструкторов
structs::grm_term::grm_term(double EmptyElementFlag,pf_sntx_func F/*=NULL*/):
	grm_term_t (GE_EMPTY,flagNone)
{
	_ASSERT(EmptyElementFlag==EMPTY_ELEMENT);
	m_pFunc=F;
}
//------------------------------------------------------------------------
structs::grm_term::grm_term(const structs::grm_term & src):
	grm_term_t (GE_NULL)

{
	*this=src;
}
//------------------------------------------------------------------------
structs::grm_term::~grm_term()						
{
	Clear();
}
//------------------------------------------------------------------------
structs::grm_term& structs::grm_term::operator=(const structs::grm_term & src)
{
	grm_term_t::operator = (src);
	m_pFunc=src.m_pFunc;
	return *this;
} 
//------------------------------------------------------------------------
bool  structs::grm_term::operator ==(const structs::grm_term & src) const
{
	return grm_term_t::operator == (src);
}
//------------------------------------------------------------------------
cstr SSntxID::GetSzTypeName()	const
{
	return pType->GetTypeName();
}
