/********************************************************************
	created:	2003/06/29
	created:	29:6:2003   22:21
	filename: 	$(Projects)\sources\grm compiler\compiler.cpp
	file path:	$(Projects)\sources\grm compiler
	file base:	compiler
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include <time.h>
#include <OutStreams.h>
#include <MiscUtils.h>
#include <AppUtils.h>
#include <GrmcExports.h>

#include "compiler.h"
#include "EWQueue.h"
#include "TypesTable.h"
#include "RulesTable.h"
#include "DLLSupport.h"
#include "InitCompiler.h"

#include "LexTables.h"
#include "LexSource.h"
#include "LexList.h"
#include "LexAnalyzer.h"
#include "LexFunctions.h"

#include "SntxList.h"
#include "SntxTables.h"
#include "SntxAnalyzer.h"
#include "Functions.h"

#include "Preprocessor.h"
#include "SmntAnalyzer.h"
#include "ImportTable.h"
#include "LangGenTable.h"
#include "PCode.h"
#include "Linker.h"

using namespace out_streams;
using namespace misc_utils;

#define _SHOW_TIME

//#ifdef _DEBUG
	#define PROCESS_REPORT(x)	out_streams::output(x)
//#else
//	#define PROCESS_REPORT(x)	true
//#endif

//------------------------------------------------------------------------
//Контекст процесса компиляции одного исходного модуля
struct CModuleContext{
	string_smart		m_strFileName;
	CLexSource			m_LexSource;
	
	CLexTable			m_LexTable;
	CSntxTable			m_SntxTable;
	CTypesTable			m_TypesTable;
	CRulesTable			m_RulesTable;

	CLexList			m_LexList;
	CSntxList			m_SntxList;
	CPCodeList			m_PCodeList;	//Должен быть последним, тогда в декструторе он разрушится первым

	CPreprocessor		m_Preprocessor;
	CLexAnalyzer 		m_LexAnalyzer;		
	CSntxAnalyzer 		m_SntxAnalyzer;		
	CSmntAnalyzer 		m_SmntAnalyzer;		

	const IBuildContext &	m_BuildContext;
	CModuleContext(const IBuildContext & BuildContext, cstr szFileName, CLinker & Linker):m_strFileName(szFileName),m_LexList(BuildContext,szFileName),m_PCodeList(BuildContext,szFileName),
		m_BuildContext(BuildContext), m_LexTable(BuildContext,szFileName),m_SntxAnalyzer(BuildContext,szFileName),m_SmntAnalyzer(BuildContext,szFileName),m_Preprocessor(BuildContext)
	{
		init_compiler::InitLexTable(m_LexTable);

		init_compiler::InitLexAnalyzer(m_LexAnalyzer);
		init_compiler::InitSntxAnalyzer(m_SntxAnalyzer,m_LexTable);

		init_compiler::InitTypesTable(m_TypesTable,m_LexTable);
		init_compiler::InitStdVariables(m_LexTable,m_SntxTable,m_TypesTable,Linker);
	}
	~CModuleContext(){
		LOG_PROTECT_CODE_BEGIN
		m_LexAnalyzer.Clear();		//В принципе, это все выполняется в автоматическом
		m_SntxAnalyzer.Clear();		//режиме, но, очищая руками, мы можем сдержать
		m_SmntAnalyzer.Clear();		//исключения
		m_PCodeList.Clear();
		m_SntxList.Clear();
		m_LexList.Clear();
		LOG_PROTECT_CODE_END
	}
};
//------------------------------------------------------------------------
class CModuleContextList: public std::list <CModuleContext*>
{
public:
	~CModuleContextList(){
		//Очищаем список модулей 
		for (iterator i=begin();i!=end();++i)
			delete (*i);
	}
};

//------------------------------------------------------------------------
//Компиляция одного файла, без линковки
grmc_exports::exit_code CompileModule (CModuleContext & Context, IImportTable & ImportTable, const ILangGenTable & LangGenTable,const build_props & props)
{
	#define CHECK_USER_BREAK if (UserBreakFlag) \
								{errors::e_user_break(); \
								 return grmc_exports::cecUSER_BREAK;}

	errors::ew_auto_output error_print;	//При выходе из данной процедуры весь 
										//набор ошибок выведется на экран

	out_streams::output("\n",extract_filenameext(Context.m_strFileName.c_str()).c_str());

	if (!is_file_exist(Context.m_strFileName.c_str()))
	{
		errors::e_file_not_found(Context.m_strFileName.c_str());
		return grmc_exports::cecINIT_FAILED;
	}

//- Препроцессор  ------------
	PROCESS_REPORT("\nPreprocessing:");
	string_smart strText;
	if (!Context.m_Preprocessor.Analyze(Context.m_strFileName.c_str(),strText)){
		if (!errors::is_errors())
			errors::e_preprocessor_error(Context.m_strFileName.c_str());
		return grmc_exports::cecCOMPILE_FAILED;
	}
	CHECK_USER_BREAK
	PROCESS_REPORT(" OK");

	
//- Лексический разбор ------------
	errors::clear_ew();
	PROCESS_REPORT("\nLexical analysis:");
	if (!Context.m_LexSource.LoadFromString(Context.m_strFileName.c_str(),strText))
	{
		errors::internal_error("Could not load preprocessed source");
		return grmc_exports::cecINIT_FAILED;
	}

	if (!Context.m_LexAnalyzer.Analyze(Context.m_LexSource,Context.m_LexList,Context.m_LexTable))
	{
		if (!errors::is_errors())
			errors::e_unknown_error(Context.m_LexSource.GetCurLocation());
		
		//Все, уходим, так как ест ошибки
		return grmc_exports::cecCOMPILE_FAILED;
	}
	_ASSERT(!errors::is_errors());
	CHECK_USER_BREAK
	if (!errors::is_ew()) PROCESS_REPORT(" OK");

//- разбор грамматик ---------
	errors::clear_ew();
	PROCESS_REPORT("\nSyntactic analysis:");

	if (!Context.m_SntxAnalyzer.Analyze(Context.m_LexList,Context.m_SntxList))
		return grmc_exports::cecCOMPILE_FAILED;

	_ASSERT(!errors::is_errors());
	CHECK_USER_BREAK
	if (!errors::is_ew()) PROCESS_REPORT(" OK");
	

	if (!props.bSyntaxCheck)
	{
	//- семантический разбор ------
		errors::clear_ew();
		PROCESS_REPORT("\nGeneration:");

		if (!Context.m_SmntAnalyzer.Analyze(Context.m_LexTable, ImportTable, LangGenTable, Context.m_TypesTable,Context.m_SntxTable, Context.m_LexList,Context.m_SntxList,Context.m_PCodeList))
			return grmc_exports::cecCOMPILE_FAILED;

		_ASSERT(!errors::is_errors());
		CHECK_USER_BREAK
		if (!errors::is_ew()) PROCESS_REPORT(" OK");


	//- оптимизация ----------
		errors::clear_ew();
		PROCESS_REPORT("\nOptimization:");
		Context.m_PCodeList.Optimize();

		if (errors::is_errors()) 
			return grmc_exports::cecCOMPILE_FAILED;
	}

	if (!errors::is_ew()) PROCESS_REPORT(" OK");
	return grmc_exports::cecOK;
}

//------------------------------------------------------------------------
// Build
//------------------------------------------------------------------------

static grmc_exports::exit_code BuildInternal(const build_props & props)
{
	if (!LoadGrammsDLL())
		return grmc_exports::cecINIT_FAILED;

#ifdef _DEBUG
	version_info info(NULL);
	out_streams::output("\nIt's DEBUG version. ",info.get_FileVersion().c_str(),". ");
#endif
	if (props.bSyntaxCheck)
		out_streams::output("\nSyntax check.");

	if (props.lstSources.items().count()==0)
	{
		out_streams::output("there are no source files\n");
		return grmc_exports::cecINIT_FAILED;
	}
	
	///////////// Here're all LexAutomats are declared ///
	CLinker			Linker(props);
	CImportTable	ImportTable;
	CLangGenTable	LangGenTable(props.strResultFileName.c_str());

#ifdef _SHOW_TIME
	clock_t start, finish;
	start=clock();
#endif

	TIME_REPORT("\nTime (start init): ",clock());
	
	//out_streams::outs<<  "-------------- Building project ... --------------";
	//out_streams::outs<<out_streams::endl;

	if (!props.bSyntaxCheck)
	{
		if (!props.strInputTranslationDir.isempty())
			out_streams::output("\nInput direction = ",props.strInputTranslationDir.c_str());
		if (!props.strOutputTranslationDir.isempty())
			out_streams::output("\nOutput direction = ",props.strOutputTranslationDir.c_str());

		//Загружаем библиотеки импорта
		for (int i=0;i<props.lstLibraries.items().count();i++){
			if (!ImportTable.AddLibary(props.lstLibraries.items()[i])){
				errors::e_file_not_found(props.lstLibraries.items()[i]);
				return grmc_exports::cecINIT_FAILED;
			}
		}

		//Загружаем генераторы кода на разл ЯП
		for (size_t i=0;i<props.lstLangGenerators.items().count();i++){
			out_streams::output("\nLoading codegenerator ",props.lstLangGenerators.items()[i],"...");
			if (!LangGenTable.AddLibary(props.lstLangGenerators.items()[i]))
				errors::e_file_not_found(props.lstLangGenerators.items()[i]);
				//return grmc_exports::cecINIT_FAILED;
			else
				out_streams::output(" OK");
		}
	}

	//Пока это только для одного файла, потому что линковка еще не доделана
	CModuleContextList Modules;

	//Анализируем все файлы
	for (size_t i=0; i<props.lstSources.items().count();i++)
	{	
		Modules.push_back(new CModuleContext(props,props.lstSources.items()[i],Linker));
		Modules.back()->m_Preprocessor.SetInputTranslationDir(props.strInputTranslationDir);
		Modules.back()->m_Preprocessor.SetOutputTranslationDir(props.strOutputTranslationDir);

		TIME_REPORT("\nTime (end init): ",clock());

		grmc_exports::exit_code ecode = CompileModule(*Modules.back(),ImportTable,LangGenTable,props);
		if (ecode!=grmc_exports::cecOK)
			return ecode;
		
		Modules.back()->m_LexAnalyzer.Clear(); //Освобождаем память из-под ненужных структур
		Modules.back()->m_SntxAnalyzer.Clear();
		CFunctions::Clear();	//Это делать необходимо, чтобы очистить все 
		CLexFunctions::Clear();	//внутренние структуры перед следующим модулем
	}

///// сборка
	if (!props.bLinking || props.bSyntaxCheck)
		goto FINISH;

	PROCESS_REPORT("\nLinking:");
	for (CModuleContextList::iterator i=Modules.begin();i!=Modules.end();++i)
		Linker.AddModule((*i)->m_strFileName.c_str(),(*i)->m_LexTable,(*i)->m_TypesTable,(*i)->m_PCodeList);
	Linker.Link(props.strResultFileName.c_str(),props.bSFX,ImportTable);
	
	CHECK_USER_BREAK
	if (errors::is_errors()) 
	{
		errors::output_ew();
		return grmc_exports::cecLINK_FAILED;
	}
	
	if (!errors::is_ew()) PROCESS_REPORT(" OK");

FINISH:
#ifdef _SHOW_TIME
	finish=clock();
	out_streams::outs<<"\nTime duration: "<<(double)((double)(finish-start)/CLOCKS_PER_SEC);
#endif
	return grmc_exports::cecOK;
}

//------------------------------------------------------------------------

grmc_exports::exit_code Build(const build_props & props)
{
	grmc_exports::exit_code result = grmc_exports::cecFORCED_TERMINATION;
	try{
		result=BuildInternal(props);
	}
	catch (exception & e) {
		LOG_EXCEPTION("Unhandled internal compiler error.",&e);
		out_streams::output("Unhandled internal compiler error. Process stopped.\n");
	}
	catch (...) {
		out_streams::output("Unhandled internal compiler error. Process stopped.\n");
	}
	return result;
}	