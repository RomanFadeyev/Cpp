/********************************************************************
	created:	2004/01/06
	created:	6:1:2004   16:51
	filename: 	$(Projects)\sources\grm language generator\delphi\delphi.cpp
	file path:	$(Projects)\sources\grm language generator\delphi
	file base:	delphi
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Object Pascal code generator. Main file
*********************************************************************/

#include "stdafx.h"
#include "Delphi.h"
#include "Resource.h"
#include <ZIP.h>
#include <ModuleNames.h>
#include <GrmExecExports.h>

//------------------------------------------------------------------------
class DelphiModule: public CAtlModule
{
	virtual HRESULT AddCommonRGSReplacements(IRegistrarBase* /*pRegistrar*/) throw()
	{
		return S_OK;
	}
};
DelphiModule Module;
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
	pIface = new CComObject<DelphiLanguageGenerator>;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// DelphiLanguageGenerator
//////////////////////////////////////////////////////////////////////////

DelphiLanguageGenerator::~DelphiLanguageGenerator()
{
}

STDMETHODIMP DelphiLanguageGenerator::GetProperties(/*[out]*/ BSTR* pName,
													 /*[out]*/ BSTR* pDescription)
{
	*pName		  = _bstr_t("Borland Delphi").Detach(); 
	*pDescription = _bstr_t("Creates the source files for Object Pascal language (Borland Delphi, ver 5.0, 6.0, 7.0)").Detach();
	return S_OK;
};

