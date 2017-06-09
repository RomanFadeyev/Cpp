/********************************************************************
	created:	2002/10/05
	created:	5:10:2002   10:23
	filename: 	$(Projects)\sources\modulenames.h
	file path:	$(Projects)\sources
	file base:	modulenames
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Здесь приведены названия различных модулей проекта
*********************************************************************/
#include <BaseUtils.h>
#include <ProductEdition.h>
#pragma  once

using namespace base_utils;
namespace module_names
{
	//Имя компилятора
	#ifdef _DEBUG
	const_str szCompilerName       = _T("grmcd.exe");
	#else
	const_str szCompilerName       = _T("grmc.exe");
	#endif

	//Имя интерпретатора команд
	#ifdef _DEBUG
	const_str szExecuterName       = _T("grmexecd.exe");
	#else
	const_str szExecuterName       = _T("grmexec.exe");
	#endif

	//Имя интерпретатора команд в виде динамической библиотеки (для отладки)
	#ifdef  _DEBUG
	const_str szDebuggerName       = _T("grmexecd.dll");
	#else
	const_str szDebuggerName       = _T("grmexec.dll");
	#endif

	//Имя динамической библиотеки, содержащей ядро разбора
	#ifdef  _DEBUG
	const_str szGrmKrnlName        = _T("grmkrnld.dll");
	#else                          
	const_str szGrmKrnlName        = _T("grmkrnl.dll");
	#endif

	const_str szSDLibName          = _T("mtsd.dll");

#ifndef USE_INTERNAL_HIGHLIGHTER
	#ifdef  _DEBUG
	const_str szSourceLexer        = _T("scilexerd.dll");
	#else                          
	const_str szSourceLexer        = _T("scilexer.dll");
	#endif
#endif

	//Имя основной библиотеки функций для языа действий
	//наподобие  CRT в C
	#ifdef  _DEBUG
	const_str szCrtLibName        = _T("grmcrtd.mtl");
	#else                          
	const_str szCrtLibName        = _T("grmcrt.mtl");
	#endif

	//Имя библиотеки c компонентами для mtdev (наподобие mso.dll в Office)
	#ifdef  _DEBUG
	const_str szMTDevLibName     = _T("mtcg.dll"); //
	#else                         
	const_str szMTDevLibName     = _T("mtcg.dll"); //Названия одинаковые
	#endif


	//Имя программы для поиска файлов
	const_str szFindInFilesName    = _T("findstr.exe");
	const_str szReplaceInFilesName = _T("mung.exe");
	
	//Назавание переменной, которая указывает 
	//каталог, в котором можно искать так же, как и в рабочем каталоге
	const_str szEnvPath				= _T("ProjDLL");

	//Каталог с файлами подсветки синтаксиса 
	const_str szFolderHighLight		= _T("highlight");
	//Каталог с плагинами расцветки
	const_str szFolderSkins			= _T("skins");
	//Каталог, открываемый по умолчанию при открытии диалога выбора файлов
	const_str szFolderDocumentsDefault	= _T("samples");
	//Каталог с плагинами генерации исходных текстов для различных языков
	const_str szFolderLangGen			= _T("generators");
	//Каталог с базой знаний
	const_str szFolderKnowledgeBase		= _T("Knowledge Base");
}
