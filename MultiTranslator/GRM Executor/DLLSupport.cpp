#include "StdAfx.h"
#include "DLLSupport.h"

#include <BaseUtils.h>
#include <ModuleNames.h>
#include <OutStreams.h>
#include <AppUtils.h>
#include <AppMessages.h>

using namespace out_streams;
using namespace module_names;

HMODULE LibraryHandle;

dll_function<pf_GetGrmKrnlInterface>	pLibGetGrmKrnlInterface(szGrmKrnlName,szf_GetGrmKrnlInterface);
_grmkrnl_iface_ptr_t pIGrmKrnl;

bool LoadGrammsDLL()
{
	if (pIGrmKrnl)
		return true;

	if ((!pLibGetGrmKrnlInterface) || (!(pIGrmKrnl=(*pLibGetGrmKrnlInterface)())))
	{
		output("Error by loading module '",szGrmKrnlName,"'");
		LOG_ERROR(MSG_ERR_CannotLoadModule(szGrmKrnlName));
		return false;	
	}
	return true;
}

