#define SECURITY_WIN32

#include <Windows.h>
#include <ntsecapi.h>
#include <sspi.h>
#include <ntsecpkg.h>

#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "ntdll.lib")

#include "../PassTheChallenge/Protocol_h.h"
#include "../PassTheChallenge/Constants.h"

#include "Types.h"

static BOOL g_Shutdown = FALSE;

int Ping()
{
    return 1;
}

int NtlmIumProtectCredential(
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [in] */ BYTE NTHash[16],
    /* [out] */ USHORT* EncryptedSize,
    /* [size_is][size_is][out] */ BYTE** EncryptedBlob)
{
    ULONG ulCode = 0;
    CLIENT_CALL_RETURN rpcReturnValue = { NULL };
    RPC_STATUS rpcStatus = 0;
    RPC_BINDING_HANDLE newContextHandle = NULL;
    MSV1_0_LM3_RESPONSE Lm3Response = { 0 };
    LM_SESSION_KEY LmSessionKey = { 0 };

    if (ProxyInfo == 0 || ContextHandle == 0) {
        ulCode = 1;
        goto out;
    }

    rpcStatus = RpcBindingCopy((RPC_BINDING_HANDLE)ContextHandle, &newContextHandle);
    if (rpcStatus != RPC_S_OK) {
        ulCode = 2;
        goto out;
    }


    PMSV1_0_SECRETS_WRAPPER secretsWrapper = malloc(sizeof(MSV1_0_SECRETS_WRAPPER));
    if (secretsWrapper == NULL) {
        ulCode = 3;
        goto out;
    }

    memset(secretsWrapper, 0, sizeof(MSV1_0_SECRETS_WRAPPER));
    secretsWrapper->NtPasswordPresent = TRUE;
    memcpy(secretsWrapper->PlaintextSecrets.NtOwfPassword.Data, NTHash, MSV1_0_NT_OWF_PASSWORD_LENGTH);

    RpcTryExcept{
        rpcReturnValue = NdrClientCall3((PMIDL_STUBLESS_PROXY_INFO)ProxyInfo, 1, 0, newContextHandle, secretsWrapper); // NtlmIumProtectCredential
    }
    RpcExcept(1); {
        ulCode = RpcExceptionCode();
        goto out;
    }
    RpcEndExcept;

    if (secretsWrapper->IsEncrypted == FALSE) {
        ulCode = 4;
        goto out;
    }

    if (rpcReturnValue.Simple != 0) {
        goto out;
    }

    *EncryptedSize = secretsWrapper->EncryptedSize;
    *EncryptedBlob = secretsWrapper->encryptedBlob;

out:
    return ulCode;
}

int NtlmIumLm20GetNtlm3ChallengeResponse(
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [string][in] */ WCHAR* UserName,
    /* [string][in] */ WCHAR* LogonDomainName,
    /* [string][in] */ WCHAR* ServerName,
    /* [size_is][in] */ BYTE* ChallengeToClient,
    /* [in] */ USHORT EncryptedSize,
    /* [size_is][in] */ BYTE* EncryptedBlob,
    /* [out] */ BYTE UserSessionKey[16],
    /* [out] */ USHORT* Ntlm3ResponseLength,
    /* [size_is][size_is][out] */ BYTE** Ntlm3Response)
{
    ULONG ulCode = 0;
    CLIENT_CALL_RETURN rpcReturnValue = { NULL };
    RPC_STATUS rpcStatus = 0;
    RPC_BINDING_HANDLE newContextHandle = NULL;
    MSV1_0_LM3_RESPONSE Lm3Response = { 0 };
    LM_SESSION_KEY LmSessionKey = { 0 };
    UNICODE_STRING uUsername = { 0 };
    UNICODE_STRING uDomain = { 0 };
    UNICODE_STRING uServerName = { 0 };
    MSV1_0_SECRETS_WRAPPER secretsWrapper = { 0 };

    if (ProxyInfo == 0 || ContextHandle == 0) {
        ulCode = 1;
        goto out;
    }

    RtlInitUnicodeString(&uUsername, UserName);
    RtlInitUnicodeString(&uDomain, LogonDomainName);
    RtlInitUnicodeString(&uServerName, ServerName);

    rpcStatus = RpcBindingCopy((RPC_BINDING_HANDLE)ContextHandle, &newContextHandle);
    if (rpcStatus != RPC_S_OK) {
        ulCode = 2;
        goto out;
    }

    memset(&secretsWrapper, 0, sizeof(secretsWrapper));

    secretsWrapper.IsEncrypted = TRUE;
    secretsWrapper.NtPasswordPresent = TRUE;
    secretsWrapper.EncryptedSize = EncryptedSize;
    memcpy(secretsWrapper.encryptedBlob, EncryptedBlob, EncryptedSize);
    
    RpcTryExcept{
        rpcReturnValue = NdrClientCall3((PMIDL_STUBLESS_PROXY_INFO)ProxyInfo, 2, 0, newContextHandle, &secretsWrapper, &uUsername, &uDomain, &uServerName, ChallengeToClient, Ntlm3ResponseLength, Ntlm3Response, &Lm3Response, UserSessionKey, &LmSessionKey); // NtlmIumLm20GetNtlm3ChallengeResponse
    }
    RpcExcept(1); {
        ulCode = RpcExceptionCode();
        goto out;
    }
    RpcEndExcept;

    if (rpcReturnValue.Simple != 0) {
        ulCode = (ULONG)rpcReturnValue.Simple;
        goto out;
    }

out:
    return ulCode;
}

