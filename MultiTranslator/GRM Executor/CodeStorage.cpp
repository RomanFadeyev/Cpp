/********************************************************************
	created:	2002/10/02
	created:	2:10:2002   17:31
	filename: 	$(Projects)\sources\grm executor\codestorage.cpp
	file path:	$(Projects)\sources\grm executor
	file base:	codestorage
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include <GrmExecExports.h>
#include <GrmcExports.h>

#include "CodeStorage.h" 

using namespace grmexec_exports;

void CCodeStorage::Clear()
{
	m_Commands.clear();
	m_Consts.clear();
	m_Types.clear();
	m_strFileName.erase();
	m_strDir.erase();
}
//------------------------------------------------------------------------
void CCodeStorage::SetIP(size_t dwAddr)
{
	_ASSERT(dwAddr<m_Commands.size()); 
	m_dwCurrentPoint=dwAddr;
	pCurCmd=&m_Commands[m_dwCurrentPoint];
}
//------------------------------------------------------------------------
void CCodeStorage::IncIP()
{
	m_dwCurrentPoint++;
	pCurCmd=&m_Commands[m_dwCurrentPoint];
}
//------------------------------------------------------------------------
bool CCodeStorage::GetSrcData(size_t nIndex, cstr & szModule, size_t & nSrcLine)
{
	bool bFound=false;
	_ASSERT(nIndex<GetSize());
	for (int i=nIndex; i>=0; i--)
		if ((m_Commands[i].dwSrcLine!=-1) && (m_Commands[i].dwSrcLine!=-2)){
			nSrcLine = m_Commands[i].dwSrcLine;
			if ((bFound) && (nSrcLine==0))	//≈сли оп€ть 0,
				continue;					//не запоминаем эти данные, но двигаемс€ дальше
			szModule=m_Commands[i].m_strModuleName.c_str();
			if (m_Commands[i].dwSrcLine!=0)	//≈сли там 0, то надо попробовать еще
				return true;				//но этот вариант мы запоминаем
			bFound=true;
		}
		
	return bFound;
}
//------------------------------------------------------------------------
bool CCodeStorage::FindNearestSrcData(size_t nIndex, cstr & szModule, size_t & nSrcLine)
{
	if (GetSrcData(nIndex,szModule,nSrcLine))
		return true;
	
	//ѕопробуем чуть вперед
	for (size_t i=nIndex; i<GetSize();i++)
		if ((m_Commands[i].dwSrcLine!=-1) && (m_Commands[i].dwSrcLine!=-2))
		{
			nSrcLine = m_Commands[i].dwSrcLine;
			szModule="";
			return true;
		}
	return false;
}
#define REPORT(x) 
//------------------------------------------------------------------------
ERunnerExitCodes CCodeStorage::Load(cstr szFileName)
{
	Clear();		

	base_utils::file_std stream(szFileName,"rb");
	DWORD	dwSize;
	
	if (!stream){
		output("Can't open file '",szFileName,"' for read. Cause: ",stream.error_str());	
		return recCANT_LOAD_CODE;
	}

	stream.exceptions(true);		
	string_smart s(255);
	str szCheck=s.buffer();

    // »де€: с помощью этого проекта компилируем EXE, всовываем его в ресурс Lexicon'a
    // «атем при генерации достаем, по определенному адресу записываем сигнатуру
    // и его размер+1, а в конец дописываем BIN - код
    // когда такой файл выполн€етс€, он считывает по известному адресу сигнатуру и 
    // свой собственный размер + 1, и с полученного таким образом адреса начинает чтение BIN
    // Ќеобходимо следить, чтобы в реурсе лежал последний  вариант EXE - файла

	#define EXIT_WRONG_FORMAT {\
		stream.close();\
		output("The binary code has wrong format.\n");\
		LOG_ERROR("The binary code has wrong format.")\
		return recWRONG_FORMAT;}
	#define EXIT_CANT_LOAD(szText,szErrText) {\
		stream.close();\
		output(szText, "\n");\
		LOG_ERROR(Format("%s. Error message: %s",szText,szErrText).c_str())\
		return recCANT_LOAD_CODE;}		
	//----------------------------------------------
	try{
		//Ќужно, чтобы массив m_Consts хранил variant, т.к. именно его мы отправл€ем внешней функции
		stream.read(szCheck,2);
		
		//BIN file	
		if (memcmp(szCheck,"MZ",2)!=0) 
			stream.seek(0,file_base::beg);
		// SFX file
		else {
			stream.seek(0x3f0,file_base::beg);
			stream.read(szCheck,sizeof("SFX"));	//sizeof==strlen+1 дл€ const array
			if (memcmp(szCheck,"SFX",strlen("SFX"))!=0)
				EXIT_WRONG_FORMAT;
			stream.seek(stream.read_dword(),file_base::beg);
		}
		stream.read(szCheck,sizeof("GRMS"));//sizeof==strlen+1 дл€ const array
		if (memcmp(szCheck,"GRMS",strlen("GRMS"))!=0) EXIT_WRONG_FORMAT;

		size_t nAsmVersion = stream.read_dword();
		if (grmc_exports::nAssembleVersion<nAsmVersion) EXIT_WRONG_FORMAT;

		stream.read(szCheck,sizeof("DEBUG"));//sizeof==strlen+1 дл€ const array
		if (memcmp(szCheck,"DEBUG",strlen("DEBUG"))!=0) EXIT_WRONG_FORMAT;

		m_bDebugInfo = stream.read_byte();
	}
	catch (exception & e){
		EXIT_CANT_LOAD("Exception has raised while reading executable file(sign section)",e.what());
	}
	
	base_utils::file_std DebugFile;
	map<size_t, string_std>	mapSourceFiles;

	if (m_bDebugInfo){
		string_smart strDebugFile= change_fileext(szFileName,".pdb");
		if (DebugFile.open(strDebugFile.c_str(),"rb")){
			size_t nModuleTableSize=DebugFile.read_dword();
			for (size_t i = 0; i<nModuleTableSize; i++)
				mapSourceFiles[i]=DebugFile.read_bstr().c_str();
		}
	}
	//-------------------- Load Import Table ---------------------	
	try{
		size_t nLen=stream.read_dword();
		m_Funcs.reserve(nLen);
		for (size_t i =0; i<nLen;i++){
			grm_structs::func_data fdata; fdata.read_dump(stream);
			m_Funcs.push_back(fdata);
		}
	}
	catch (exception & e){
		EXIT_CANT_LOAD("Exception has raised while reading of executable file(import section)",e.what());
	}
	//-------------------- Load Const Array ----------------------
	try{
		variant Const;
		stream>>(DWORD &)dwSize;
		DWORD	dwSz;
		for (DWORD i=0;i<dwSize;i++)
		{
			long lType,lTemp; double fTemp; string_smart strTemp;		
			stream>>lType;
			switch (lType)
			{
				case variant::vtCHAR:	stream>>lTemp;	Const=(char)  lTemp; break;
				case variant::vtBOOL:	stream>>lTemp;	Const=(bool)  (lTemp!=0); break;
				case variant::vtSMALL:	stream>>lTemp;	Const=(byte)  lTemp; break;
				case variant::vtSHORT:	stream>>lTemp;	Const=(short) lTemp; break;
				case variant::vtINT:	stream>>lTemp;	Const=(int)	  lTemp; break;
				case variant::vtDOUBLE: stream>>fTemp;	Const=(double)fTemp; break;
				case variant::vtSTRING: 
					stream>>(DWORD &) dwSz;
					strTemp.reserve(dwSz);
					stream.read(strTemp.buffer(),dwSz);strTemp.buffer()[dwSz]=0;
					Const=strTemp; 
					break;
				case variant::vtNULL: Const.clear(); break;
				default:
					_ASSERT(false);
			}
			m_Consts.push_back(Const);
		}
	}
	catch (exception & e){
		EXIT_CANT_LOAD("Exception in reading Executable file(Const Section)",e.what());
	}
	//-------------------- Load m_Types Table --------------------
	try{
		stream>>dwSize;
		for (DWORD i=0;i<dwSize;i++)
		{
			SType type; DWORD dwMembersAccount;
			stream>>(DWORD)dwMembersAccount;	// количество членов
			
			for (DWORD j=0;j<dwMembersAccount;j++)
			{
				type.Members.push_back(SType::SMember());					
				stream>>(DWORD &) type.Members[j].nTypeIndex;		// член типа - индекс своего типа в таблице
				stream>>(DWORD &) type.Members[j].nDim;				// дл€ членов - массивов
			}
			type.SetTypeClass((SType::type_class) stream.read_dword()); // класс
			type.SetType((object::type) stream.read_dword());	// дл€ простых типов - variant::vtNULL, variant::vtBOOL and etc.
			type.SetMemSize(stream.read_dword());				//–азмер пам€ти
			
			m_Types.push_back(type);
		}

		for (size_t i=0;i<dwSize;i++)
			for (size_t j=0;j<m_Types[i].Members.size();j++)
				m_Types[i].Members[j].m_pSrc=&m_Types[m_Types[i].Members[j].nTypeIndex];

	}
	catch (exception & e){
		EXIT_CANT_LOAD("Exception in reading Executable file(Type Section)",e.what());
	}
	
	//-------------------- Load Grammatics--------------------
	try{
		(pIGrmKrnl->LoadRules)(stream); 
	}
	catch (exception & e){
		EXIT_CANT_LOAD("Exception in reading Executable file(Grammatics Section)",e.what());
	}

	//-------------------- Load StartPoint --------------------
	try{
		stream>>(char &)m_bStartPoint;
		stream>>(DWORD &) m_dwStartPoint;
	}
	catch (exception & e){
		EXIT_CANT_LOAD("Exception in reading Executable file(Startpoint Section)",e.what());
	}

	//-------------------- Load m_Commands --------------------
	try{
		DWORD dwLastSrcLine=-srclnNoNumber;
		dwSize=stream.read_dword();
		for (DWORD i=0;i<dwSize;i++)
		{
			SCommand cmd;			
			stream>>(DWORD&)cmd.Op1.Class;	
			stream>>(DWORD&)cmd.Op1.Index;	
			stream>>(DWORD&)cmd.Op2.Class;	
			stream>>(DWORD&)cmd.Op2.Index;	
			stream>>(DWORD&)cmd.Op3.Class;	
			stream>>(DWORD&)cmd.Op3.Index;	
			stream>>(DWORD&)cmd.Op4.Class;	
			stream>>(DWORD&)cmd.Op4.Index;	
			stream>>(DWORD&)cmd.Operation;
			if (DebugFile.isopen())
			{
				_ASSERT(!DebugFile.iseof());
				long nModuleIndex= DebugFile.read_dword();
				if (nModuleIndex>=0)
					cmd.m_strModuleName = mapSourceFiles[nModuleIndex];
				cmd.dwSrcLine=DebugFile.read_dword();
				
				if (cmd.dwSrcLine!=srclnNoNumber){
					if (cmd.dwSrcLine!=dwLastSrcLine)	//≈сли строка не повтор€етс€
						dwLastSrcLine=cmd.dwSrcLine;
					else
						cmd.dwSrcLine=srclnSameAsPrevious;
				}

				cmd.Op1.m_strSrcText = DebugFile.read_bstr().c_str();
				cmd.Op2.m_strSrcText = DebugFile.read_bstr().c_str();
				cmd.Op3.m_strSrcText = DebugFile.read_bstr().c_str();
				cmd.Op4.m_strSrcText = DebugFile.read_bstr().c_str();
			}
			m_Commands.push_back(cmd);
		}
	}
	catch (exception & e) {
		EXIT_CANT_LOAD("Exception in reading Executable file(m_Commands Section)",e.what());
	}
	m_strFileName=szFileName;
	m_strDir = extract_filedir(szFileName);
	return recOK;
}
