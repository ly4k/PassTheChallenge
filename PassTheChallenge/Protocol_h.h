

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for Protocol.IDL:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __Protocol_h_h__
#define __Protocol_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __PassTheChallenge_INTERFACE_DEFINED__
#define __PassTheChallenge_INTERFACE_DEFINED__

/* interface PassTheChallenge */
/* [implicit_handle][version][uuid] */ 

#pragma once
int Ping( void);

int NtlmIumProtectCredential( 
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [in] */ BYTE NTHash[ 16 ],
    /* [out] */ USHORT *EncryptedSize,
    /* [size_is][size_is][out] */ BYTE **EncryptedBlob);

int NtlmIumLm20GetNtlm3ChallengeResponse( 
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [string][in] */ WCHAR *UserName,
    /* [string][in] */ WCHAR *LogonDomainName,
    /* [string][in] */ WCHAR *ServerName,
    /* [size_is][in] */ BYTE *ChallengeToClient,
    /* [in] */ USHORT EncryptedSize,
    /* [size_is][in] */ BYTE *EncryptedBlob,
    /* [out] */ BYTE UserSessionKey[ 16 ],
    /* [out] */ USHORT *Ntlm3ResponseLength,
    /* [size_is][size_is][out] */ BYTE **Ntlm3Response);

int NtlmIumCalculateNtResponse( 
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [size_is][in] */ BYTE *ChallengeToClient,
    /* [in] */ USHORT EncryptedSize,
    /* [size_is][in] */ BYTE *EncryptedBlob,
    /* [out] */ BYTE Response[ 24 ]);

int NtlmIumCompareCredentials( 
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [in] */ USHORT EncryptedSize1,
    /* [size_is][in] */ BYTE *EncryptedBlob1,
    /* [in] */ BOOL IsEncrypted,
    /* [in] */ USHORT EncryptedSize2,
    /* [size_is][in] */ BYTE *EncryptedBlob2,
    /* [out] */ BOOL *Matches);

long Shutdown( void);


extern handle_t hBinding;


extern RPC_IF_HANDLE PassTheChallenge_v1_0_c_ifspec;
extern RPC_IF_HANDLE PassTheChallenge_v1_0_s_ifspec;
#endif /* __PassTheChallenge_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