int NtlmIumCalculateNtResponse(
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [size_is][in] */ BYTE* ChallengeToClient,
    /* [in] */ USHORT EncryptedSize,
    /* [size_is][in] */ BYTE* EncryptedBlob,
    /* [out] */ BYTE Response[24])
{
    ULONG ulCode = 0;
    CLIENT_CALL_RETURN rpcReturnValue = { NULL };
    RPC_STATUS rpcStatus = 0;
    RPC_BINDING_HANDLE newContextHandle = NULL;
    MSV1_0_LM3_RESPONSE Lm3Response = { 0 };
    LM_SESSION_KEY LmSessionKey = { 0 };
    MSV1_0_SECRETS_WRAPPER secretsWrapper = { 0 };

    if (ProxyInfo == 0 || ContextHandle == 0) {
        ulCode = 1;
        goto out;
    }

    rpcStatus = RpcBindingCopy((RPC_BINDING_HANDLE)ContextHandle, &newContextHandle);
    if (rpcStatus != RPC_S_OK) {
        ulCode = 2;
        goto out;
    }

    memset(&secretsWrapper, 0, sizeof(secretsWrapper));

    secretsWrapper.IsEncrypted = TRUE;
    secretsWrapper.NtPasswordPresent = TRUE;
    secretsWrapper.EncryptedSize = EncryptedSize;
    memcpy(secretsWrapper.encryptedBlob, EncryptedBlob, EncryptedSize);

    RpcTryExcept{
        rpcReturnValue = NdrClientCall3((PMIDL_STUBLESS_PROXY_INFO)ProxyInfo, 3, 0, newContextHandle, ChallengeToClient, &secretsWrapper, Response); // NtlmIumCalculateNtResponse
    }
    RpcExcept(1); {
        ulCode = RpcExceptionCode();
        goto out;
    }
    RpcEndExcept;

    if (rpcReturnValue.Simple != 0) {
        ulCode = (ULONG)rpcReturnValue.Simple;
        goto out;
    }

out:
    return ulCode;
}

