

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IDocument,0xC7242A99,0xD06C,0x4A9E,0x81,0x8D,0x5D,0x31,0x80,0xEB,0x8D,0x90);


MIDL_DEFINE_GUID(IID, IID_ISourceDocument,0x4FE902DC,0x9597,0x4d4e,0x81,0xEC,0x17,0x9A,0xFA,0xDC,0xF8,0xF7);


MIDL_DEFINE_GUID(IID, IID_IProjectDocument,0xC7242A99,0xD06C,0x4A9E,0x81,0x8D,0x5D,0x31,0x80,0xEB,0x8D,0x99);


MIDL_DEFINE_GUID(IID, IID_IApplication,0xEE3E904C,0x6AA0,0x401A,0xBC,0xB5,0xA9,0x36,0xE3,0xB7,0x17,0xF6);


MIDL_DEFINE_GUID(IID, IID_IGrammaticalSymbol,0xB0E9D7B9,0xECC5,0x44bd,0x8F,0x18,0xBD,0x34,0x99,0xB7,0x3D,0xC2);


MIDL_DEFINE_GUID(IID, IID_IGrammaticalVariant,0x6AF2BCFC,0xCCFF,0x40a2,0x83,0xC6,0x8A,0xD7,0x25,0xEA,0xE4,0x8A);


MIDL_DEFINE_GUID(IID, IID_IGrammaticalRule,0x092C0FB9,0x67A7,0x4C8A,0x9C,0x6E,0xF1,0x25,0xCD,0x40,0xC4,0x98);


MIDL_DEFINE_GUID(IID, IID_IGrammar,0xF27BAF30,0x1371,0x49f6,0xB1,0x2D,0xAD,0xCA,0x02,0xC9,0x4E,0x05);


MIDL_DEFINE_GUID(IID, IID_IParser,0x763009C4,0x1A1B,0x4185,0x96,0xA6,0x2B,0xFA,0xEF,0x10,0x6F,0x0D);


MIDL_DEFINE_GUID(IID, LIBID_mtdev,0xB5708153,0xCD7A,0x416D,0x90,0x22,0xD4,0x6E,0x48,0x6C,0x53,0xF0);


MIDL_DEFINE_GUID(CLSID, CLSID_Parser,0x0217A734,0x4550,0x4CD9,0xA8,0xA2,0xF6,0x06,0x6E,0x20,0x16,0x40);


MIDL_DEFINE_GUID(CLSID, CLSID_ProjectDocument,0x206DDA10,0x728E,0x4C72,0x81,0xCC,0x36,0xFB,0x3B,0xE8,0x09,0x16);


MIDL_DEFINE_GUID(CLSID, CLSID_SourceDocument,0x0314E4F6,0xE6C5,0x4773,0x84,0xD4,0xB7,0x1E,0x89,0x45,0x85,0x1B);


MIDL_DEFINE_GUID(CLSID, CLSID_Application,0xADC6D969,0xC5D5,0x49E4,0xA7,0x5C,0xF5,0x81,0x56,0xBD,0xDB,0xC9);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

