// grmcrt.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "grmcrt.h"
#include <BaseUtils.h>
//#include <AppUtils.h>
#include <GrmCrtLibExports.h>

using namespace base_utils;
using namespace	grm_structs;
HMODULE g_hDLLHandle;

void str_assign(string_smart & str, cstr sz, thread_utils::critical_section & cs)
{
	cs.lock();
	try{
		str.assign(sz);
	}
	catch(...){
		cs.unlock();
		throw;
	}
	cs.unlock();
}

#define STRING_IMPL(x) static string_smart s;\
	static thread_utils::critical_section cs;\
	str_assign(s,x,cs);


//В связи с особенностями передачи данных через стек
//нам необходимо "отцеплять" текстовый буфер от variant, т.к. он автоматически 
//уничтожается на стороне вызывающего
void finalize_variant(variant & var)
{
	struct var_friend: public variant
	{
		void finalize(){
			cstr szDummy;
			m_strVal.release_buffer(szDummy);
		}
	};
	((var_friend&)var).finalize();
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			g_hDLLHandle = (HMODULE)hModule;
			_RPT0(_CRT_WARN,"grmcrt DLL: process attached");
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			_RPT0(_CRT_WARN,"grmcrt DLL: process detached");
			base_utils::cleanup();
			_ASSERT(_CrtCheckMemory());
		}
	}
    return TRUE;
}

//------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------
registrator::lst_funcs* registrator::m_pRegisteredFuncs=NULL;

EXPORT HANDLE GetFirstFunc()
{
	if (registrator::count()==0)
		return 0;

	return HANDLE(1);
}

EXPORT HANDLE GetNextFunc(HANDLE handle, OUT func_data & FuncData)
{
	intptr_t nIndex = intptr_t(handle)-1;

	if ((nIndex<0) || (nIndex>=(intptr_t)registrator::count()))
		return 0;

	const registrator::func_data & dt =	registrator::get_at((size_t)nIndex);

	//Присваивание
	memset(&FuncData,0,sizeof(FuncData));
	strncpy(FuncData.m_szFuncName,dt.m_strFuncName.c_str(),sizeof(FuncData.m_szFuncName));
	strncpy(FuncData.m_szLibName,get_module_filename(g_hDLLHandle).c_str(),sizeof(FuncData.m_szLibName));
	strncpy(FuncData.m_szCategory,dt.m_strCategory.c_str(),sizeof(FuncData.m_szCategory));
	strncpy(FuncData.m_szDesciption,dt.m_strDescription.c_str(),sizeof(FuncData.m_szDesciption));
	
	FuncData.m_RetValue = dt.m_RetValue;
	FuncData.m_nArgCount = dt.m_lstArgs.size();

	for (registrator::args::const_iterator i=dt.m_lstArgs.begin(); i!=dt.m_lstArgs.end(); i++)
		FuncData.m_Args[i-dt.m_lstArgs.begin()]=(*i);

	nIndex++;
	
	if (nIndex<(intptr_t)registrator::count())
		return HANDLE(nIndex+1);
	
	return HANDLE(0);
}
