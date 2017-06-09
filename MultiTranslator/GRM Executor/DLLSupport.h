#ifndef __LEXICON_DLL_SUPPORT_HEADER
#define __LEXICON_DLL_SUPPORT_HEADER

#include <GrmKrnlExports.h> //Здесь описаны прототипы функций ядра разбора
using namespace grmkrnl_exports;

#define ERROR_NOT_DEFINED	-1

//Обертка для интерфейса IGrmKrnl. Не стал тащить стандартные обертки из COM
class _grmkrnl_iface_ptr_t
{
private:
	IGrmKrnl * m_pIface;
	void Release(){
		if (m_pIface!=NULL)
			m_pIface->Release();
		m_pIface=NULL;
	}
public:
	_grmkrnl_iface_ptr_t():m_pIface(NULL){
	}
	~_grmkrnl_iface_ptr_t(){
		Release();
	}

	_grmkrnl_iface_ptr_t & operator = (IGrmKrnl * pIface){
		Release();
		m_pIface=pIface; 
		if (m_pIface)
			m_pIface->AddRef();
		return *this;
	}
	
	IGrmKrnl * operator ->(){
		_ASSERT(m_pIface);
		return m_pIface;
	}

	operator bool(){
		return m_pIface!=NULL;
	}
};

extern dll_function<pf_GetGrmKrnlInterface>	pLibGetGrmKrnlInterface;
extern _grmkrnl_iface_ptr_t pIGrmKrnl;

bool LoadGrammsDLL();

#endif