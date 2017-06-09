/********************************************************************
	created:	2002/09/30
	created:	30:9:2002   22:01
	filename: 	$(Projects)\sources\grm executor\package.cpp
	file path:	$(Projects)\sources\grm executor
	file base:	package
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Работа с архивами. Паковка/Распаковка
*********************************************************************/

#include "stdafx.h"
#include <BaseUtils.h>
#include <OutStreams.h>
#include "grmcrt.h"
#include "tchar.h"

const cstr  szCategory = _T("Packages");

using namespace base_utils;
using namespace out_streams;

namespace crt_package
{
	//Распаковывает файл szPackageName в каталог szToFolder, если 
	//szPackageName - относительный, используется каталог szAuxWorkDir
	//для поиска в нем
	EXPORT bool WINAPI InstallPackage(cstr szPackageName, cstr szToFolder)
	{
		cstr szAuxWorkDir = szToFolder;  //Не доделано

		file_std	CabFile;
		file_std	File;
		DWORD	dwFilesCount;
		DWORD	dwFileSize;	
		TCHAR	szFileName[255];
		vector <string_smart>	FileNames;
		vector <DWORD>			FileSizes;
		
		string_smart strPackageName;
		string_smart strToFolder;
		
		bool	Searched=false;

		//Ищем в текущей директории(если название файла указано не абсолютно)
		strPackageName=change_fileext(szPackageName,".pkg");	
		if (!CabFile.open(strPackageName.c_str(),"rb"))
		{
			//Доп ищем в указанной директории (если strPackageName - относительный)
			if (is_relative_path(strPackageName.c_str())){
				Searched=true;
				output("Searching cab file...\n");
				
				strPackageName=include_backslash(szAuxWorkDir).cat(strPackageName.c_str()); 

				if (!CabFile.open(strPackageName.c_str(),"rb")){
					output("ERROR: Cannot open cab file \"",strPackageName.c_str(),"\".\n");
					return false;
				}
			}
		}
		
		//Если мы нашли файл в доп директории, то и вывод должен быть туда же
		if ((Searched) && (is_relative_path(szToFolder))){
			strToFolder=szAuxWorkDir;
			strToFolder.cat(include_backslash(szToFolder).c_str());
		}
		else
			strToFolder=include_backslash(szToFolder);
			
		//Собираем список файлов в архиве
		dwFilesCount = CabFile.read_dword();
		for (DWORD i=0;i<dwFilesCount;i++)
		{
			CabFile.read(szFileName,255);
			dwFileSize = CabFile.read_dword();
			FileNames.push_back(string_smart(strToFolder.c_str(),szFileName));
			FileSizes.push_back(dwFileSize);
		}
		
		//Читаем поочередно все файлы из архива и записываем их отдельно
		for (DWORD i=0;i<dwFilesCount;i++)
		{
			if (!File.open(FileNames[i].c_str(),"wb"))
			{
				MessageBox(0,string_smart("Cannot create ",FileNames[i].c_str()).c_str(),NULL,0);
				continue;
			}

			string_smart strBuffer(FileSizes[i]);
			CabFile.read(strBuffer.buffer(),FileSizes[i]);
			File.write(strBuffer.buffer(),FileSizes[i]);
			
			File.close();
		}
		CabFile.close();
		return true;
	}
	registrator  reg_InstallPackage(InstallPackage,szCategory, "InstallPackage","Installs the specified archive to the specified directory");

	//Не доделано
	bool PackFiles(cstr argv[], size_t ArgCount, cstr szPackageName)
	{
		base_utils::file_std CabFile;
		base_utils::file_std File;
		char_t FileName[255];

		if (!CabFile.open(szPackageName,"wb"))
		{
			output("Cannot create cab file for write ");
			return false;
		}

		CabFile.write_dword((dword_t)ArgCount);


		for (size_t i=0;i<ArgCount;i++)
		{
			memset(FileName,0,255);
			strcpy(FileName,argv[i]);

			CabFile.write(FileName,255);

			if (!File.open(argv[i],"rb"))
			{
				output("Cannot open file ",argv[0]);
				CabFile.close();
				return false;
			}
			CabFile.write_dword((dword_t)File.length());
			File.close();
		}
		
		
		for (size_t i=0;i<ArgCount;i++)
		{
			if (!File.open(argv[i],"rb"))
			{
				output("Cannot open file ",argv[0]);
				CabFile.close();
				return false;
			}

			size_t dwSize=File.length();
			raw_buffer<char> buffer (dwSize);
			File.read(buffer.ptr(),dwSize);
			CabFile.write(buffer.ptr(),dwSize);
			File.close();
			
		}
		CabFile.close();
		return true;
	}
}