//------------------------------------------------------------------------
STDMETHODIMP DelphiLanguageGenerator::OnGrmItem (/* [in] */ BSTR RuleName,
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
STDMETHODIMP DelphiLanguageGenerator::Begin(/*[in] */BSTR WorkDir, 
											 /*[in] */BSTR ProjectPath)
{
	m_strWorkDir	 = include_backslash(WorkDir);
	m_strProjectPath = _bstr_t(ProjectPath);

	string_smart strFileName(m_strWorkDir,"main.pas");
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
STDMETHODIMP DelphiLanguageGenerator::End()
{
	if (!m_MainFile.isopen()) return S_OK;; //Important. We might not open the session

	string_smart strText1; 
	string_smart strText2; 
	
	strText1 =	"\n"\
				"procedure OnItem(nID: DWORD; szText: pchar; pTranslator: pointer); stdcall;\n"\
				"begin\n"\
				"  case (nID) of\n";

	strText2 = 	"\nprocedure report(szRule: pchar; szVariant: pchar; szTerm: pchar; szText: pchar);\n"\
				"begin\n"\
				"  writeln(szRule,':',szVariant,':',szTerm,' -> ',szText);\n"\
				"end;\n\n";

	for (list_rules::iterator i=m_lstRules.begin(); i!=m_lstRules.end(); ++i)
	{
		grm_rule & Rule =(*i);
		string_smart strRule=str_convert_to_ident(Rule.m_strName);

		strText2.cat("\n\n//rule ").cat(Rule.m_strName);
		
		for (grm_rule::list_variants::iterator i=Rule.m_lstVariants.begin(); i!=Rule.m_lstVariants.end(); ++i)
		{
			grm_variant & Variant =(*i);
			string_smart strVariant = str_convert_to_ident(string_smart("variant_",Variant.m_strName));

			for (grm_variant::list_terms::iterator i=Variant.m_lstTerms.begin(); i!=Variant.m_lstTerms.end(); ++i)
			{
				grm_term & Term = (*i);
				string_smart strTerm = str_convert_to_ident(Term.m_strName);

				strText1.cat("\n    $").cat(itos((int)Term.m_nID,16)).cat(" : ").cat(strRule).cat("_").cat(strVariant).cat("_").cat(strTerm).cat("(szText,pTranslator);");
				
				strText2.cat("\nprocedure ").cat(strRule).cat("_").cat(strVariant).cat("_").cat(strTerm).cat("(szText: pchar; pTranslator: pointer); //").cat((*i).m_strName);
				strText2.cat("\nbegin");
				strText2.cat("\n  report('").cat(Rule.m_strName).cat("','").cat(Variant.m_strName).cat("','").cat(Term.m_strName).cat("', szText);");
				strText2.cat("\nend;\n");
			}
		}
	}
	strText2.cat("\n");

	m_MainFile.write_strln("unit main;\n");
	m_MainFile.write_strln("interface");
	m_MainFile.write_strln("uses SysUtils,Windows,Classes;\n");
	m_MainFile.write_strln("function start: boolean;\n");
	m_MainFile.write_strln("implementation\n");

	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln("// Term particular functions");
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile<<strText2;

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln("// Term selector");
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile<<strText1<<"\n  end;\nend;\n\n";

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln("// Start function");
	m_MainFile.write_strln(strnchar('/',80));
	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("type pf_ParseCallBack = procedure (nID: DWORD; szText: pchar; pTranslator: pointer); stdcall;"); 
	m_MainFile.write_strln("type pf_Execute = function (szFileName,szParams: pchar; pCallBack: pf_ParseCallBack; out iExitCode: integer): integer; stdcall;");

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln(Format("const  ecOK                   =%d; //%s",recOK,ERunnerExitCodes_Str(recOK)));
	m_MainFile.write_strln(Format("const  ecNEED_PARAMS          =%d; //%s",recNEED_PARAMS,ERunnerExitCodes_Str(recNEED_PARAMS)));
	m_MainFile.write_strln(Format("const  ecCANT_LOAD_LIB        =%d; //%s",recCANT_LOAD_LIB,ERunnerExitCodes_Str(recCANT_LOAD_LIB)));
	m_MainFile.write_strln(Format("const  ecCANT_LOAD_CODE       =%d; //%s",recCANT_LOAD_CODE,ERunnerExitCodes_Str(recCANT_LOAD_CODE)));
	m_MainFile.write_strln(Format("const  ecWRONG_FORMAT         =%d; //%s",recWRONG_FORMAT,ERunnerExitCodes_Str(recWRONG_FORMAT)));
	m_MainFile.write_strln(Format("const  ecWRONG_SFX_FORMAT     =%d; //%s",recWRONG_SFX_FORMAT,ERunnerExitCodes_Str(recWRONG_SFX_FORMAT)));
	m_MainFile.write_strln(Format("const  ecNO_LOADED_CODE       =%d; //%s",recNO_LOADED_CODE,ERunnerExitCodes_Str(recNO_LOADED_CODE)));
	m_MainFile.write_strln(Format("const  ecABNORMAL_TERMINATION =%d; //%s",recABNORMAL_TERMINATION,ERunnerExitCodes_Str(recABNORMAL_TERMINATION)));
	m_MainFile.write_strln(Format("const  ecFORCED_TERMINATION   =%d; //%s",recFORCED_TERMINATION,ERunnerExitCodes_Str(recFORCED_TERMINATION)));

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("function AnsiToOem(const s: string): string;");
	m_MainFile.write_strln("begin");
	m_MainFile.write_strln("  result:=Copy(s,1,High(integer));");
	m_MainFile.write_strln("  Windows.AnsiToOem(pchar(s),pchar(result));");
	m_MainFile.write_strln("end;");

	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("function start: boolean;");
	m_MainFile.write_strln("var");
	m_MainFile.write_strln("  szPath : string;");
	m_MainFile.write_strln("  hLib   : HMODULE;"); 
	m_MainFile.write_strln("  pFunc  : pf_Execute;"); 
	m_MainFile.write_strln("  nProcessExitCode: integer;");
	m_MainFile.write_strln("begin");
	m_MainFile.write_strln("  result:=false;");
	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("  writeln('Input path to file for translation:');");
	m_MainFile.write_strln("  readln(szPath);");

	string_smart strModulePath = get_relative_path(m_strProjectPath,m_strWorkDir);

	m_MainFile<<		   "  hLib := LoadLibrary('"<<module_names::szDebuggerName<<"');\n"; 
	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("  if hLib=0 then");
	m_MainFile.write_strln("  begin");
	m_MainFile.write_strln("    writeln('Cannot load library. Error: \"',AnsiToOem(SysErrorMessage(GetLastError)),\'\"\');");
	m_MainFile.write_strln("    exit;");
	m_MainFile.write_strln("  end;");
	m_MainFile.write_strln(STR_NULL);
	m_MainFile<<		   "  pFunc := GetProcAddress(hLib,'"<<grmexec_exports::szf_Execute<<"');\n";
	m_MainFile.write_strln("  if not Assigned(pFunc) then");
	m_MainFile.write_strln("  begin");
	m_MainFile.write_strln("    writeln('Cannot load library');");
	m_MainFile.write_strln("    exit;");
	m_MainFile.write_strln("  end;");
	m_MainFile.write_strln(STR_NULL);
	m_MainFile.write_strln("  try");
	m_MainFile<<		   "    result:=pFunc('"<<strModulePath<<"',pchar(szPath), OnItem,nProcessExitCode)=ecOK;\n";
	m_MainFile.write_strln("  finally");
	m_MainFile.write_strln("  FreeLibrary(hLib);");
	m_MainFile.write_strln("  end");   
	m_MainFile.write_strln("end;");
	m_MainFile.write_strln("\nend.");

	m_MainFile.close();
	return S_OK;
}