int NtlmIumCompareCredentials(
    /* [in] */ DWORD64 ContextHandle,
    /* [in] */ DWORD64 ProxyInfo,
    /* [in] */ USHORT EncryptedSize1,
    /* [size_is][in] */ BYTE* EncryptedBlob1,
    /* [in] */ BOOL IsEncrypted,
    /* [in] */ USHORT EncryptedSize2,
    /* [size_is][in] */ BYTE* EncryptedBlob2,
    /* [out] */ BOOL* Matches)
{
    ULONG ulCode = 0;
    CLIENT_CALL_RETURN rpcReturnValue = { NULL };
    RPC_STATUS rpcStatus = 0;
    RPC_BINDING_HANDLE newContextHandle = NULL;
    MSV1_0_SECRETS_WRAPPER secretsWrapper1, secretsWrapper2;

    if (ProxyInfo == 0 || ContextHandle == 0) {
        ulCode = 1;
        goto out;
    }

    rpcStatus = RpcBindingCopy((RPC_BINDING_HANDLE)ContextHandle, &newContextHandle);
    if (rpcStatus != RPC_S_OK) {
        ulCode = 2;
        goto out;
    }

    memset(&secretsWrapper1, 0, sizeof(secretsWrapper1));
    memset(&secretsWrapper2, 0, sizeof(secretsWrapper2));

    if (IsEncrypted == FALSE) {
        secretsWrapper1.NtPasswordPresent = TRUE;
        memcpy(secretsWrapper1.PlaintextSecrets.NtOwfPassword.Data, EncryptedBlob2, MSV1_0_NT_OWF_PASSWORD_LENGTH);

        RpcTryExcept{
            rpcReturnValue = NdrClientCall3((PMIDL_STUBLESS_PROXY_INFO)ProxyInfo, 1, 0, newContextHandle, &secretsWrapper1); // NtlmIumProtectCredential
        }
        RpcExcept(1); {
            ulCode = RpcExceptionCode();
            goto out;
        }
        RpcEndExcept;

        if (secretsWrapper1.IsEncrypted == FALSE) {
            ulCode = 3;
            goto out;
        }

        if (rpcReturnValue.Simple != 0) {
            goto out;
        }
    }
    else {
        secretsWrapper1.IsEncrypted = TRUE;
        secretsWrapper1.NtPasswordPresent = TRUE;
        secretsWrapper1.EncryptedSize = EncryptedSize2;
        memcpy(secretsWrapper1.encryptedBlob, EncryptedBlob2, EncryptedSize2);
    }

    secretsWrapper2.IsEncrypted = TRUE;
    secretsWrapper2.NtPasswordPresent = TRUE;
    secretsWrapper2.EncryptedSize = EncryptedSize1;
    memcpy(secretsWrapper2.encryptedBlob, EncryptedBlob1, EncryptedSize1);

    RpcTryExcept{
        rpcReturnValue = NdrClientCall3((PMIDL_STUBLESS_PROXY_INFO)ProxyInfo, 9, 0, newContextHandle, &secretsWrapper1, &secretsWrapper2, Matches, NULL, NULL); // NtlmIumCompareCredentials
    }
    RpcExcept(1); {
        ulCode = RpcExceptionCode();
        goto out;
    }
    RpcEndExcept;

out:
    return ulCode;
}

long Shutdown()
{
    g_Shutdown = TRUE;
    return 0;
}


RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE hHandle, void* ptr)
{
    return RPC_S_OK;
}


DWORD WINAPI CreateServer(LPVOID lParam)
{
    RPC_STATUS status;

    status = RpcServerUseProtseqEp(
        (RPC_WSTR)L"ncalrpc",
        RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
        (RPC_WSTR)L"PtC",
        NULL);

    if (status != RPC_S_OK && status != RPC_S_DUPLICATE_ENDPOINT) {
        return -1;
    }

    status = RpcServerRegisterIf2(
        PassTheChallenge_v1_0_s_ifspec,              // Interface to register.
        NULL,                                // Use the MIDL generated entry-point vector.
        NULL,                                // Use the MIDL generated entry-point vector.
        RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH | RPC_IF_ALLOW_LOCAL_ONLY | RPC_IF_AUTOLISTEN, // Forces use of security callback.
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,      // Use default number of concurrent calls.
        (unsigned)0x100000,                        // Infinite max size of incoming data blocks.
        SecurityCallback);                   // Naive security callback.


    if (status != RPC_S_OK) {
        return -1;
    }

    while (TRUE) {
        if (g_Shutdown == TRUE) break;
        Sleep(10000);
    }

    status = RpcServerUnregisterIf(PassTheChallenge_v1_0_s_ifspec, NULL, 1);

    return status;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

void __RPC_USER midl_user_free(void* p)
{
    free(p);
}
