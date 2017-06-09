/********************************************************************
created:	2003/12/24
created:	24:12:2003   19:38
filename: 	$(Projects)\sources\grm language generator\cpp\cpp.cpp
file path:	$(Projects)\sources\grm language generator\cpp
file base:	cpp
file ext:	cpp
author:		Fadeyev

purpose:	C++ code generator. Main file
*********************************************************************/

#include "stdafx.h"
#include "CPP.h"
#include "Resource.h"
#include <ZIP.h>
#include <ModuleNames.h>
#include <GrmExecExports.h>

//------------------------------------------------------------------------
class CppModule: public CAtlModule
{
	virtual HRESULT AddCommonRGSReplacements(IRegistrarBase* /*pRegistrar*/) throw()
	{
		return S_OK;
	}
};
CppModule Module;
HMODULE g_hModule;

//------------------------------------------------------------------------
//Dll Entry point. 
BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule=(HMODULE)hModule; break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL WINAPI grm_lang_gen_exports::Initialize(OUT ILanguageCodeGenerator* & pIface)
{
	pIface = new CComObject<CppLanguageCodeGenerator>;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CppLanguageCodeGenerator
//////////////////////////////////////////////////////////////////////////

CppLanguageCodeGenerator::~CppLanguageCodeGenerator()
{
}

STDMETHODIMP CppLanguageCodeGenerator::GetProperties(/*[out]*/ BSTR* pName,
													 /*[out]*/ BSTR* pDescription)
{
	*pName		  = _bstr_t("MS VCPP 70").Detach(); 
	*pDescription = _bstr_t("Creates source files for C++ language (Microsoft Visual C++)").Detach();
	return S_OK;
};

//------------------------------------------------------------------------
STDMETHODIMP CppLanguageCodeGenerator::OnGrmItem (/* [in] */ BSTR RuleName,
												  /* [in] */ BSTR VariantName,
												  /* [in] */ BSTR TermName,
												  /* [in] */ ULONG ItemID)
{
	//Get Rule 
	grm_rule* & pRule = m_mapRules[RuleName];
	if (!pRule){
		m_lstRules.push_back(grm_rule(_bstr_t(RuleName)));
		pRule = &m_lstRules.back();
	}
	
	//Get Variant
	grm_variant* & pVariant = pRule->m_mapVariants[VariantName];
	if (!pVariant){
		pRule->m_lstVariants.push_back(grm_variant(_bstr_t(VariantName)));
		pVariant = &pRule->m_lstVariants.back();
	}

	//Get Term
	grm_term* & pTerm = pVariant->m_mapTerms[TermName];
	if (!pTerm){
		pVariant->m_lstTerms.push_back(grm_term(_bstr_t(TermName)));
		pTerm = & pVariant->m_lstTerms.back();
	}
	pTerm->m_nID = ItemID;

	return S_OK;
}

//------------------------------------------------------------------------
STDMETHODIMP CppLanguageCodeGenerator::Begin(/*[in] */BSTR WorkDir, 
											 /*[in] */BSTR ProjectPath)
{
	m_strWorkDir	 = include_backslash(WorkDir);
	m_strProjectPath = _bstr_t(ProjectPath);

	string_smart strFileName(m_strWorkDir,"main.cpp");
	if (!m_MainFile.open(strFileName,"wt"))
		return E_FAIL;

	// Идея: всовываем в виде ресурса ZIP-файл в нашу DLL
	// Затем при генерации достаем и распаковываем

	// Не знаю почему. но это работает только при 
	// Use MFC in staticLibrary
	HRSRC hSFXRes=FindResource(g_hModule,(char_t*)IDR_TEMPLATE,"TEMPLATE");
	_ASSERT(hSFXRes);
	HGLOBAL MemArea= LoadResource(g_hModule,hSFXRes);
	_ASSERT(MemArea);
	size_t   ResSize= SizeofResource(g_hModule,hSFXRes);
	_ASSERT(ResSize>0x100);
	char_t * RawData= (char_t*) LockResource(MemArea);
	_ASSERT(RawData);

	//Распаковываем
	string_smart strZipFileName = get_temp_filename(".dat");
	base_utils::file_std TemplateFile;
	if (!TemplateFile.open(strZipFileName,"wb"))
		return E_FAIL;

	TemplateFile.write(RawData,ResSize);
	zip::zip_operation Zip(false);
	TemplateFile.close();

	if (!Zip.do_unzip(strZipFileName,m_strWorkDir))
		return E_FAIL;
	
	return S_OK;
}
//------------------------------------------------------------------------
STDMETHODIMP CppLanguageCodeGenerator::End()
{
	if (!m_MainFile.isopen()) return S_OK; //Important. We might not open the session

	string_smart strText1; 
	string_smart strText2; 
	
	strText1 =	"\n"\
				"void WINAPI OnItem(size_t nID, const char* szText, void * pTranslator)\n{\n"\
				"  switch (nID)\n  {";
	strText2 = 	"\nstatic void report(LPCTSTR szRule, LPCTSTR szVariant, LPCTSTR szTerm, LPCTSTR szText)\n"\
				"{\n"\
				"  cout<<szRule<<_T(\":\")<<szVariant<<_T(\":\")<<szTerm<<_T(\" -> \")<<szText<<endl;\n"\
				"}\n\n"\
				"#define REPORT(rule,variant,term,text) report(rule,variant,term,text)\n";

	for (list_rules::iterator i=m_lstRules.begin(); i!=m_lstRules.end(); ++i)
	{
		grm_rule & Rule =(*i);
		string_smart strRule=str_convert_to_ident(Rule.m_strName);

		strText2.cat("\n\n//rule ").cat(Rule.m_strName);
		strText2.cat("\nnamespace ").cat(strRule).cat("\n{");
		
		for (grm_rule::list_variants::iterator i=Rule.m_lstVariants.begin(); i!=Rule.m_lstVariants.end(); ++i)
		{
			grm_variant & Variant =(*i);
			string_smart strVariant = str_convert_to_ident(string_smart("variant_",Variant.m_strName));

			strText2.cat("\n  namespace ").cat(strVariant).cat("\n  {");
			
			for (grm_variant::list_terms::iterator i=Variant.m_lstTerms.begin(); i!=Variant.m_lstTerms.end(); ++i)
			{
				grm_term & Term = (*i);
				string_smart strTerm = str_convert_to_ident(Term.m_strName);

				strText1.cat("\n    case 0x").cat(itos((int)Term.m_nID,16)).cat(" : ").cat(strRule).cat("::").cat(strVariant).cat("::").cat(strTerm).cat("(szText,pTranslator); break;");
				strText2.cat("\n    void ").cat(strTerm).cat("(const char* szText, void * pTranslator) /*").cat((*i).m_strName).cat("*/{");
				strText2.cat("\n      REPORT(_T(\"").cat(Rule.m_strName).cat("\"),_T(\"").cat(Variant.m_strName).cat("\"),_T(\"").cat(Term.m_strName).cat("\"), szText);\n");
				strText2.cat("\n    }");
			}
			strText2.cat("\n  }");
		}
		strText2.cat("\n}");
	}
	strText2.cat("\n");

	m_MainFile.write_strln("#include \"stdafx.h\"");
	m_MainFile.write_strln("#include <iostream>");
	m_MainFile.write_strln("using namespace std;\n");

	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln("// Term particular functions");
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile<<strText2;

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln("// Term selector");
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile<<strText1<<"\n  }\n}\n\n";

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln("// Start function");
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("typedef void (WINAPI *pf_ParseCallBack) (size_t nID, LPCTSTR szText, void * pTranslator);");	
	m_MainFile.write_strln("typedef long (WINAPI *pf_Execute) (LPCTSTR szFileName, LPCTSTR szParams, pf_ParseCallBack pCallBack, int &iExitCode);");

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("enum err_code {");
	m_MainFile.write_strln(Format("  ecOK                   =%d, //%s",recOK,ERunnerExitCodes_Str(recOK)));
	m_MainFile.write_strln(Format("  ecNEED_PARAMS          =%d, //%s",recNEED_PARAMS,ERunnerExitCodes_Str(recNEED_PARAMS)));
	m_MainFile.write_strln(Format("  ecCANT_LOAD_LIB        =%d, //%s",recCANT_LOAD_LIB,ERunnerExitCodes_Str(recCANT_LOAD_LIB)));
	m_MainFile.write_strln(Format("  ecCANT_LOAD_CODE       =%d, //%s",recCANT_LOAD_CODE,ERunnerExitCodes_Str(recCANT_LOAD_CODE)));
	m_MainFile.write_strln(Format("  ecWRONG_FORMAT         =%d, //%s",recWRONG_FORMAT,ERunnerExitCodes_Str(recWRONG_FORMAT)));
	m_MainFile.write_strln(Format("  ecWRONG_SFX_FORMAT     =%d, //%s",recWRONG_SFX_FORMAT,ERunnerExitCodes_Str(recWRONG_SFX_FORMAT)));
	m_MainFile.write_strln(Format("  ecNO_LOADED_CODE       =%d, //%s",recNO_LOADED_CODE,ERunnerExitCodes_Str(recNO_LOADED_CODE)));
	m_MainFile.write_strln(Format("  ecABNORMAL_TERMINATION =%d, //%s",recABNORMAL_TERMINATION,ERunnerExitCodes_Str(recABNORMAL_TERMINATION)));
	m_MainFile.write_strln(Format("  ecFORCED_TERMINATION   =%d  //%s",recFORCED_TERMINATION,ERunnerExitCodes_Str(recFORCED_TERMINATION)));
	m_MainFile.write_strln("};\n");

	m_MainFile.write_strln("bool WINAPI start(LPCTSTR szFilePath,LPCTSTR szCommandLine)\n{");
	m_MainFile<<		   "  HMODULE hLib = ::LoadLibrary(_T(\""<<module_names::szDebuggerName<<"\"));\n"; 
	m_MainFile.write_strln("  if (hLib==NULL) {");
	m_MainFile.write_strln("    cout<<_T(\"Cannot load library\");");
	m_MainFile.write_strln("    return false;");
	m_MainFile.write_strln("  }");
	m_MainFile<<		   "  pf_Execute pFunc = (pf_Execute)::GetProcAddress(hLib,_T(\""<<grmexec_exports::szf_Execute<<"\"));\n";
	m_MainFile.write_strln("  if (pFunc==NULL) {");
	m_MainFile.write_strln("    cout<<_T(\"Cannot load library\");");
	m_MainFile.write_strln("    return false;");
	m_MainFile.write_strln("  }");
	m_MainFile.write_strln("  int nProcessExitCode;");
	m_MainFile<<		   "  return pFunc(szFilePath,szCommandLine, OnItem,nProcessExitCode)==ecOK;\n";
	m_MainFile.write_strln("}");

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln("// main function");
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("int _tmain(int argc, _TCHAR* argv[])");
	m_MainFile.write_strln("{");
	m_MainFile.write_strln("  cout<<_T(\"Input path to file for translation:\\n\");");
	m_MainFile.write_strln("  TCHAR szPath[MAX_PATH];");
	m_MainFile.write_strln("  cin>>szPath;");
	string_smart strModulePath = get_relative_path(m_strProjectPath,m_strWorkDir);
	strModulePath.replace("\\","\\\\");

	m_MainFile<<		   "  start(_T(\""<<strModulePath.c_str()<<"\"),szPath);"<<"\n";
	m_MainFile.write_strln("  return 0;");
	m_MainFile.write_strln("};");

	m_MainFile.close();

	return S_OK;
}
