#ifndef __COMPILER__CPP
#define __COMPILER__CPP

#include <GrmcExports.h>
#include "BaseStructs.h"

struct build_props:public IBuildContext
{
	string_list lstSources;
	string_list lstLibraries;
	string_list	lstLangGenerators;
	string_smart strResultFileName;

	string_smart strOutputDirectory;
	string_smart strIntermediateDirectory;

	string_smart strInputTranslationDir;
	string_smart strOutputTranslationDir;

	bool bLinking;
	bool bSFX;
	bool bDebug;
	bool bCompress;
	bool bSyntaxCheck;
	
	build_props() : bLinking(true),bSFX(true),bDebug(true),bCompress(false),bSyntaxCheck(false){
	}

	override string_smart GetOutputDirectory() const {return strOutputDirectory.c_str();}
	override string_smart GetIntermediateDirectory() const {return strIntermediateDirectory.c_str();}
	override string_smart GetResultFileName() const {return strResultFileName.c_str();}

};
grmc_exports::exit_code Build(const build_props & props);
#endif