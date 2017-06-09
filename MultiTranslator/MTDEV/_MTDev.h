

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Mon Apr 17 22:03:05 2006
 */
/* Compiler settings for _MTDev.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef ___MTDev_h__
#define ___MTDev_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDocument_FWD_DEFINED__
#define __IDocument_FWD_DEFINED__
typedef interface IDocument IDocument;
#endif 	/* __IDocument_FWD_DEFINED__ */


#ifndef __ISourceDocument_FWD_DEFINED__
#define __ISourceDocument_FWD_DEFINED__
typedef interface ISourceDocument ISourceDocument;
#endif 	/* __ISourceDocument_FWD_DEFINED__ */


#ifndef __IProjectDocument_FWD_DEFINED__
#define __IProjectDocument_FWD_DEFINED__
typedef interface IProjectDocument IProjectDocument;
#endif 	/* __IProjectDocument_FWD_DEFINED__ */


#ifndef __IApplication_FWD_DEFINED__
#define __IApplication_FWD_DEFINED__
typedef interface IApplication IApplication;
#endif 	/* __IApplication_FWD_DEFINED__ */


#ifndef __IGrammaticalSymbol_FWD_DEFINED__
#define __IGrammaticalSymbol_FWD_DEFINED__
typedef interface IGrammaticalSymbol IGrammaticalSymbol;
#endif 	/* __IGrammaticalSymbol_FWD_DEFINED__ */


#ifndef __IGrammaticalVariant_FWD_DEFINED__
#define __IGrammaticalVariant_FWD_DEFINED__
typedef interface IGrammaticalVariant IGrammaticalVariant;
#endif 	/* __IGrammaticalVariant_FWD_DEFINED__ */


#ifndef __IGrammaticalRule_FWD_DEFINED__
#define __IGrammaticalRule_FWD_DEFINED__
typedef interface IGrammaticalRule IGrammaticalRule;
#endif 	/* __IGrammaticalRule_FWD_DEFINED__ */


#ifndef __IGrammar_FWD_DEFINED__
#define __IGrammar_FWD_DEFINED__
typedef interface IGrammar IGrammar;
#endif 	/* __IGrammar_FWD_DEFINED__ */


#ifndef __IParser_FWD_DEFINED__
#define __IParser_FWD_DEFINED__
typedef interface IParser IParser;
#endif 	/* __IParser_FWD_DEFINED__ */


#ifndef __Parser_FWD_DEFINED__
#define __Parser_FWD_DEFINED__

#ifdef __cplusplus
typedef class Parser Parser;
#else
typedef struct Parser Parser;
#endif /* __cplusplus */

#endif 	/* __Parser_FWD_DEFINED__ */


#ifndef __ProjectDocument_FWD_DEFINED__
#define __ProjectDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class ProjectDocument ProjectDocument;
#else
typedef struct ProjectDocument ProjectDocument;
#endif /* __cplusplus */

#endif 	/* __ProjectDocument_FWD_DEFINED__ */


#ifndef __SourceDocument_FWD_DEFINED__
#define __SourceDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class SourceDocument SourceDocument;
#else
typedef struct SourceDocument SourceDocument;
#endif /* __cplusplus */

#endif 	/* __SourceDocument_FWD_DEFINED__ */


#ifndef __Application_FWD_DEFINED__
#define __Application_FWD_DEFINED__

#ifdef __cplusplus
typedef class Application Application;
#else
typedef struct Application Application;
#endif /* __cplusplus */

#endif 	/* __Application_FWD_DEFINED__ */


/* header files for imported files */
#include "docobj.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf__MTDev_0000 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf__MTDev_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__MTDev_0000_v0_0_s_ifspec;

#ifndef __IDocument_INTERFACE_DEFINED__
#define __IDocument_INTERFACE_DEFINED__

