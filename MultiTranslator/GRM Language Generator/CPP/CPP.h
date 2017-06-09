/********************************************************************
	created:	2003/12/30
	created:	30:12:2003   23:04
	filename: 	$(Projects)\sources\grm language generator\cpp\cpp.h
	file path:	$(Projects)\sources\grm language generator\cpp
	file base:	cpp
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include <BaseUtils.h>
#include <map>
#include "..\..\GrmLangGenExports.h"
using namespace base_utils;

//------------------------------------------------------------------------
//Main interface implementation. Each CodeGenerator has to has this interface
//The compiler calls CodeGenerator via this interface
//The consequence of calls: get_Description, BeginDir, OnGrmItem

class ATL_NO_VTABLE CppLanguageCodeGenerator: 
	public CComObjectRoot,
	public ILanguageCodeGenerator
{

	struct grm_term{
		size_t		m_nID;
		string_std	m_strName;
		grm_term(cstr szName): m_strName(szName){}
	};

	struct grm_variant{
		typedef list<grm_term> list_terms;
		typedef	map<string_std,grm_term*> map_terms;
		map_terms	m_mapTerms;
		list_terms  m_lstTerms;
		string_std	m_strName;
		
		grm_variant(cstr szName): m_strName(szName){}
	};

	struct grm_rule{
		typedef list <grm_variant> list_variants;
		typedef map  <string_std,grm_variant*> map_variants;
		map_variants  m_mapVariants;
		list_variants m_lstVariants;
		string_std	  m_strName;
		
		grm_rule(cstr szName): m_strName(szName){}
	};
private:
	string_smart	m_strWorkDir;
	string_smart	m_strProjectPath;

	typedef map<string_std,grm_rule*> map_rules;
	typedef list<grm_rule> list_rules;
	map_rules		m_mapRules;
	list_rules		m_lstRules;
	file_std		m_MainFile;
protected:
	//Is called when the comopiler or IDE tries to determine the properties
	//of generator
	STDMETHOD (GetProperties) (	/*[out]*/ BSTR* pName,
								/*[out]*/ BSTR* pDescription);

	//Is called each time while compiler determines new grammatical item
	STDMETHOD(OnGrmItem) (	/* [in] */ BSTR RuleName,
							/* [in] */ BSTR VariantName,
							/* [in] */ BSTR TermName,
							/* [in] */ ULONG ItemID);

	//Set work dir
	STDMETHOD (Begin)(	/*[in] */BSTR WorkDir,
						/*[in]*/ BSTR ProjectPath);

	STDMETHOD (End)();

	//Destructor
	override ~CppLanguageCodeGenerator();

	BEGIN_COM_MAP(CppLanguageCodeGenerator)	
		COM_INTERFACE_ENTRY(ILanguageCodeGenerator)
	END_COM_MAP()
public:
};
