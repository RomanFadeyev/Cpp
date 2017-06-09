
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Thu Jan 08 15:57:14 2004
 */
/* Compiler settings for \Projects\MultiTranslator.V2.5\Sources\GRM Language Generator\Library.idl:
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

#ifndef __Library_h_h__
#define __Library_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILanguageCodeGenerator_FWD_DEFINED__
#define __ILanguageCodeGenerator_FWD_DEFINED__
typedef interface ILanguageCodeGenerator ILanguageCodeGenerator;
#endif 	/* __ILanguageCodeGenerator_FWD_DEFINED__ */


#ifndef __ILanguageCodeGenerator_FWD_DEFINED__
#define __ILanguageCodeGenerator_FWD_DEFINED__
typedef interface ILanguageCodeGenerator ILanguageCodeGenerator;
#endif 	/* __ILanguageCodeGenerator_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ILanguageCodeGenerator_INTERFACE_DEFINED__
#define __ILanguageCodeGenerator_INTERFACE_DEFINED__

/* interface ILanguageCodeGenerator */
/* [helpstring][object][uuid] */ 


EXTERN_C const IID IID_ILanguageCodeGenerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("07C8697D-368B-46e9-8311-13DB6FA4C892")
    ILanguageCodeGenerator : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetProperties( 
            /* [out] */ BSTR *pName,
            /* [out] */ BSTR *pDescription) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnGrmItem( 
            /* [in] */ BSTR RuleName,
            /* [in] */ BSTR VariantName,
            /* [in] */ BSTR TermName,
            /* [in] */ ULONG ItemID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Begin( 
            /* [in] */ BSTR WorkDir,
            /* [in] */ BSTR ProjectPath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE End( void) = 0;
        
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
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProperties )( 
            ILanguageCodeGenerator * This,
            /* [out] */ BSTR *pName,
            /* [out] */ BSTR *pDescription);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *OnGrmItem )( 
            ILanguageCodeGenerator * This,
            /* [in] */ BSTR RuleName,
            /* [in] */ BSTR VariantName,
            /* [in] */ BSTR TermName,
            /* [in] */ ULONG ItemID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Begin )( 
            ILanguageCodeGenerator * This,
            /* [in] */ BSTR WorkDir,
            /* [in] */ BSTR ProjectPath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *End )( 
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


#define ILanguageCodeGenerator_GetProperties(This,pName,pDescription)	\
    (This)->lpVtbl -> GetProperties(This,pName,pDescription)

#define ILanguageCodeGenerator_OnGrmItem(This,RuleName,VariantName,TermName,ItemID)	\
    (This)->lpVtbl -> OnGrmItem(This,RuleName,VariantName,TermName,ItemID)

#define ILanguageCodeGenerator_Begin(This,WorkDir,ProjectPath)	\
    (This)->lpVtbl -> Begin(This,WorkDir,ProjectPath)

#define ILanguageCodeGenerator_End(This)	\
    (This)->lpVtbl -> End(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE ILanguageCodeGenerator_GetProperties_Proxy( 
    ILanguageCodeGenerator * This,
    /* [out] */ BSTR *pName,
    /* [out] */ BSTR *pDescription);


void __RPC_STUB ILanguageCodeGenerator_GetProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ILanguageCodeGenerator_OnGrmItem_Proxy( 
    ILanguageCodeGenerator * This,
    /* [in] */ BSTR RuleName,
    /* [in] */ BSTR VariantName,
    /* [in] */ BSTR TermName,
    /* [in] */ ULONG ItemID);


void __RPC_STUB ILanguageCodeGenerator_OnGrmItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ILanguageCodeGenerator_Begin_Proxy( 
    ILanguageCodeGenerator * This,
    /* [in] */ BSTR WorkDir,
    /* [in] */ BSTR ProjectPath);


void __RPC_STUB ILanguageCodeGenerator_Begin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ILanguageCodeGenerator_End_Proxy( 
    ILanguageCodeGenerator * This);


void __RPC_STUB ILanguageCodeGenerator_End_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILanguageCodeGenerator_INTERFACE_DEFINED__ */



#ifndef __LanguageCodeGenerator_LIBRARY_DEFINED__
#define __LanguageCodeGenerator_LIBRARY_DEFINED__

/* library LanguageCodeGenerator */
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_LanguageCodeGenerator;
#endif /* __LanguageCodeGenerator_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


