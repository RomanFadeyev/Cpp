#ifndef __LEXICON_DLL_SUPPORT_HEADER
#define __LEXICON_DLL_SUPPORT_HEADER

#include <GrmKrnlExports.h> //Здесь описаны прототипы функций ядра разбора
using namespace grmkrnl_exports;

#define ERROR_NOT_DEFINED	-1

//Обертка для интерфейса IGrmKrnl. Не стал тащить стандартные обертки из COM
typedef base_utils::ref_cnt_ptr<IGrmKrnl> _grmkrnl_iface_ptr_t;

extern dll_function<pf_GetGrmKrnlInterface>	pLibGetGrmKrnlInterface;
extern _grmkrnl_iface_ptr_t pIGrmKrnl;

bool LoadGrammsDLL();

#endif