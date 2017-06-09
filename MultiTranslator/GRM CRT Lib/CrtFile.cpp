/********************************************************************
	created:	2003/07/09
	created:	9:7:2003   8:10
	filename: 	$(Projects)\sources\grm crt lib\crtfile.cpp
	file path:	$(Projects)\sources\grm crt lib
	file base:	crtfile
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include "StdAfx.h"
#include <BaseUtils.h>
#include <Algorithm>
#include "GrmCrt.h"
#include "CrtContainer.h"
#include "tchar.h"

const cstr  szCategory = _T("Files");

using namespace base_utils;
//------------------------------------------------------------------------
// global functions for crt files
//------------------------------------------------------------------------

namespace crt_files
{

	struct file_container{
		crt_containers::text_container	m_Data;
		file_std						m_File;
	}; 
	
	class file_manager{
	private:
		typedef std::list<file_container*> list_files;
		list_files m_lstFiles;
	public:
		~file_manager(){
			while (m_lstFiles.size())
				close_file(m_lstFiles.front());
		}

		file_container* create_file(cstr szFileName){
			file_container * pFile  = new file_container; 
			if (!pFile->m_File.open(szFileName,"wt")){
				delete pFile;
				return NULL;
			}
			m_lstFiles.push_back(pFile);
			return pFile;
		}

		bool is_valid_file(file_container * pFile){
			return find(m_lstFiles.begin(),m_lstFiles.end(),pFile)!=m_lstFiles.end();
		}
		
		void close_file(file_container * pFile){
			list_files::iterator it = find(m_lstFiles.begin(),m_lstFiles.end(),pFile);
			if (it==m_lstFiles.end())
				return;
			string_smart s = (*it)->m_Data.get_text();
			if ((*it)->m_File.iswriteable())
				(*it)->m_File.write(s.c_str(),s.length());
			delete (*it);
			m_lstFiles.erase (it);
		}

		size_t append_to_file(file_container * pFile,variant & arg){
			list_files::iterator it = find(m_lstFiles.begin(),m_lstFiles.end(),pFile);
			if (it==m_lstFiles.end())
				return 0;	
			return (*it)->m_Data.append(arg);
		}

		size_t insert_into_file(file_container * pFile,variant & arg, size_t nPos, bool before){
			list_files::iterator it = find(m_lstFiles.begin(),m_lstFiles.end(),pFile);
			if (it==m_lstFiles.end())
				return 0;
			return (*it)->m_Data.insert(arg,nPos,before);
		}

		//---------------------------------------------------------------------------------
		file_container* open_file_for_read(cstr szFileName){
			file_container * pFile  = new file_container; 
			if (!pFile->m_File.open(szFileName,"rt")){
				delete pFile;
				return NULL;
			}
			m_lstFiles.push_back(pFile);
			return pFile;
		}

		size_t read_from_file(file_container * pFile, size_t nSymbols, string_smart & s){
			s.reserve_and_zero(nSymbols);
			return pFile->m_File.read(s.buffer(),nSymbols);
		}
	}; 

	file_manager g_FileManager;

	//Opens a file for writing. Returns a Handle of opened file
	EXPORT size_t WINAPI FileCreate(cstr szFileName)
	{
		return (size_t)g_FileManager.create_file(szFileName);
	}
	registrator reg_file_create(FileCreate,szCategory, "FileCreate","Creates a new file or opens the existing file for writing.");

	//Opens a file for reading. Returns a Handle of opened file
	EXPORT size_t WINAPI FileOpen(cstr szFileName)
	{
		return (size_t) g_FileManager.open_file_for_read(szFileName);
	}
	registrator reg_file_open(FileOpen,szCategory,"FileOpen","Opens the existing file for reading.");

	//Closes the previously opened file;
	EXPORT bool WINAPI FileClose(size_t nHandle)
	{
		if (!g_FileManager.is_valid_file((file_container*)nHandle))
			return false;
		g_FileManager.close_file((file_container*)nHandle);
		return true;
	}
	registrator reg_file_close(FileClose,szCategory, "FileClose","Closes the file previously opened with functions FileCreate or FileOpen");

	//Writes to the file. 
	EXPORT size_t WINAPI FileAppend(size_t nHandle, variant Arg)
	{
		size_t ret = g_FileManager.append_to_file((file_container*)nHandle,Arg);
		finalize_variant(Arg); //Из-за передачи по значению
		return ret;
	}
	registrator reg_file_append(FileAppend,szCategory,"FileAppend","Writes the value to the end of the indicated file. The file must be preliminarily opened by function FileCreate");

	//Inserts into the file. 
	EXPORT size_t WINAPI FileInsertBefore(size_t nHandle, variant Arg, size_t nPos)
	{
		size_t ret = g_FileManager.insert_into_file((file_container*)nHandle,Arg,nPos,true);
		finalize_variant(Arg); //Из-за передачи по значению
		return ret;
	}
	registrator reg_file_insert_b(FileInsertBefore,szCategory,"FileInsertBefore","Writes the value before the specified position of the indicated file. The file must be preliminarily opened by function FileCreate");

	//Inserts into the file. 
	EXPORT size_t WINAPI FileInsertAfter(size_t nHandle, variant Arg, size_t nPos)
	{
		size_t ret = g_FileManager.insert_into_file((file_container*)nHandle,Arg,nPos,false);
		finalize_variant(Arg); //Из-за передачи по значению
		return ret;
	}
	registrator reg_file_insert_a(FileInsertAfter,szCategory,"FileInsertAfter","Writes the value after the specified position of the indicated file. The file must be preliminarily opened by function FileCreate");

	//Reads from the file. 
	EXPORT cstr WINAPI FileRead(size_t nHandle, long nLen, int & nRead)
	{
		STRING_IMPL("");
		nRead = (int)g_FileManager.read_from_file((file_container*)nHandle,nLen,s);
		return s.c_str();
	}
	registrator reg_file_read(FileRead,szCategory,"FileRead","Reads the value to the indicated file. File must be preliminarily opened by function FileOpen. Second parameter specifies bytes to read, third one - returns bytes that were actually read");

	//Removes the file
	EXPORT bool WINAPI FileRemove(cstr szFileName)
	{
		return delete_file(szFileName);
	}
	registrator reg_file_remove(FileRemove,szCategory,"FileRemove","Removes the file");

	//Check if file exists
	EXPORT bool WINAPI FileExists(cstr szFileName)
	{
		return is_file_exist(szFileName);
	}
	registrator reg_file_exists(FileExists,szCategory,"FileExists","Ensures whether the specified file exists or not");

	//Check if directory exists
	EXPORT bool WINAPI DirExists(cstr szDirName)
	{
		return is_dir_exist(szDirName);
	}
	registrator reg_dir_exists(DirExists,szCategory,"DirExists","Ensures whether the specified directory exists or not");

	//Changes an extension of given filenemae
	EXPORT cstr WINAPI FileChangeExt(cstr szFileName, cstr szExt)
	{
		STRING_IMPL(szFileName);
		s = change_fileext(szFileName,szExt);
		return s.c_str();
	}
	registrator reg_file_c_ext(FileChangeExt,szCategory,"FileChangeExt","Changes an extension of specified filename");

	//Finds a full path of indicated file
	EXPORT cstr WINAPI FileFullPath(cstr szFileName)
	{
		STRING_IMPL(szFileName);
		s=find_full_filepath(szFileName);
		return s.c_str();
	}
	registrator reg_file_full_path(FileFullPath,szCategory,"FileFullPath","Finds a full path of indicated file");

	//Extracts directory from path
	EXPORT cstr WINAPI FileExtractDir(cstr szFileName)
	{
		STRING_IMPL(szFileName);
		s=extract_filedir(szFileName);
		return s.c_str();
	}
	registrator reg_extract_file_dir(FileExtractDir,szCategory,"FileExtractDir","Extracts a directory path from indicated file");

	//Creates all specified directories
	EXPORT bool WINAPI ForceDirectories(cstr szPath)
	{
		return force_dirs(szPath);
	}
	registrator reg_force_directories(ForceDirectories,szCategory,"ForceDirectories","Creates all specified directories");
}