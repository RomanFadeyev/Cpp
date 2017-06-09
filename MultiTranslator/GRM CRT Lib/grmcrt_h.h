
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Wed Jan 21 12:56:09 2004
 */
/* Compiler settings for grmcrt.idl:
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

#ifndef __grmcrt_h_h__
#define __grmcrt_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMTCrtLibraryFunction_FWD_DEFINED__
#define __IMTCrtLibraryFunction_FWD_DEFINED__
typedef interface IMTCrtLibraryFunction IMTCrtLibraryFunction;
#endif 	/* __IMTCrtLibraryFunction_FWD_DEFINED__ */


#ifndef __IMTCrtLibrary_FWD_DEFINED__
#define __IMTCrtLibrary_FWD_DEFINED__
typedef interface IMTCrtLibrary IMTCrtLibrary;
#endif 	/* __IMTCrtLibrary_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IMTCrtLibraryFunction_INTERFACE_DEFINED__
#define __IMTCrtLibraryFunction_INTERFACE_DEFINED__

/* interface IMTCrtLibraryFunction */
/* [helpstring][object][uuid] */ 


EXTERN_C const IID IID_IMTCrtLibraryFunction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8385F363-4F8B-4faf-897F-FCD0C2187D0D")
    IMTCrtLibraryFunction : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMTCrtLibraryFunctionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMTCrtLibraryFunction * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMTCrtLibraryFunction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMTCrtLibraryFunction * This);
        
        END_INTERFACE
    } IMTCrtLibraryFunctionVtbl;

    interface IMTCrtLibraryFunction
    {
        CONST_VTBL struct IMTCrtLibraryFunctionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMTCrtLibraryFunction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMTCrtLibraryFunction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMTCrtLibraryFunction_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMTCrtLibraryFunction_INTERFACE_DEFINED__ */


#ifndef __IMTCrtLibrary_INTERFACE_DEFINED__
#define __IMTCrtLibrary_INTERFACE_DEFINED__

/* interface IMTCrtLibrary */
/* [helpstring][object][uuid] */ 


EXTERN_C const IID IID_IMTCrtLibrary;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("54C4F0CC-A09C-4495-BF6A-4E1F8E517458")
    IMTCrtLibrary : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMTCrtLibraryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMTCrtLibrary * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMTCrtLibrary * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMTCrtLibrary * This);
        
        END_INTERFACE
    } IMTCrtLibraryVtbl;

    interface IMTCrtLibrary
    {
        CONST_VTBL struct IMTCrtLibraryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMTCrtLibrary_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMTCrtLibrary_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMTCrtLibrary_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMTCrtLibrary_INTERFACE_DEFINED__ */



#ifndef __MTCrtLibrary_LIBRARY_DEFINED__
#define __MTCrtLibrary_LIBRARY_DEFINED__

/* library MTCrtLibrary */
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_MTCrtLibrary;
#endif /* __MTCrtLibrary_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


