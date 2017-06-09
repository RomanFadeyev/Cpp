
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)

/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "CPPGen_h.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ILanguageCodeGenerator_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ILanguageCodeGenerator_ProxyInfo;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ILanguageCodeGenerator, ver. 0.0,
   GUID={0x07C8697D,0x368B,0x46e9,{0x83,0x11,0x13,0xDB,0x6F,0xA4,0xC8,0x92}} */

#pragma code_seg(".orpc")
static const unsigned short ILanguageCodeGenerator_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO ILanguageCodeGenerator_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ILanguageCodeGenerator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ILanguageCodeGenerator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ILanguageCodeGenerator_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(3) _ILanguageCodeGeneratorProxyVtbl = 
{
    &IID_ILanguageCodeGenerator,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};


static const PRPC_STUB_FUNCTION ILanguageCodeGenerator_table[] =
{
    0    /* a dummy for an empty interface */
};

const CInterfaceStubVtbl _ILanguageCodeGeneratorStubVtbl =
{
    &IID_ILanguageCodeGenerator,
    &ILanguageCodeGenerator_ServerInfo,
    3,
    &ILanguageCodeGenerator_table[-3],
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _CPPGen_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_ILanguageCodeGeneratorProxyVtbl,
    0
};

const CInterfaceStubVtbl * _CPPGen_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_ILanguageCodeGeneratorStubVtbl,
    0
};

PCInterfaceName const _CPPGen_InterfaceNamesList[] = 
{
    "ILanguageCodeGenerator",
    0
};


#define _CPPGen_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _CPPGen, pIID, n)

int __stdcall _CPPGen_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_CPPGen_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo CPPGen_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _CPPGen_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _CPPGen_StubVtblList,
    (const PCInterfaceName * ) & _CPPGen_InterfaceNamesList,
    0, // no delegation
    & _CPPGen_IID_Lookup, 
    1,
    1,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

