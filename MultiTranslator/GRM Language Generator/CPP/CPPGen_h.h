
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Wed Dec 24 17:01:28 2003
 */
/* Compiler settings for CPPGen.idl:
    Os, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CPPGen_h_h__
#define __CPPGen_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILanguageCodeGenerator_FWD_DEFINED__
#define __ILanguageCodeGenerator_FWD_DEFINED__
typedef interface ILanguageCodeGenerator ILanguageCodeGenerator;
#endif 	/* __ILanguageCodeGenerator_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ILanguageCodeGenerator_INTERFACE_DEFINED__
#define __ILanguageCodeGenerator_INTERFACE_DEFINED__

/* interface ILanguageCodeGenerator */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ILanguageCodeGenerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("07C8697D-368B-46e9-8311-13DB6FA4C892")
    ILanguageCodeGenerator : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ILanguageCodeGeneratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILanguageCodeGenerator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILanguageCodeGenerator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILanguageCodeGenerator * This);
        
        END_INTERFACE
    } ILanguageCodeGeneratorVtbl;

    interface ILanguageCodeGenerator
    {
        CONST_VTBL struct ILanguageCodeGeneratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILanguageCodeGenerator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILanguageCodeGenerator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILanguageCodeGenerator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILanguageCodeGenerator_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


