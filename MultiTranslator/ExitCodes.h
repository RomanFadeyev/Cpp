/********************************************************************
	created:	2002/08/19
	created:	19:8:2002   9:32
	filename: 	$(Projects)\sources\exitcodes.h
	file path:	$(Projects)\sources
	file base:	exitcodes
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Здесь приведены коды возврата для различных программ проекта
*********************************************************************/

#pragma once

namespace exit_codes
{
	//Коды возврата интерпретатора команд (Executer.exe или grmexec[d].exe)
	//Код ошибки всегда <0 и является зарезервированным
	//Все остальные коды возврата определяются пользовательской функцией main
	//Что вернет она, то вернет и интерпретатор (в DLL версии это немного не
	//так, там при вызове функции Run передается специальный параметр по ссылке для получения кода)
	//Если функции main нет, возвращается 0
	enum ERunnerExitCodes	
	{
		recOK					= 0,	//Все нормально
		recNEED_PARAMS			=-1,	//Нет входных параметров
		recCANT_LOAD_LIB		=-2,	//Не удается загрузить ряд библиотек для работы
		recCANT_LOAD_CODE		=-3,	//Не удается загрузить исполняемый файл
		recWRONG_FORMAT			=-4,	//Неверный формат bin-файла
		recWRONG_SFX_FORMAT		=-5,	//Неверный формат SFX-файла
		recNO_LOADED_CODE		=-6,	//Нет загруженного кода (DLL-версия)
		recABNORMAL_TERMINATION =-7,	//Неномальное завершение (пока только для возврата main)
		recFORCED_TERMINATION	=-255	//Принудительное завершение
	};
	
	inline const char * ERunnerExitCodes_Str(ERunnerExitCodes nCode)
	{
		switch(nCode) {
		case recOK					: return "Completed. No errors.";		
		case recNEED_PARAMS			: return "Input parameters are absent.";
		case recCANT_LOAD_LIB		: return "Can't load library(s).";		
		case recCANT_LOAD_CODE		: return "Can't load executable file.";	
		case recWRONG_FORMAT		: return "Wrong format of executable file.";
		case recWRONG_SFX_FORMAT	: return "Wrong format of executable file.";
		case recNO_LOADED_CODE		: return "No loaded code.";				
		case recABNORMAL_TERMINATION: return "Abnormal termination.";			
		case recFORCED_TERMINATION	: return "Forced termination";				
		default:
			return "Unknown error";
		}
	}
}
