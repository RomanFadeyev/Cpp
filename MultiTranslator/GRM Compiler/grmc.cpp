// grmc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <PropSet.h>
#include <BaseUtils.h>
#include <AppUtils.h>
#include <OutStreams.h>
#include <MiscUtils.h>
#include <GrmcExports.h>
#include <Shellapi.h> 
#include <ComUtil.h> 

#include "grmc.h"
#include "compiler.h"
#include "DLLSupport.h"

using namespace std;
using namespace base_utils; 
using namespace misc_utils; 
using namespace properties;

static void exec_process_callback(exec_process &, cstr szText){
	out_streams::outs<<szText;
}

bool exec_tool(string_adapter strDesc, string_adapter strCommandLine)
{
	//cmd - обязательно, Иначе не работают команды copy
	string_smart strCommand(_tgetenv(_T("COMSPEC"))," /C ","\"",strCommandLine,"\"");
	out_streams::outs<<strDesc.c_str()<<"\n";

	misc_utils::exec_process ep(strCommand,exec_process::call_back_t(&exec_process_callback));
	try{
		ep.invoke();
	}
	catch (exception & e) {
		out_streams::outs<<"Error occured during invoking the tool: "<<e.what()<<'\n';
		return false;
	}
	catch (...) {
		out_streams::outs<<"Undefined error occured during invoking the tool.\n";
		return false;
	}
	return true;
}

