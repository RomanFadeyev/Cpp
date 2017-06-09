/********************************************************************
	created:	2004/08/18
	created:	18:8:2004   14:14
	filename: 	X:\MultiTranslator\sources\mtdev\com\comparser.h
	file path:	X:\MultiTranslator\sources\mtdev\com
	file base:	comparser
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Declaration of the Parser (co-class of COM::IParser)
*********************************************************************/

#pragma once
#include "resource.h"       // main symbols
#include "COMInterfaces.h"

class CParser;

namespace COM
{
	//--------------------------------------------------------------------
	// GrammaticalSymbol
	// этот кокласс недоступен для создания через библиотеку
	// его можно создать только из Parser
	//--------------------------------------------------------------------
	class ATL_NO_VTABLE GrammaticalSymbol : 
		public IDispatchImpl<IGrammaticalSymbol>,
		public CComObjectRoot
	{
	private:
		_bstr_t					m_strName;
		GrammaticalSymbolClass	m_nClass;
		GrammaticalSymbolKind	m_nKind;
		_bstr_t					m_strActions;
		friend class GrammaticalRule;

		BEGIN_COM_MAP(GrammaticalSymbol)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(IGrammaticalSymbol)
		END_COM_MAP( )
	public:
		GrammaticalSymbol(): m_nClass(gscUndefined), m_nKind(gskNormal) {
		}
		STDMETHOD(get_Name)(BSTR* pVal);
		STDMETHOD(put_Name)(BSTR newVal);
		STDMETHOD(get_Class)(GrammaticalSymbolClass* pVal);
		STDMETHOD(put_Class)(GrammaticalSymbolClass newVal);
		STDMETHOD(get_Kind)(GrammaticalSymbolKind* pVal);
		STDMETHOD(put_Kind)(GrammaticalSymbolKind newVal);
		STDMETHOD(get_Actions)(BSTR* pVal);
		STDMETHOD(put_Actions)(BSTR newVal);
	};

	//--------------------------------------------------------------------
	// GrammaticalVariant
	// этот кокласс недоступен для создания через библиотеку
	// его можно создать только из Parser
	//--------------------------------------------------------------------
	class ATL_NO_VTABLE GrammaticalVariant : 
		public IDispatchImpl<IGrammaticalVariant>,
		public CComObjectRoot
	{
	private:
		typedef list<ref_cnt_ptr<GrammaticalSymbol> > list_symbols;
		list_symbols	m_lstSymbols;
		_bstr_t			m_strActions;
		friend class GrammaticalRule;

		BEGIN_COM_MAP(GrammaticalVariant)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(IGrammaticalVariant)
		END_COM_MAP( )
	public:
		GrammaticalVariant(){
		}

		STDMETHOD(AddSymbol)(IGrammaticalSymbol** pSymbol);
		STDMETHOD(get_Actions)(BSTR* pVal);
		STDMETHOD(put_Actions)(BSTR newVal);
	};

	//--------------------------------------------------------------------
	// GrammaticalRule
	// этот кокласс недоступен для создания через библиотеку
	// его можно создать только из Parser
	//--------------------------------------------------------------------
	class ATL_NO_VTABLE GrammaticalRule : 
		public IDispatchImpl<IGrammaticalRule>,
		public CComObjectRoot
	{
	private:
		typedef list<ref_cnt_ptr<GrammaticalVariant> > list_variants;
		_bstr_t			m_strName;
		list_variants	m_lstVariants;
		_bstr_t			m_strActions;
	
	protected:
		BEGIN_COM_MAP(GrammaticalRule)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(IGrammaticalRule)
		END_COM_MAP( )

		GrammaticalRule(){
		}
	public:
		STDMETHOD(get_Name)(BSTR* pVal);
		STDMETHOD(put_Name)(BSTR newVal);
		STDMETHOD(AddVariant)(IGrammaticalVariant** pVariant);
		STDMETHOD(AssembleText)(BSTR* aText);
		STDMETHOD(get_Actions)(BSTR* pVal);
		STDMETHOD(put_Actions)(BSTR newVal);
	};

	//--------------------------------------------------------------------
	// Grammar
	// этот кокласс недоступен для создания через библиотеку
	// его можно создать только из Parser
	//--------------------------------------------------------------------
	class ATL_NO_VTABLE Grammar : 
		public IDispatchImpl<IGrammar>,
		public CComObjectRoot
	{
	private:
		typedef vector<ref_cnt_ptr<GrammaticalRule> > list_rules;
		list_rules	m_lstRules;
	
	protected:
		BEGIN_COM_MAP(Grammar)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(IGrammar)
		END_COM_MAP( )

		Grammar(){
		}
	public:
		STDMETHOD(get_RuleCount)(LONG* pVal);
		STDMETHOD(get_Rules)(LONG nIndex, IGrammaticalRule** pRule);

		STDMETHOD(AddRule)(IGrammaticalRule** aGrmRule);
		STDMETHOD(AssembleText)(BSTR* aText);
	};

	//--------------------------------------------------------------------
	// Parser
	//--------------------------------------------------------------------
	[
		coclass,
		threading("apartment"),
		vi_progid("MTDev.Parser"),
		progid("MTDev.Parser.1"),
		version(1.0),
		uuid("0217A734-4550-4CD9-A8A2-F6066E201640"),
		helpstring("Parser Class")
	]
	class ATL_NO_VTABLE Parser : public IParser
	{
	private:
		CParser *m_pOwner;
	public:
		Parser():m_pOwner(NULL)
		{
		}

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct();
		void FinalRelease();
		void LinkToOwner(CParser *pOwner);

		STDMETHOD(CreateRule)(IGrammaticalRule** aGrmRule);
		STDMETHOD(CreateGrammar)(IGrammar** aGrammar);

		STDMETHOD(Parse)(BSTR pText,IGrammar** aGrammar);
		STDMETHOD(ComposeMainFunction) (BSTR RootRule,VARIANT_BOOL IgnoreCase,
			BSTR IgnorePairList, BSTR ReplacePairList, BSTR * aText);
	};

}; //of namespace