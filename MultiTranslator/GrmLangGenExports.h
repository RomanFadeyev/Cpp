/********************************************************************
	created:	2003/12/24
	created:	24:12:2003   15:55
	filename: 	$(Projects)\sources\grm language export\cpp\grmlanggenexports.h
	file path:	$(Projects)\sources\grm language export\cpp
	file base:	grmlanggnrexports
	file ext:	h
	author:		Fadeyev
	
	purpose:	Declaration of interfaces, that are used to perform
			    a code generation of translator for given language 
				like C++ or Pascal
*********************************************************************/

#pragma	once

#ifdef _GRM_LANG_GENERATOR_
	#ifdef _USRDLL 
		#pragma	message("\n --- Exporting GrmLangGen functions... ---\n")  
		#define	GRM_LANG_GEN_DLL_EXPORT _declspec(dllexport)
	#else
		#define	GRM_LANG_GEN_DLL_EXPORT
	#endif
#else
	#define GRM_LANG_GEN_DLL_EXPORT  _declspec(dllimport)
	#include <BaseUtils.h>
	using namespace base_utils;
#endif

#include "GRM Language Generator\Library.h" 


namespace grm_lang_gen_exports
{
	//���������	���������� �� �������
	typedef	bool (WINAPI *pf_Initialize)			(OUT ILanguageCodeGenerator*& pIface);

	//�������� �������
	typedef	const TCHAR* const	const_str;
	
	const_str szf_Initialize		= "Initialize";

	extern "C"
	{	
		//���������	����� �������
		GRM_LANG_GEN_DLL_EXPORT BOOL WINAPI Initialize(OUT ILanguageCodeGenerator*& pIface);
	};

	#ifndef _GRM_LANG_GENERATOR_
	//��������������� �������
	class inializator
	{
	private:
		ILanguageCodeGenerator *	 m_pIface;
		dll_function <pf_Initialize> m_InitFunc;
	public:
		inializator(): m_pIface(NULL){
		}
		inializator(const inializator & src){
			m_InitFunc = src.m_InitFunc;
			m_pIface=src.m_pIface;
			if (m_pIface)
				m_pIface->AddRef();
		}
		~inializator(){
			close();
		}

		//�������� ��������� DLL � ��������� ���������� � ����������
		bool open(cstr szDllPath){
			close();

			if (m_InitFunc.attach(szDllPath,szf_Initialize))
				if ((*m_InitFunc)(m_pIface)){
					m_pIface->AddRef();
					return true;
				}
			
			return false;
		}
		//�������� ����������
		void close(){
			if (m_pIface) m_pIface->Release();
			m_InitFunc.detach();
			m_pIface=NULL;
		}
		//������ ����������
		ILanguageCodeGenerator * iface() const {
			return m_pIface;
		}
	};
	#endif
};

