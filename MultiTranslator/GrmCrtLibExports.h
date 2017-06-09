/********************************************************************
	created:	2003/05/29
	created:	29:5:2003   9:41
	filename: 	$(Projects)\sources\grmcrtlibexports.h
	file path:	$(Projects)\sources
	file base:	grmcrtlibexports
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#ifdef _GRMCRT_EXPORTS_
#pragma message("Exporting grmctlib....")
#define GRMCRT_EXPORT 	extern "C" __declspec(dllexport) 
#else
#define GRMCRT_EXPORT 	extern "C" __declspec(dllimport)
#endif

#include <BaseUtils.h>
#include <GrmStructs.h>

#pragma	once
using namespace	base_utils;

namespace grmcrtlib_exports
{
	
	//Прототипы	указателей на функции
	typedef	HANDLE (*pf_GetFirstFunc)	();
	typedef	HANDLE (*pf_GetNextFunc)	(HANDLE handle, OUT grm_structs::func_data & FuncData);
	
	//Названия функций
	const_str szf_GetFirstFunc				= "GetFirstFunc";
	const_str szf_GetNextFunc				= "GetNextFunc";

	//Класс для хранения информации обо всех функциях в указанной библиотеке
	class lib_data
	{
	private:
		typedef vector<grm_structs::func_data> lib_list;
		lib_list  m_lstFuncs;
	public:
		bool load(cstr szLibName)
		{
			//????? (find_full_filepath()){
			m_lstFuncs.clear();
			HMODULE hModule = LoadLibrary(szLibName);
			if (!hModule)
				return false;

			//Две основные функции, которые должны присутствовать в каждой библиотеке
			pf_GetFirstFunc pGetFirstFunc = (pf_GetFirstFunc)GetProcAddress(hModule,szf_GetFirstFunc);
			pf_GetNextFunc  pGetNextFunc = (pf_GetNextFunc)GetProcAddress(hModule,szf_GetNextFunc);

			if ((!pGetFirstFunc) || (!pGetNextFunc)){
				FreeLibrary(hModule);
				return false;
			}

			for (HANDLE fh = pGetFirstFunc(); fh ; ){
				grm_structs::func_data FData;
				fh = pGetNextFunc(fh,FData);
				m_lstFuncs.push_back(FData);
			}

			FreeLibrary(hModule);
			return true;
		}

		size_t count() const {return m_lstFuncs.size();}
		const grm_structs::func_data & operator [] (size_t nIndex) const {return m_lstFuncs[nIndex];}
	};
	//Прототипы	функций
	GRMCRT_EXPORT HANDLE GetFirstFunc();
	GRMCRT_EXPORT HANDLE GetNextFunc(HANDLE handle, OUT grm_structs::func_data & FuncData);
}