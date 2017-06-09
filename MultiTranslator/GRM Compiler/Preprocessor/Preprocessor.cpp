/********************************************************************
	created:	2004/10/23
	created:	23:10:2004   21:53
	filename: 	x:\multitranslator\sources\grm compiler\preprocessor\preprocessor.cpp
	file path:	x:\multitranslator\sources\grm compiler\preprocessor
	file base:	preprocessor
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include <OutStreams.h>
#include <ParseUtils.h>

#include "Preprocessor.h"
#include "Resource.h"
#include "EWQueue.h"

CPreprocessor::CPreprocessor(const IBuildContext & BuildContext):m_BuildContext(BuildContext)
{
}

CPreprocessor::~CPreprocessor(void)
{
}
//------------------------------------------------------------------------
bool CPreprocessor::ProcessComments(string_smart OUT & strData)
{
	//Предварительный разбор: удаление комментариев
	char_t ch_prev	= 0;

	for (str szText = strData.buffer(); *szText; ch_prev = *szText, ++szText)
	{
		char_t ch		= *szText;
		char_t ch_next	= *(szText+1);

		//Начало/конец строки
		if (ch=='\"') {
			for (++szText; (*szText); szText++)
				if ((*szText=='\n') && (*(szText-1)!='\\'))
					break;
				else if ((*szText=='\"') && (*(szText-1)!='\\'))
					break;
			continue;
		}
		else if (ch=='\'') {
			for (++szText; (*szText); szText++)
				if ((*szText=='\n') && (*(szText-1)!='\\'))
					break;
				else if ((*szText=='\'') && (*(szText-1)!='\\'))
					break;
			continue;
		}
		//Если мы наткнулись на комментарий 1-го вида
		else if ((ch=='/') && (ch_next=='/')) {
			//Пропускаем все вплоть до конца строки
			for (; (*szText) && (*szText!='\n'); szText++)
				*szText=' ';
			continue;
		}
		//Если мы наткнулись на комментарий второго вида
		else if ((ch=='/') && (ch_next=='*')) {
			//Пропускаем все вплоть до '*/'
			for (; (*szText) && !(((*szText)=='*') && (*(szText+1)=='/')); szText++)
				*szText=' ';
		}
	}
	return true;
}
//------------------------------------------------------------------------
bool CPreprocessor::ProcessDirectives(string_smart OUT & strData)
{
	//Выполнение директив
	size_t nLine=0, nPos=0;
	char_t ch_prev	= 0;

	for (str szText = strData.buffer(); *szText; ch_prev = *szText, ++szText)
	{
		char_t ch		= *szText;
		char_t ch_next	= *(szText+1);

		//#line directive
		if ((ch=='#') && (nPos==0) && (strncmp(szText+1,"line",4)==0)) {
			cstr szStart = szText+1+4;
			cstr szWord=NULL;
			parse_utils::GetWord(szStart,szWord);
			size_t nMappedLine = atoi(string_smart(szWord,szStart-szWord).c_str());
	
			DWORD nLen=0; 
			parse_utils::GetString(szStart,nLen);
			string_smart strMappedFilename(szStart,nLen); strMappedFilename.replace("\\\\","\\");

			//nLine+1 потому что базис с 1
			errors::main_pos_corrector.AddPosition(m_strFilename,nLine+1,strMappedFilename,nMappedLine);
			
			//Пропускаем все вплоть до конца строки
			for (; (*szText) && (*szText!='\n'); szText++)
				*szText=' ';
			nLine++; nPos=0;
		}
		else if (ch=='\n'){
			nLine++; nPos=0;
		}
		else
			nPos++;
	}
	return true;
}
//------------------------------------------------------------------------
static inline void clear_block(str pStart, str pEnd){
	for (;pStart!=pEnd;pStart++)
		if ((*pStart!='\r') && (*pStart!='\n'))
			*pStart=32;
}
//------------------------------------------------------------------------
bool CPreprocessor::ProcessInOutDirections(string_smart OUT & strData)
{
	size_t nLine =0;
	size_t nPos = 0; //Не доделано!

	cstr pText = strData.c_str();
	//Выбираем входящие направления трансляции
	while (parse_utils::SearchWord(pText,"IL"))
	{
		cstr pWord;
		if (!parse_utils::GetWord(pText,pWord)){
			errors::e_wrong_use_of_directive("IL",structs::text_location(m_strFilename.c_str(),nLine,nPos));
			return false;
		}
		
		cstr pEnd=pText; 
		if (!parse_utils::SearchWord(pEnd,"EIL")){
			errors::e_il_without_eil(structs::text_location(m_strFilename.c_str(),nLine,nPos));
			return false;
		}
		pEnd+=4;

		string_smart strWord(pWord,pText-pWord); //Это название направления
		if (m_strInputTranslationDir!=strWord){
			memset((void*)pText,32,pEnd-pText);
		}
		pText = pEnd;
	}

	//Выбираем исходящие направления трансляции
	pText = strData.c_str();
	while (parse_utils::SearchWord(pText,"OL"))
	{
		cstr pStart = pText-2;
		
		cstr pWord;
		if (!parse_utils::GetWord(pText,pWord)){
			errors::e_wrong_use_of_directive("OL",structs::text_location(m_strFilename.c_str(),nLine,nPos));
			return false;
		}
		
		string_smart strWord(pWord,pText-pWord); //Это название направления		
		clear_block((str)pStart,(str)pText);

		cstr pEnd=pText; 
		if (!parse_utils::SearchWord(pEnd,"EOL")){
			errors::e_ol_without_eol(structs::text_location(m_strFilename.c_str(),nLine,nPos));
			return false;
		}

		cstr pMiddle=pText; 
		if (parse_utils::SearchWord(pMiddle,"OL",pEnd-pMiddle)){
			pEnd=pMiddle-2;
		}
		else{
			clear_block((str)pEnd-3,(str)pEnd); //Сотрем EOL
		}

		if (m_strOutputTranslationDir!=strWord){
			clear_block((str)pText,(str)pEnd);
		}
		pText = pEnd;
	}
	return true;
}
//------------------------------------------------------------------------
bool CPreprocessor::Analyze(string_adapter szFilename, string_smart OUT & strData)
{
	m_strFilename = szFilename;

	HRSRC hSFXRes=FindResource(NULL,(char_t*)IDR_PREPROCESSOR,"PREPROCESSOR");
	_ASSERT(hSFXRes);
	HGLOBAL MemArea= LoadResource(NULL,hSFXRes);
	_ASSERT(MemArea);
	size_t   ResSize= SizeofResource(NULL,hSFXRes);
	_ASSERT(ResSize>0x1000);
	char_t * RawData= (char_t*) LockResource(MemArea);
	_ASSERT(RawData);

	string_smart strPreprocessorFileName = get_temp_filename(".bin");
	string_smart strOuputFile = get_temp_filename(".i");
	{
		file_std ppr_file(strPreprocessorFileName.c_str(),"wb");
		ppr_file.write(RawData,ResSize);
		ppr_file.close();
	}

	//---------------------------------------------------------------
	//Создаем канал
	SECURITY_ATTRIBUTES sa = {0}; sa.nLength=sizeof(sa); sa.bInheritHandle=true;
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite,&sa,0))
		throw exception(Format("Can't open pipe. Cause: '%s'",string_err(GetLastError()).c_str()).c_str());
	//---------------------------------------------------------------
	//Открываем процесс
	STARTUPINFO si={0};
	si.cb			= sizeof(si);
	si.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow	= SW_HIDE;
	si.hStdOutput	= hWrite;
	si.hStdError	= hWrite;
	PROCESS_INFORMATION pi;

	//CommandLine
	string_smart strCommandLine(strPreprocessorFileName.frames_wrap_on('\"','\"').c_str(),
		" -q -o\"",strOuputFile,"\" \"",szFilename.c_str(),"\"");

	//Запускать компилятор нужно обязательно с текущей директорией проекта
	string_smart strCurrentDirectory = get_current_dir();
	if (!CreateProcess(NULL,strCommandLine.buffer(),NULL,NULL,true,NORMAL_PRIORITY_CLASS,NULL,strCurrentDirectory.c_str(),&si,&pi)){
		DWORD dwErr  = GetLastError();
		CloseHandle(hWrite);
		CloseHandle(hRead);
		throw exception(Format("Can't create process.  Cause: '%s'",string_err(dwErr).c_str()).c_str());
	}
	CloseHandle(hWrite);	

	//---------------------------------------------------------------
	//Читаем результаты
	string_smart strOutput;
	char  szBuff[256];DWORD nRead;
	while (ReadFile(hRead,szBuff,255,&nRead,NULL)){
		szBuff[nRead]=0;
		strOutput.cat(szBuff);
	}
	if (!strOutput.trim().isempty()){
		out_streams::outs<<"\n"<<strOutput<<"\n";
	}

	DWORD dwExitCode;
	GetExitCodeProcess(pi.hProcess,&dwExitCode);
	CloseHandle(hRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	bool bRes=false;

	if (dwExitCode==0){
		bRes = load_file(strOuputFile,strData) && 
				ProcessComments(strData) &&
				ProcessDirectives(strData) &&
				ProcessInOutDirections(strData);
		#ifdef __DIAGNOSTIC__
		string_smart strDiagnosticFilename (include_backslash(m_BuildContext.GetIntermediateDirectory()),extract_filename(szFilename),".ppr");
		save_file(strDiagnosticFilename,strData);
		#endif
	}

	delete_file(strOutput);
	delete_file(strPreprocessorFileName);
	return bRes;
}
