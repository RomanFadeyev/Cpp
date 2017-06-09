/********************************************************************
	created:	2004/08/18
	created:	18:8:2004   14:13
	filename: 	X:\MultiTranslator\sources\mtdev\com\comparser.cpp
	file path:	X:\MultiTranslator\sources\mtdev\com
	file base:	comparser
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Implementation of Parser (co-class of COM::IParser)
*********************************************************************/

#include "stdafx.h"
#include "COMParser.h"
#include "Parser.h"


//------------------------------------------------------------------------
// GrammaticalSymbol
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	
	*pVal = _bstr_t((cstr)m_strName).Detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	m_strName = newVal;
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::get_Class(GrammaticalSymbolClass* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	
	*pVal = m_nClass;
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::put_Class(GrammaticalSymbolClass newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	m_nClass = newVal;
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::get_Kind(GrammaticalSymbolKind* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*pVal = m_nKind;
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::put_Kind(GrammaticalSymbolKind newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	m_nKind = newVal;
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::get_Actions(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	*pVal = _bstr_t(m_strActions).Detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalSymbol::put_Actions(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	m_strActions = newVal;
	return S_OK;
}


//------------------------------------------------------------------------
// GrammaticalVariant
//------------------------------------------------------------------------

STDMETHODIMP COM::GrammaticalVariant::AddSymbol(IGrammaticalSymbol** pSymbol)
{
	ref_cnt_ptr <GrammaticalSymbol> ptr (new CComObject<GrammaticalSymbol>);
	m_lstSymbols.push_back(ptr);
	
	*pSymbol = ptr.detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalVariant::get_Actions(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	*pVal = _bstr_t(m_strActions).Detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalVariant::put_Actions(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	m_strActions = newVal;
	return S_OK;
}


//------------------------------------------------------------------------
// GrammaticalRule
//------------------------------------------------------------------------

STDMETHODIMP COM::GrammaticalRule::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	
	*pVal = _bstr_t((cstr)m_strName).Detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalRule::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	m_strName = newVal;
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalRule::get_Actions(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	*pVal = _bstr_t(m_strActions).Detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalRule::put_Actions(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	m_strActions = newVal;
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalRule::AddVariant(IGrammaticalVariant** pVariant)
{
	ref_cnt_ptr <GrammaticalVariant> ptr (new CComObject<GrammaticalVariant>);
	m_lstVariants.push_back(ptr);
	
	*pVariant = ptr.detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::GrammaticalRule::AssembleText(BSTR* aText)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	IParserLink * pParser = singleton_cache::GetParserLink();
	IParserLink::syntax_info SInfo;

	//Добавляем правило
	IParserLink::syntax_item & SItemRule = SInfo.m_Rules.AddUnit();
	SItemRule.strName=(cstr)m_strName;
	SItemRule.strActions = m_strActions;

	//Заполняем правило
	for	(list_variants::iterator i=m_lstVariants.begin(); i!=m_lstVariants.end();++i)
	{
		//Добавляем вариант
		IParserLink::syntax_item & SVariant = SItemRule.AddUnit();
		COM::GrammaticalVariant * CurVariant=(*i);
		SVariant.strActions = CurVariant->m_strActions;

		for	(GrammaticalVariant::list_symbols::iterator i=CurVariant->m_lstSymbols.begin(); 
			 i!=CurVariant->m_lstSymbols.end(); ++i)
		{
			//Добавляем символ
			IParserLink::syntax_item & STerm = SVariant.AddUnit();
			STerm.strName=string_adapter((*i)->m_strName).c_str();
			STerm.nSymbolClass = (IParserLink::syntax_item::symbol_class) (*i)->m_nClass;
			STerm.nSymbolKind = (IParserLink::syntax_item::symbol_kind) (*i)->m_nKind;
			STerm.strActions = (*i)->m_strActions;
		}
	}

	//Отправляем данные парсеру - он знает формат оформления текста
	string_smart strText;
	pParser->AssembleToText(SInfo,strText);
	
	*aText = _bstr_t(strText.c_str()).Detach();
	return S_OK;
}

//------------------------------------------------------------------------
// Grammar
//------------------------------------------------------------------------
STDMETHODIMP COM::Grammar::get_RuleCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	*pVal = m_lstRules.size();

	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::Grammar::get_Rules(LONG nIndex, IGrammaticalRule** pRule)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	

	*pRule = ref_cnt_ptr <GrammaticalRule>(m_lstRules[nIndex]).detach();	
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::Grammar::AddRule(IGrammaticalRule** aGrmRule)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::Grammar::AssembleText(BSTR* aText)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	string_smart s;
	
	for (size_t i=0; i<m_lstRules.size();++i) {
		_bstr_t ss; m_lstRules[i]->AssembleText(ss.GetAddress());
		s.cat(ss);
	}

	*aText = _bstr_t(s.c_str()).Detach();
	return S_OK;
}

//------------------------------------------------------------------------
// Parser
//------------------------------------------------------------------------
void COM::Parser::LinkToOwner(CParser *pOwner)
{
	_ASSERT(pOwner!=NULL);

	m_pOwner = pOwner;
	m_pOwner->m_pCoClass=this;
}
//------------------------------------------------------------------------
HRESULT	COM::Parser::FinalConstruct()
{
	return S_OK;
}
//------------------------------------------------------------------------
void COM::Parser::FinalRelease()
{
	if (m_pOwner!=NULL){
		m_pOwner->m_pCoClass = NULL;
		m_pOwner=NULL; 
	}
}
//------------------------------------------------------------------------
STDMETHODIMP COM::Parser::CreateRule(IGrammaticalRule** aRule)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*aRule = ref_cnt_ptr <GrammaticalRule> (new CComObject<GrammaticalRule>).detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::Parser::CreateGrammar(IGrammar** aGrammar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	*aGrammar = ref_cnt_ptr <Grammar> (new CComObject<Grammar>).detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::Parser::Parse(BSTR pText,IGrammar** aGrammar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	IParserLink * pParser = singleton_cache::GetParserLink();
	IParserLink::syntax_info SInfo;
	pParser->ParseSourceText(string_smart(pText),"",SInfo,false);

	ref_cnt_ptr <Grammar> TheGrammar(new CComObject<Grammar>);

	for (IParserLink::sir_iterator j=SInfo.GetBeginPos();j!=SInfo.GetEndPos();j++)
		for (IParserLink::si_iterator i=SInfo.GetRoot(j).GetFirstUnitPos();!SInfo.GetRoot(j).IsEndUnitPos(i);) 
		{
			IParserLink::syntax_item & SItem = SInfo.GetRoot(j).GetNextUnit(i);
			IGrammaticalRule * pRule; TheGrammar->AddRule(&pRule);
			pRule->put_Name(_bstr_t(SItem.strName.c_str()));
			
			//Перебираем варианты
			
			pRule->Release();
		}
		

	*aGrammar = TheGrammar.detach();
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP COM::Parser::ComposeMainFunction (BSTR RootRule, VARIANT_BOOL IgnoreCase, BSTR IgnorePairList, BSTR ReplacePairList, BSTR * pText)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	
	string_list strIgnorePairList  = string_smart(IgnorePairList);
	string_list strReplacePairList = string_smart(ReplacePairList);

	list<string_pair> lstIgnorePair;
	if (strIgnorePairList.items().count()>=2)
		for (size_t i=0; i<strIgnorePairList.items().count();i+=2)
			lstIgnorePair.push_back(string_pair(strIgnorePairList.items()[i], strIgnorePairList.items()[i+1]));

	list<string_pair> lstReplacePair;
	if (strReplacePairList.items().count()>=2)
		for (size_t i=0; i<strReplacePairList.items().count();i+=2)
			lstReplacePair.push_back(string_pair(strReplacePairList.items()[i], strReplacePairList.items()[i+1]));

	string_smart s = m_pOwner->ComposeMainFunction(_bstr_t(RootRule),IgnoreCase!=FALSE,lstIgnorePair, lstReplacePair);

	*pText = _bstr_t(s.c_str()).Detach();
	return S_OK;
}