int _tmain(int argc, char_t* argv[], char_t* envp[])
{   
    //отключаем буферизацию вывода, иначе при чтении через pipe у нас будут большие паузы
	setvbuf( stdout, NULL, _IONBF, 0 );

    if (argc<2)
    {
		string_smart strProgramName = extract_filename(get_module_filename().c_str());

		out_streams::outs<< ApplicationData::APP_NAME << ", Version " << ApplicationData::APP_VERSION << out_streams::endl;
		out_streams::outs<< ApplicationData::APP_COPYRIGHT << ". All rights reserved"<<out_streams::endl;
		out_streams::outs<< out_streams::endl;
		out_streams::outs<< "Usage:\n\n";
		out_streams::outs<< "  "<<strProgramName<<" [options] input_file1 [,input_file1, ...] output_file\n";
		out_streams::outs<< "  "<<strProgramName<<" @make_file\n";
		out_streams::outs<< out_streams::endl;
		
		out_streams::outs<< "  Options:\n";
		out_streams::outs<< "     /SFX -  creates executable file\n";
		out_streams::outs<< out_streams::endl;
        
		out_streams::outs<< "Example: \n";
		out_streams::outs<< "  "<<strProgramName<<" /SFX src1.grm src2.grm src3.grm ResultFile\n";
		out_streams::outs<< "  "<<strProgramName<<" @your_makefile.mak\n";
		return grmc_exports::cecOK;
    }
    
    bool SFX=false;
    bool bMakeFile=false;
	grmc_exports::exit_code	result=grmc_exports::cecOK;

	//Работа через make-file ---------------------------------------------
	if (argv[1][0]=='@')
	{
		bMakeFile=true;

		//Разбираем make-file
		string_smart strBuffer;
		string_smart strMakeFilePath = argv[1]+1;
		if (!load_file(strMakeFilePath.c_str(),strBuffer))
		{
			out_streams::outs<<Format("Cannot open makefile %s\n",strMakeFilePath.c_str()).c_str();
			return -1;
		}
		property_set Props;	//Здесь мы будем хранить считанные из файла строки 
    
		char * szSectStart  =strBuffer.buffer();
		char * szSectEnd    =NULL;


		string_parse Parser;
		Parser.AddSkipRanges("\"","\"");
		Parser.AddSkipRanges("//","\n");

		while ((szSectStart) && (*szSectStart)) 
		{
			char * strKey=strtrim(szSectStart); //Ключ 
			if (Parser.findpair(szSectStart,"{","}",szSectStart,szSectEnd)) //Иначе не найдено значение для ключа
			{
				*szSectStart=*szSectEnd='\0';
				Props.insert(strKey,strtrim(szSectStart+1));
				szSectStart=szSectEnd+1;
			}
		}  
	
		_ASSERT(Parser.isempty());
		//------Запуск компилятора
		build_props BuildProps;
		//Получение списка файлов
		BuildProps.lstSources= Props.get(grmc_exports::TAG_PROJECT_SOURCES).c_str();
		if (BuildProps.lstSources.isempty())
		{
			out_streams::outs<<"There are no input source files\n";
			return -1;
		}
		//Поправляем пути к файлам, есмли они не абсолютные
		string_list strAuxDirs = extract_filedir(find_full_filepath(strMakeFilePath.c_str()).c_str());
		for (size_t i = 0; i<BuildProps.lstSources.items().count();i++)
			BuildProps.lstSources.items().set_at(i, find_full_filepath(BuildProps.lstSources.items()[i],strAuxDirs).c_str());
			
		//Получение имени выходного файла
		BuildProps.strResultFileName= Props.get(grmc_exports::TAG_PROJECT_OUTPUT).c_str();
		if (BuildProps.strResultFileName.isempty())
		{
			out_streams::outs<<"Cannot determine output filename\n";
			return -1;
		}
		BuildProps.strResultFileName = get_abs_path(BuildProps.strResultFileName.c_str(), strAuxDirs.c_str());
		//Получение списка библиотек
		BuildProps.lstLibraries= Props.get(grmc_exports::TAG_PROJECT_LIBRARIES).c_str();
		for (size_t i=0; i<BuildProps.lstLibraries.items().count();i++)
			BuildProps.lstLibraries.items().get_at(i).trim();
		
		//Получение списка генераторов кода
		BuildProps.lstLangGenerators = Props.get(grmc_exports::TAG_PROJECT_LANG_GEN).c_str();
		for (size_t i=0; i<BuildProps.lstLangGenerators.items().count();i++)
			BuildProps.lstLangGenerators.items().get_at(i).trim();
		
		//Получение ключей
		string_smart strSwitches = Props.get(grmc_exports::TAG_PROJECT_SWITCHES).c_str();
		BuildProps.bSFX = strstr(strSwitches.c_str(),grmc_exports::szSwitch_SFX)!=NULL;
		BuildProps.bSyntaxCheck = strstr(strSwitches.c_str(),grmc_exports::szSwitch_SYNTAXCHECK)!=NULL;
		
		if (BuildProps.bSFX)
			BuildProps.strResultFileName=change_fileext(BuildProps.strResultFileName.c_str(),".exe");
		else
			BuildProps.strResultFileName=change_fileext(BuildProps.strResultFileName.c_str(),".bin");
		
		//Получение директорий
		BuildProps.strOutputDirectory=Props.get(grmc_exports::TAG_PROJECT_OUTPUT_DIR);
		if (BuildProps.strOutputDirectory.isempty())
			BuildProps.strOutputDirectory=extract_filedir(BuildProps.strResultFileName);
		BuildProps.strIntermediateDirectory=BuildProps.strOutputDirectory.c_str();
		force_dirs(BuildProps.strOutputDirectory);
		force_dirs(BuildProps.strIntermediateDirectory);

		//InputTranslationDir + OutputTranslationDir
		BuildProps.strInputTranslationDir= Props.get(grmc_exports::TAG_PROJECT_L_IN_DIR);
		BuildProps.strOutputTranslationDir= Props.get(grmc_exports::TAG_PROJECT_L_OUT_DIR);

		string_parse strLine;
		strLine.AddSkipRanges("\"","\"");

		//Получение списка PreBuildSteps
		string_list lstPreBuildSteps = Props.get(grmc_exports::TAG_PROJECT_PREBUILD_STEPS);
		if ((!BuildProps.bSyntaxCheck) && (lstPreBuildSteps.items().count()>0))
		{
			out_streams::outs<<"Performing pre-build steps...\n";
			for (size_t i = 0; i<lstPreBuildSteps.items().count();++i)
			{
				strLine = lstPreBuildSteps.items()[i];
				string_smart strDesc; string_smart strCommand;
				if (strLine.split("|",strDesc,strCommand))
				{
					strDesc.trim().frames_delete('"','"');
					strCommand.trim().frames_delete('"','"');
					if (!exec_tool(strDesc,strCommand))
					{
						result=grmc_exports::cecINIT_FAILED;
						break;
					}
				}
			}
			out_streams::outs<<"------------------------------------------\n";
		}

		//Выполнение компиляции
		if (result==grmc_exports::cecOK)
			result = Build(BuildProps);

		//Получение списка PostBuildSteps
		if (result == grmc_exports::cecOK) 
		{
			//Получение списка PostBuildSteps
			string_list lstPostBuildSteps = Props.get(grmc_exports::TAG_PROJECT_POSTBUILD_STEPS);
			if ((BuildProps.bSyntaxCheck) && (lstPostBuildSteps.items().count()>0))
			{
				out_streams::outs<<"\n\n------------------------------------------\n";
				out_streams::outs<<"Performing post-build steps...\n";
				for (size_t i = 0; i<lstPostBuildSteps.items().count();++i){
					strLine = lstPostBuildSteps.items()[i];
					string_smart strDesc; string_smart strCommand;
					if (strLine.split("|",strDesc,strCommand))
					{
						strDesc.trim().frames_delete('"','"');
						strCommand.trim().frames_delete('"','"');
						if (!exec_tool(strDesc,strCommand))
						{
							result=grmc_exports::cecINIT_FAILED;
							break;
						}
					}
				}
			}
		}

		//Получение имени файла для сброса в него информации по грамматике
		string_smart strInfoFileName= Props.get(grmc_exports::TAG_PROJECT_INFO).c_str();
		if (!strInfoFileName.isempty())
		{
			file_std aFile(strInfoFileName.c_str(),"w",false);
			if (!aFile.isopen())
				out_streams::outs<<"Warning: cannot open "<<strInfoFileName.c_str()<<" for writing info.\n";
			else if (!pIGrmKrnl->GetGrammarInfo(aFile))
				out_streams::outs<<"Warning: cannot write information to"<<strInfoFileName.c_str()<<".\n";				
		}

		//Выход из ветки 
	}
	
	//Работа с командной строкой------------------------------------------
    else
    {
		build_props BuildProps;
		BuildProps.bSFX = (stricmp(argv[1],"/SFX")==0);
		BuildProps.strResultFileName= argv[argc-1];
		if (BuildProps.bSFX)
			BuildProps.strResultFileName=change_fileext(BuildProps.strResultFileName.c_str(),".exe");
		else
			BuildProps.strResultFileName=change_fileext(BuildProps.strResultFileName.c_str(),".bin");
		
		for (size_t i=0; i<argc-2-BuildProps.bSFX;i++)
			BuildProps.lstSources.items().add(argv[1+SFX+i]);

		result = Build(BuildProps);
	}

	//Контрольный перевод строки, так мы отделяем любые возможные сообщения от 
	//текущей строки, не убирать
	out_streams::outs<<out_streams::endl;

	//
	base_utils::cleanup();
	return result;
}
