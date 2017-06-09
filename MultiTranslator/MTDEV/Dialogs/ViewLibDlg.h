/********************************************************************
	created:	2002/09/19
	created:	19:9:2002   13:39
	filename: 	$(Projects)\sources\mtdev\dialogs\statisticdlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	statisticdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Диалог оптимизации правил на основе информации из файла *.sct
	
*********************************************************************/

#pragma once
//
#include <ExtControls.h>
#include <GrmCrtLibExports.h>

#include "Names.h"
#include "CObjectEx.h"		//Один из родителей
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CdlgViewLib dialog
class CdlgViewLib : public CCObjectEx, public IViewLibLink, public  mtdev_lib::IViewCrtLibraryNotifier
{
public:
	CdlgViewLib();   // standard constructor
protected:
	typedef vector<grm_structs::func_data> func_list;
	//-- class -----------------------------------------------------------
	struct 	lib_data
	{
		struct func_data{
			string_std m_strSignature;
			string_std m_strDescription;
			string_std m_strCategory;
			func_data(cstr szCategory,cstr szSign, cstr szDesc): m_strCategory(szCategory), m_strSignature(szSign),m_strDescription(szDesc){
			}
		};
		typedef vector<func_data> lib_list;
		string_std					m_strLibName;
		string_std					m_strLibComment;
		lib_list					m_lstFuncs;
		HICON						m_hIcon;

		lib_data(): m_hIcon(NULL){
		}

		lib_data(cstr szFileName, cstr szLibComment): m_strLibName(szFileName), m_strLibComment(szLibComment), m_hIcon(NULL){
		}

		bool load(cstr szLibName)
		{
			m_lstFuncs.clear();
			HMODULE hModule = LoadLibrary(szLibName);
			if (!hModule)
				return false;

			//Две основные функции, которые должны присутствовать в каждой библиотеке
			grmcrtlib_exports::pf_GetFirstFunc pGetFirstFunc = (grmcrtlib_exports::pf_GetFirstFunc)GetProcAddress(hModule,grmcrtlib_exports::szf_GetFirstFunc);
			grmcrtlib_exports::pf_GetNextFunc  pGetNextFunc = (grmcrtlib_exports::pf_GetNextFunc)GetProcAddress(hModule,grmcrtlib_exports::szf_GetNextFunc);

			if ((!pGetFirstFunc) || (!pGetNextFunc)){
				FreeLibrary(hModule);
				return false;
			}

			for (HANDLE fh = pGetFirstFunc(); fh ; ){
				grm_structs::func_data FData;
				fh = pGetNextFunc(fh,FData);
				m_lstFuncs.push_back(func_data(cstr_safe(FData.m_szCategory),
											   FData.str().c_str(),
											   cstr_safe(FData.m_szDesciption)));
			}

			FreeLibrary(hModule);
			return true;
		}

	};
	mtdev_lib::IViewCrtLibraryPtr m_spViewLibrary;

	//Просматривает переданный путь к библиотеке на предмет, является 
	//ли этот можуль действительно библиотекой, и если это так, 
	//добавляет его в список
	bool ObserveLibrary(cstr szFilePath, OUT lib_data & LData);

	//
	bool AddLibrary(const lib_data & LData);
	
	override bool Run();

	//From IViewCrtLibraryNotifier 
	override HRESULT __stdcall raw_OnOpenLibrary (BSTR FileName );

	//IUnknown
	DEFINE_ADD_REF_STUB 
	DEFINE_RELEASE_STUB 
	BEGIN_QUERY_INTERFACE 
		COM_INTERFACE_SUPPORT(mtdev_lib::IViewCrtLibraryNotifier) 
	END_QUERY_INTERFACE 
};