/* interface IDocument */
/* [unique][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7242A99-D06C-4A9E-818D-5D3180EB8D90")
    IDocument : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IApplication **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Modified( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Modified( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDocument * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IDocument * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IDocument * This,
            /* [retval][out] */ IApplication **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Modified )( 
            IDocument * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Modified )( 
            IDocument * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IDocumentVtbl;

    interface IDocument
    {
        CONST_VTBL struct IDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDocument_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define IDocument_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IDocument_get_Application(This,pVal)	\
    (This)->lpVtbl -> get_Application(This,pVal)

#define IDocument_get_Modified(This,pVal)	\
    (This)->lpVtbl -> get_Modified(This,pVal)

#define IDocument_put_Modified(This,newVal)	\
    (This)->lpVtbl -> put_Modified(This,newVal)

#define IDocument_get_Path(This,pVal)	\
    (This)->lpVtbl -> get_Path(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDocument_Save_Proxy( 
    IDocument * This);


void __RPC_STUB IDocument_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDocument_Close_Proxy( 
    IDocument * This);


void __RPC_STUB IDocument_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDocument_get_Application_Proxy( 
    IDocument * This,
    /* [retval][out] */ IApplication **pVal);


void __RPC_STUB IDocument_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDocument_get_Modified_Proxy( 
    IDocument * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IDocument_get_Modified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDocument_put_Modified_Proxy( 
    IDocument * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IDocument_put_Modified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDocument_get_Path_Proxy( 
    IDocument * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IDocument_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDocument_INTERFACE_DEFINED__ */


#ifndef __ISourceDocument_INTERFACE_DEFINED__
#define __ISourceDocument_INTERFACE_DEFINED__

/* interface ISourceDocument */
/* [unique][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_ISourceDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4FE902DC-9597-4d4e-81EC-179AFADCF8F7")
    ISourceDocument : public IDocument
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ISourceDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISourceDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISourceDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISourceDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISourceDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISourceDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISourceDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISourceDocument * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            ISourceDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            ISourceDocument * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            ISourceDocument * This,
            /* [retval][out] */ IApplication **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Modified )( 
            ISourceDocument * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Modified )( 
            ISourceDocument * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            ISourceDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } ISourceDocumentVtbl;

    interface ISourceDocument
    {
        CONST_VTBL struct ISourceDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISourceDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISourceDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISourceDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISourceDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISourceDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISourceDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISourceDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISourceDocument_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define ISourceDocument_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define ISourceDocument_get_Application(This,pVal)	\
    (This)->lpVtbl -> get_Application(This,pVal)

#define ISourceDocument_get_Modified(This,pVal)	\
    (This)->lpVtbl -> get_Modified(This,pVal)

#define ISourceDocument_put_Modified(This,newVal)	\
    (This)->lpVtbl -> put_Modified(This,newVal)

#define ISourceDocument_get_Path(This,pVal)	\
    (This)->lpVtbl -> get_Path(This,pVal)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISourceDocument_INTERFACE_DEFINED__ */


#ifndef __IProjectDocument_INTERFACE_DEFINED__
#define __IProjectDocument_INTERFACE_DEFINED__

/* interface IProjectDocument */
/* [unique][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IProjectDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7242A99-D06C-4A9E-818D-5D3180EB8D99")
    IProjectDocument : public IDocument
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveAll( 
            /* [retval][out] */ VARIANT_BOOL *Result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Build( 
            /* [retval][out] */ VARIANT_BOOL *Result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Run( 
            /* [out] */ LONG *ExitCode,
            /* [retval][out] */ VARIANT_BOOL *Result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clean( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddFile( 
            /* [in] */ BSTR FilePath,
            /* [retval][out] */ VARIANT_BOOL *Result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveFile( 
            /* [in] */ BSTR FilePath,
            /* [retval][out] */ VARIANT_BOOL *Result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileFirst( 
            /* [out] */ BSTR *FileName,
            /* [retval][out] */ VARIANT_BOOL *Result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileNext( 
            /* [out] */ BSTR *FileName,
            /* [retval][out] */ VARIANT_BOOL *Result) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Active( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Active( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TargetPath( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TargetDirectory( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Notes( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Notes( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Params( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Params( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Switches( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Switches( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LangGens( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LangGens( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProjectDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProjectDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProjectDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProjectDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProjectDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProjectDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProjectDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProjectDocument * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IProjectDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IProjectDocument * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IProjectDocument * This,
            /* [retval][out] */ IApplication **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Modified )( 
            IProjectDocument * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Modified )( 
            IProjectDocument * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IProjectDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveAll )( 
            IProjectDocument * This,
            /* [retval][out] */ VARIANT_BOOL *Result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Build )( 
            IProjectDocument * This,
            /* [retval][out] */ VARIANT_BOOL *Result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Run )( 
            IProjectDocument * This,
            /* [out] */ LONG *ExitCode,
            /* [retval][out] */ VARIANT_BOOL *Result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clean )( 
            IProjectDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddFile )( 
            IProjectDocument * This,
            /* [in] */ BSTR FilePath,
            /* [retval][out] */ VARIANT_BOOL *Result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveFile )( 
            IProjectDocument * This,
            /* [in] */ BSTR FilePath,
            /* [retval][out] */ VARIANT_BOOL *Result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileFirst )( 
            IProjectDocument * This,
            /* [out] */ BSTR *FileName,
            /* [retval][out] */ VARIANT_BOOL *Result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileNext )( 
            IProjectDocument * This,
            /* [out] */ BSTR *FileName,
            /* [retval][out] */ VARIANT_BOOL *Result);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Active )( 
            IProjectDocument * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Active )( 
            IProjectDocument * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TargetPath )( 
            IProjectDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TargetDirectory )( 
            IProjectDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Notes )( 
            IProjectDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Notes )( 
            IProjectDocument * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Params )( 
            IProjectDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Params )( 
            IProjectDocument * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Switches )( 
            IProjectDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Switches )( 
            IProjectDocument * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LangGens )( 
            IProjectDocument * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LangGens )( 
            IProjectDocument * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IProjectDocumentVtbl;

    interface IProjectDocument
    {
        CONST_VTBL struct IProjectDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProjectDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProjectDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProjectDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProjectDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IProjectDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IProjectDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IProjectDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IProjectDocument_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define IProjectDocument_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IProjectDocument_get_Application(This,pVal)	\
    (This)->lpVtbl -> get_Application(This,pVal)

#define IProjectDocument_get_Modified(This,pVal)	\
    (This)->lpVtbl -> get_Modified(This,pVal)

#define IProjectDocument_put_Modified(This,newVal)	\
    (This)->lpVtbl -> put_Modified(This,newVal)

#define IProjectDocument_get_Path(This,pVal)	\
    (This)->lpVtbl -> get_Path(This,pVal)


#define IProjectDocument_SaveAll(This,Result)	\
    (This)->lpVtbl -> SaveAll(This,Result)

#define IProjectDocument_Build(This,Result)	\
    (This)->lpVtbl -> Build(This,Result)

#define IProjectDocument_Run(This,ExitCode,Result)	\
    (This)->lpVtbl -> Run(This,ExitCode,Result)

#define IProjectDocument_Clean(This)	\
    (This)->lpVtbl -> Clean(This)

#define IProjectDocument_AddFile(This,FilePath,Result)	\
    (This)->lpVtbl -> AddFile(This,FilePath,Result)

#define IProjectDocument_RemoveFile(This,FilePath,Result)	\
    (This)->lpVtbl -> RemoveFile(This,FilePath,Result)

#define IProjectDocument_GetFileFirst(This,FileName,Result)	\
    (This)->lpVtbl -> GetFileFirst(This,FileName,Result)

#define IProjectDocument_GetFileNext(This,FileName,Result)	\
    (This)->lpVtbl -> GetFileNext(This,FileName,Result)

#define IProjectDocument_get_Active(This,pVal)	\
    (This)->lpVtbl -> get_Active(This,pVal)

#define IProjectDocument_put_Active(This,newVal)	\
    (This)->lpVtbl -> put_Active(This,newVal)

#define IProjectDocument_get_TargetPath(This,pVal)	\
    (This)->lpVtbl -> get_TargetPath(This,pVal)

#define IProjectDocument_get_TargetDirectory(This,pVal)	\
    (This)->lpVtbl -> get_TargetDirectory(This,pVal)

#define IProjectDocument_get_Notes(This,pVal)	\
    (This)->lpVtbl -> get_Notes(This,pVal)

#define IProjectDocument_put_Notes(This,newVal)	\
    (This)->lpVtbl -> put_Notes(This,newVal)

#define IProjectDocument_get_Params(This,pVal)	\
    (This)->lpVtbl -> get_Params(This,pVal)

#define IProjectDocument_put_Params(This,newVal)	\
    (This)->lpVtbl -> put_Params(This,newVal)

#define IProjectDocument_get_Switches(This,pVal)	\
    (This)->lpVtbl -> get_Switches(This,pVal)

#define IProjectDocument_put_Switches(This,newVal)	\
    (This)->lpVtbl -> put_Switches(This,newVal)

#define IProjectDocument_get_LangGens(This,pVal)	\
    (This)->lpVtbl -> get_LangGens(This,pVal)

#define IProjectDocument_put_LangGens(This,newVal)	\
    (This)->lpVtbl -> put_LangGens(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_SaveAll_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ VARIANT_BOOL *Result);


void __RPC_STUB IProjectDocument_SaveAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_Build_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ VARIANT_BOOL *Result);


void __RPC_STUB IProjectDocument_Build_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_Run_Proxy( 
    IProjectDocument * This,
    /* [out] */ LONG *ExitCode,
    /* [retval][out] */ VARIANT_BOOL *Result);


void __RPC_STUB IProjectDocument_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_Clean_Proxy( 
    IProjectDocument * This);


void __RPC_STUB IProjectDocument_Clean_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_AddFile_Proxy( 
    IProjectDocument * This,
    /* [in] */ BSTR FilePath,
    /* [retval][out] */ VARIANT_BOOL *Result);


void __RPC_STUB IProjectDocument_AddFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_RemoveFile_Proxy( 
    IProjectDocument * This,
    /* [in] */ BSTR FilePath,
    /* [retval][out] */ VARIANT_BOOL *Result);


void __RPC_STUB IProjectDocument_RemoveFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_GetFileFirst_Proxy( 
    IProjectDocument * This,
    /* [out] */ BSTR *FileName,
    /* [retval][out] */ VARIANT_BOOL *Result);


void __RPC_STUB IProjectDocument_GetFileFirst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProjectDocument_GetFileNext_Proxy( 
    IProjectDocument * This,
    /* [out] */ BSTR *FileName,
    /* [retval][out] */ VARIANT_BOOL *Result);


void __RPC_STUB IProjectDocument_GetFileNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IProjectDocument_get_Active_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IProjectDocument_get_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IProjectDocument_put_Active_Proxy( 
    IProjectDocument * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IProjectDocument_put_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IProjectDocument_get_TargetPath_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IProjectDocument_get_TargetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IProjectDocument_get_TargetDirectory_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IProjectDocument_get_TargetDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IProjectDocument_get_Notes_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IProjectDocument_get_Notes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IProjectDocument_put_Notes_Proxy( 
    IProjectDocument * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IProjectDocument_put_Notes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IProjectDocument_get_Params_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IProjectDocument_get_Params_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IProjectDocument_put_Params_Proxy( 
    IProjectDocument * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IProjectDocument_put_Params_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IProjectDocument_get_Switches_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IProjectDocument_get_Switches_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IProjectDocument_put_Switches_Proxy( 
    IProjectDocument * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IProjectDocument_put_Switches_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IProjectDocument_get_LangGens_Proxy( 
    IProjectDocument * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IProjectDocument_get_LangGens_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IProjectDocument_put_LangGens_Proxy( 
    IProjectDocument * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IProjectDocument_put_LangGens_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProjectDocument_INTERFACE_DEFINED__ */


#ifndef __IApplication_INTERFACE_DEFINED__
#define __IApplication_INTERFACE_DEFINED__

/* interface IApplication */
/* [unique][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EE3E904C-6AA0-401A-BCB5-A936E3B717F6")
    IApplication : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Quit( 
            /* [in] */ VARIANT_BOOL SaveChanges) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ScreenRefresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Resize( 
            /* [in] */ LONG Width,
            /* [in] */ LONG Height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Activate( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenDocument( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ IDocument **pDoc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FindDocument( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ IDocument **pDoc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateProject( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ IProjectDocument **pDoc) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProjectCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Projects( 
            /* [in] */ LONG nIndex,
            /* [retval][out] */ IProjectDocument **pDoc) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveProject( 
            /* [retval][out] */ IProjectDocument **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MRUDocument( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Parser( 
            /* [retval][out] */ IParser **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IApplication * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IApplication * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IApplication * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IApplication * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IApplication * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IApplication * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IApplication * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Quit )( 
            IApplication * This,
            /* [in] */ VARIANT_BOOL SaveChanges);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ScreenRefresh )( 
            IApplication * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Resize )( 
            IApplication * This,
            /* [in] */ LONG Width,
            /* [in] */ LONG Height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Activate )( 
            IApplication * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenDocument )( 
            IApplication * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ IDocument **pDoc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FindDocument )( 
            IApplication * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ IDocument **pDoc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateProject )( 
            IApplication * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ IProjectDocument **pDoc);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProjectCount )( 
            IApplication * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Projects )( 
            IApplication * This,
            /* [in] */ LONG nIndex,
            /* [retval][out] */ IProjectDocument **pDoc);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveProject )( 
            IApplication * This,
            /* [retval][out] */ IProjectDocument **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MRUDocument )( 
            IApplication * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IApplication * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IApplication * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parser )( 
            IApplication * This,
            /* [retval][out] */ IParser **pVal);
        
        END_INTERFACE
    } IApplicationVtbl;

    interface IApplication
    {
        CONST_VTBL struct IApplicationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IApplication_Quit(This,SaveChanges)	\
    (This)->lpVtbl -> Quit(This,SaveChanges)

#define IApplication_ScreenRefresh(This)	\
    (This)->lpVtbl -> ScreenRefresh(This)

#define IApplication_Resize(This,Width,Height)	\
    (This)->lpVtbl -> Resize(This,Width,Height)

#define IApplication_Activate(This)	\
    (This)->lpVtbl -> Activate(This)

#define IApplication_OpenDocument(This,Path,pDoc)	\
    (This)->lpVtbl -> OpenDocument(This,Path,pDoc)

#define IApplication_FindDocument(This,Path,pDoc)	\
    (This)->lpVtbl -> FindDocument(This,Path,pDoc)

#define IApplication_CreateProject(This,Path,pDoc)	\
    (This)->lpVtbl -> CreateProject(This,Path,pDoc)

#define IApplication_get_ProjectCount(This,pVal)	\
    (This)->lpVtbl -> get_ProjectCount(This,pVal)

#define IApplication_get_Projects(This,nIndex,pDoc)	\
    (This)->lpVtbl -> get_Projects(This,nIndex,pDoc)

#define IApplication_get_ActiveProject(This,pVal)	\
    (This)->lpVtbl -> get_ActiveProject(This,pVal)

#define IApplication_get_MRUDocument(This,pVal)	\
    (This)->lpVtbl -> get_MRUDocument(This,pVal)

#define IApplication_get_Visible(This,pVal)	\
    (This)->lpVtbl -> get_Visible(This,pVal)

#define IApplication_put_Visible(This,newVal)	\
    (This)->lpVtbl -> put_Visible(This,newVal)

#define IApplication_get_Parser(This,pVal)	\
    (This)->lpVtbl -> get_Parser(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_Quit_Proxy( 
    IApplication * This,
    /* [in] */ VARIANT_BOOL SaveChanges);


void __RPC_STUB IApplication_Quit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_ScreenRefresh_Proxy( 
    IApplication * This);


void __RPC_STUB IApplication_ScreenRefresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_Resize_Proxy( 
    IApplication * This,
    /* [in] */ LONG Width,
    /* [in] */ LONG Height);


void __RPC_STUB IApplication_Resize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_Activate_Proxy( 
    IApplication * This);


void __RPC_STUB IApplication_Activate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_OpenDocument_Proxy( 
    IApplication * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ IDocument **pDoc);


void __RPC_STUB IApplication_OpenDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_FindDocument_Proxy( 
    IApplication * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ IDocument **pDoc);


void __RPC_STUB IApplication_FindDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_CreateProject_Proxy( 
    IApplication * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ IProjectDocument **pDoc);


void __RPC_STUB IApplication_CreateProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_ProjectCount_Proxy( 
    IApplication * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IApplication_get_ProjectCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_Projects_Proxy( 
    IApplication * This,
    /* [in] */ LONG nIndex,
    /* [retval][out] */ IProjectDocument **pDoc);


void __RPC_STUB IApplication_get_Projects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_ActiveProject_Proxy( 
    IApplication * This,
    /* [retval][out] */ IProjectDocument **pVal);


void __RPC_STUB IApplication_get_ActiveProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_MRUDocument_Proxy( 
    IApplication * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IApplication_get_MRUDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_Visible_Proxy( 
    IApplication * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IApplication_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IApplication_put_Visible_Proxy( 
    IApplication * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IApplication_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_Parser_Proxy( 
    IApplication * This,
    /* [retval][out] */ IParser **pVal);


void __RPC_STUB IApplication_get_Parser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IApplication_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__MTDev_0273 */
/* [local] */ 

/* [uuid] */ 
enum  DECLSPEC_UUID("0971D8E9-34A6-4846-81F3-B7E8FE27B9A3") GrammaticalSymbolClass
    {	gscUndefined	= 0,
	gscTerminal	= 1,
	gscNonTerminal	= 2,
	gscStdTerminal	= 3
    } ;
/* [uuid] */ 
enum  DECLSPEC_UUID("6BE132AD-00C5-4afd-ABDB-32F1D0C013D3") GrammaticalSymbolKind
    {	gskUndefined	= 0,
	gskNormal	= 1,
	gskOptional	= 2,
	gskChildOptional	= 3
    } ;


extern RPC_IF_HANDLE __MIDL_itf__MTDev_0273_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__MTDev_0273_v0_0_s_ifspec;

#ifndef __IGrammaticalSymbol_INTERFACE_DEFINED__
#define __IGrammaticalSymbol_INTERFACE_DEFINED__

/* interface IGrammaticalSymbol */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGrammaticalSymbol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B0E9D7B9-ECC5-44bd-8F18-BD3499B73DC2")
    IGrammaticalSymbol : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Class( 
            /* [retval][out] */ enum GrammaticalSymbolClass *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Class( 
            /* [in] */ enum GrammaticalSymbolClass newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Kind( 
            /* [retval][out] */ enum GrammaticalSymbolKind *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Kind( 
            /* [in] */ enum GrammaticalSymbolKind newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Actions( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Actions( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGrammaticalSymbolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGrammaticalSymbol * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGrammaticalSymbol * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGrammaticalSymbol * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGrammaticalSymbol * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGrammaticalSymbol * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGrammaticalSymbol * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGrammaticalSymbol * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IGrammaticalSymbol * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IGrammaticalSymbol * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Class )( 
            IGrammaticalSymbol * This,
            /* [retval][out] */ enum GrammaticalSymbolClass *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Class )( 
            IGrammaticalSymbol * This,
            /* [in] */ enum GrammaticalSymbolClass newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Kind )( 
            IGrammaticalSymbol * This,
            /* [retval][out] */ enum GrammaticalSymbolKind *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Kind )( 
            IGrammaticalSymbol * This,
            /* [in] */ enum GrammaticalSymbolKind newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Actions )( 
            IGrammaticalSymbol * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Actions )( 
            IGrammaticalSymbol * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IGrammaticalSymbolVtbl;

    interface IGrammaticalSymbol
    {
        CONST_VTBL struct IGrammaticalSymbolVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGrammaticalSymbol_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGrammaticalSymbol_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGrammaticalSymbol_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGrammaticalSymbol_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGrammaticalSymbol_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGrammaticalSymbol_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGrammaticalSymbol_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGrammaticalSymbol_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IGrammaticalSymbol_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define IGrammaticalSymbol_get_Class(This,pVal)	\
    (This)->lpVtbl -> get_Class(This,pVal)

#define IGrammaticalSymbol_put_Class(This,newVal)	\
    (This)->lpVtbl -> put_Class(This,newVal)

#define IGrammaticalSymbol_get_Kind(This,pVal)	\
    (This)->lpVtbl -> get_Kind(This,pVal)

#define IGrammaticalSymbol_put_Kind(This,newVal)	\
    (This)->lpVtbl -> put_Kind(This,newVal)

#define IGrammaticalSymbol_get_Actions(This,pVal)	\
    (This)->lpVtbl -> get_Actions(This,pVal)

#define IGrammaticalSymbol_put_Actions(This,newVal)	\
    (This)->lpVtbl -> put_Actions(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_get_Name_Proxy( 
    IGrammaticalSymbol * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IGrammaticalSymbol_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_put_Name_Proxy( 
    IGrammaticalSymbol * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IGrammaticalSymbol_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_get_Class_Proxy( 
    IGrammaticalSymbol * This,
    /* [retval][out] */ enum GrammaticalSymbolClass *pVal);


void __RPC_STUB IGrammaticalSymbol_get_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_put_Class_Proxy( 
    IGrammaticalSymbol * This,
    /* [in] */ enum GrammaticalSymbolClass newVal);


void __RPC_STUB IGrammaticalSymbol_put_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_get_Kind_Proxy( 
    IGrammaticalSymbol * This,
    /* [retval][out] */ enum GrammaticalSymbolKind *pVal);


void __RPC_STUB IGrammaticalSymbol_get_Kind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_put_Kind_Proxy( 
    IGrammaticalSymbol * This,
    /* [in] */ enum GrammaticalSymbolKind newVal);


void __RPC_STUB IGrammaticalSymbol_put_Kind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_get_Actions_Proxy( 
    IGrammaticalSymbol * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IGrammaticalSymbol_get_Actions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGrammaticalSymbol_put_Actions_Proxy( 
    IGrammaticalSymbol * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IGrammaticalSymbol_put_Actions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGrammaticalSymbol_INTERFACE_DEFINED__ */


#ifndef __IGrammaticalVariant_INTERFACE_DEFINED__
#define __IGrammaticalVariant_INTERFACE_DEFINED__

/* interface IGrammaticalVariant */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGrammaticalVariant;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6AF2BCFC-CCFF-40a2-83C6-8AD725EAE48A")
    IGrammaticalVariant : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddSymbol( 
            /* [retval][out] */ IGrammaticalSymbol **pSymbol) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Actions( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Actions( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGrammaticalVariantVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGrammaticalVariant * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGrammaticalVariant * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGrammaticalVariant * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGrammaticalVariant * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGrammaticalVariant * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGrammaticalVariant * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGrammaticalVariant * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddSymbol )( 
            IGrammaticalVariant * This,
            /* [retval][out] */ IGrammaticalSymbol **pSymbol);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Actions )( 
            IGrammaticalVariant * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Actions )( 
            IGrammaticalVariant * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IGrammaticalVariantVtbl;

    interface IGrammaticalVariant
    {
        CONST_VTBL struct IGrammaticalVariantVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGrammaticalVariant_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGrammaticalVariant_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGrammaticalVariant_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGrammaticalVariant_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGrammaticalVariant_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGrammaticalVariant_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGrammaticalVariant_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGrammaticalVariant_AddSymbol(This,pSymbol)	\
    (This)->lpVtbl -> AddSymbol(This,pSymbol)

#define IGrammaticalVariant_get_Actions(This,pVal)	\
    (This)->lpVtbl -> get_Actions(This,pVal)

#define IGrammaticalVariant_put_Actions(This,newVal)	\
    (This)->lpVtbl -> put_Actions(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGrammaticalVariant_AddSymbol_Proxy( 
    IGrammaticalVariant * This,
    /* [retval][out] */ IGrammaticalSymbol **pSymbol);


void __RPC_STUB IGrammaticalVariant_AddSymbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammaticalVariant_get_Actions_Proxy( 
    IGrammaticalVariant * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IGrammaticalVariant_get_Actions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGrammaticalVariant_put_Actions_Proxy( 
    IGrammaticalVariant * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IGrammaticalVariant_put_Actions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGrammaticalVariant_INTERFACE_DEFINED__ */


#ifndef __IGrammaticalRule_INTERFACE_DEFINED__
#define __IGrammaticalRule_INTERFACE_DEFINED__

/* interface IGrammaticalRule */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGrammaticalRule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("092C0FB9-67A7-4C8A-9C6E-F125CD40C498")
    IGrammaticalRule : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddVariant( 
            /* [retval][out] */ IGrammaticalVariant **pVariant) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AssembleText( 
            /* [retval][out] */ BSTR *aText) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Actions( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Actions( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGrammaticalRuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGrammaticalRule * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGrammaticalRule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGrammaticalRule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGrammaticalRule * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGrammaticalRule * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGrammaticalRule * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGrammaticalRule * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddVariant )( 
            IGrammaticalRule * This,
            /* [retval][out] */ IGrammaticalVariant **pVariant);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AssembleText )( 
            IGrammaticalRule * This,
            /* [retval][out] */ BSTR *aText);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IGrammaticalRule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IGrammaticalRule * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Actions )( 
            IGrammaticalRule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Actions )( 
            IGrammaticalRule * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IGrammaticalRuleVtbl;

    interface IGrammaticalRule
    {
        CONST_VTBL struct IGrammaticalRuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGrammaticalRule_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGrammaticalRule_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGrammaticalRule_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGrammaticalRule_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGrammaticalRule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGrammaticalRule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGrammaticalRule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGrammaticalRule_AddVariant(This,pVariant)	\
    (This)->lpVtbl -> AddVariant(This,pVariant)

#define IGrammaticalRule_AssembleText(This,aText)	\
    (This)->lpVtbl -> AssembleText(This,aText)

#define IGrammaticalRule_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IGrammaticalRule_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define IGrammaticalRule_get_Actions(This,pVal)	\
    (This)->lpVtbl -> get_Actions(This,pVal)

#define IGrammaticalRule_put_Actions(This,newVal)	\
    (This)->lpVtbl -> put_Actions(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGrammaticalRule_AddVariant_Proxy( 
    IGrammaticalRule * This,
    /* [retval][out] */ IGrammaticalVariant **pVariant);


void __RPC_STUB IGrammaticalRule_AddVariant_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGrammaticalRule_AssembleText_Proxy( 
    IGrammaticalRule * This,
    /* [retval][out] */ BSTR *aText);


void __RPC_STUB IGrammaticalRule_AssembleText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammaticalRule_get_Name_Proxy( 
    IGrammaticalRule * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IGrammaticalRule_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGrammaticalRule_put_Name_Proxy( 
    IGrammaticalRule * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IGrammaticalRule_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammaticalRule_get_Actions_Proxy( 
    IGrammaticalRule * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IGrammaticalRule_get_Actions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGrammaticalRule_put_Actions_Proxy( 
    IGrammaticalRule * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IGrammaticalRule_put_Actions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGrammaticalRule_INTERFACE_DEFINED__ */


#ifndef __IGrammar_INTERFACE_DEFINED__
#define __IGrammar_INTERFACE_DEFINED__

/* interface IGrammar */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGrammar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F27BAF30-1371-49f6-B12D-ADCA02C94E05")
    IGrammar : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddRule( 
            /* [retval][out] */ IGrammaticalRule **pVariant) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AssembleText( 
            /* [retval][out] */ BSTR *aText) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RuleCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rules( 
            /* [in] */ LONG nIndex,
            /* [retval][out] */ IGrammaticalRule **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGrammarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGrammar * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGrammar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGrammar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGrammar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGrammar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGrammar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGrammar * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddRule )( 
            IGrammar * This,
            /* [retval][out] */ IGrammaticalRule **pVariant);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AssembleText )( 
            IGrammar * This,
            /* [retval][out] */ BSTR *aText);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RuleCount )( 
            IGrammar * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Rules )( 
            IGrammar * This,
            /* [in] */ LONG nIndex,
            /* [retval][out] */ IGrammaticalRule **pVal);
        
        END_INTERFACE
    } IGrammarVtbl;

    interface IGrammar
    {
        CONST_VTBL struct IGrammarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGrammar_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGrammar_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGrammar_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGrammar_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGrammar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGrammar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGrammar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGrammar_AddRule(This,pVariant)	\
    (This)->lpVtbl -> AddRule(This,pVariant)

#define IGrammar_AssembleText(This,aText)	\
    (This)->lpVtbl -> AssembleText(This,aText)

#define IGrammar_get_RuleCount(This,pVal)	\
    (This)->lpVtbl -> get_RuleCount(This,pVal)

#define IGrammar_get_Rules(This,nIndex,pVal)	\
    (This)->lpVtbl -> get_Rules(This,nIndex,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGrammar_AddRule_Proxy( 
    IGrammar * This,
    /* [retval][out] */ IGrammaticalRule **pVariant);


void __RPC_STUB IGrammar_AddRule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGrammar_AssembleText_Proxy( 
    IGrammar * This,
    /* [retval][out] */ BSTR *aText);


void __RPC_STUB IGrammar_AssembleText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammar_get_RuleCount_Proxy( 
    IGrammar * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IGrammar_get_RuleCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGrammar_get_Rules_Proxy( 
    IGrammar * This,
    /* [in] */ LONG nIndex,
    /* [retval][out] */ IGrammaticalRule **pVal);


void __RPC_STUB IGrammar_get_Rules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGrammar_INTERFACE_DEFINED__ */


#ifndef __IParser_INTERFACE_DEFINED__
#define __IParser_INTERFACE_DEFINED__

/* interface IParser */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IParser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("763009C4-1A1B-4185-96A6-2BFAEF106F0D")
    IParser : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateRule( 
            /* [retval][out] */ IGrammaticalRule **aRule) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateGrammar( 
            /* [retval][out] */ IGrammar **aGrammar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Parse( 
            /* [in] */ BSTR pText,
            /* [retval][out] */ IGrammar **aGrammar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ComposeMainFunction( 
            /* [in] */ BSTR RootRule,
            /* [in] */ VARIANT_BOOL IgnoreCase,
            /* [in] */ BSTR IgnorePairList,
            /* [in] */ BSTR ReplacePairList,
            /* [retval][out] */ BSTR *pText) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IParserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IParser * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IParser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IParser * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IParser * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IParser * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IParser * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IParser * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateRule )( 
            IParser * This,
            /* [retval][out] */ IGrammaticalRule **aRule);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateGrammar )( 
            IParser * This,
            /* [retval][out] */ IGrammar **aGrammar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Parse )( 
            IParser * This,
            /* [in] */ BSTR pText,
            /* [retval][out] */ IGrammar **aGrammar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ComposeMainFunction )( 
            IParser * This,
            /* [in] */ BSTR RootRule,
            /* [in] */ VARIANT_BOOL IgnoreCase,
            /* [in] */ BSTR IgnorePairList,
            /* [in] */ BSTR ReplacePairList,
            /* [retval][out] */ BSTR *pText);
        
        END_INTERFACE
    } IParserVtbl;

    interface IParser
    {
        CONST_VTBL struct IParserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IParser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IParser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IParser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IParser_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IParser_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IParser_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IParser_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IParser_CreateRule(This,aRule)	\
    (This)->lpVtbl -> CreateRule(This,aRule)

#define IParser_CreateGrammar(This,aGrammar)	\
    (This)->lpVtbl -> CreateGrammar(This,aGrammar)

#define IParser_Parse(This,pText,aGrammar)	\
    (This)->lpVtbl -> Parse(This,pText,aGrammar)

#define IParser_ComposeMainFunction(This,RootRule,IgnoreCase,IgnorePairList,ReplacePairList,pText)	\
    (This)->lpVtbl -> ComposeMainFunction(This,RootRule,IgnoreCase,IgnorePairList,ReplacePairList,pText)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IParser_CreateRule_Proxy( 
    IParser * This,
    /* [retval][out] */ IGrammaticalRule **aRule);


void __RPC_STUB IParser_CreateRule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IParser_CreateGrammar_Proxy( 
    IParser * This,
    /* [retval][out] */ IGrammar **aGrammar);


void __RPC_STUB IParser_CreateGrammar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IParser_Parse_Proxy( 
    IParser * This,
    /* [in] */ BSTR pText,
    /* [retval][out] */ IGrammar **aGrammar);


void __RPC_STUB IParser_Parse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IParser_ComposeMainFunction_Proxy( 
    IParser * This,
    /* [in] */ BSTR RootRule,
    /* [in] */ VARIANT_BOOL IgnoreCase,
    /* [in] */ BSTR IgnorePairList,
    /* [in] */ BSTR ReplacePairList,
    /* [retval][out] */ BSTR *pText);


void __RPC_STUB IParser_ComposeMainFunction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IParser_INTERFACE_DEFINED__ */



#ifndef __mtdev_LIBRARY_DEFINED__
#define __mtdev_LIBRARY_DEFINED__

/* library mtdev */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_mtdev;

EXTERN_C const CLSID CLSID_Parser;

#ifdef __cplusplus

class DECLSPEC_UUID("0217A734-4550-4CD9-A8A2-F6066E201640")
Parser;
#endif

EXTERN_C const CLSID CLSID_ProjectDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("206DDA10-728E-4C72-81CC-36FB3BE80916")
ProjectDocument;
#endif

EXTERN_C const CLSID CLSID_SourceDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("0314E4F6-E6C5-4773-84D4-B71E8945851B")
SourceDocument;
#endif

EXTERN_C const CLSID CLSID_Application;

#ifdef __cplusplus

class DECLSPEC_UUID("ADC6D969-C5D5-49E4-A75C-F58156BDDBC9")
Application;
#endif
#endif /* __mtdev_LIBRARY_DEFINED__ */

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


