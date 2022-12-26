#pragma once

#include <Windows.h>
#include <ntsecapi.h>

#include "../PassTheChallenge/Constants.h"

typedef struct _MSV1_0_NT_OWF_PASSWORD {
    UCHAR Data[MSV1_0_NT_OWF_PASSWORD_LENGTH];
} MSV1_0_NT_OWF_PASSWORD;

typedef struct _MSV1_0_LM_OWF_PASSWORD {
    UCHAR Data[MSV1_0_LM_OWF_PASSWORD_LENGTH];
} MSV1_0_LM_OWF_PASSWORD;

typedef struct _MSV1_0_SHA_OWF_PASSWORD {
    UCHAR Data[MSV1_0_SHA_OWF_PASSWORD_LENGTH];
} MSV1_0_SHA_OWF_PASSWORD;

typedef struct _PLAINTEXT_SECRETS
{
    MSV1_0_NT_OWF_PASSWORD NtOwfPassword;
    MSV1_0_LM_OWF_PASSWORD LmOwfPassword;
    MSV1_0_SHA_OWF_PASSWORD ShaOwfPassword;
} PLAINTEXT_SECRETS, * PPLAINTEXT_SECRETS;

typedef struct _MSV1_0_SECRETS_WRAPPER
{
    DWORD64 _Reserved0;
    BOOLEAN IsEncrypted;
    BOOLEAN NtPasswordPresent;
    BOOLEAN LmPasswordPresent;
    BOOLEAN ShaPasswordPresent;
    BOOLEAN CredKeyIsPresent;
    BOOLEAN _Reserved1;
    BOOLEAN _Reserved2;
    BOOLEAN _Reserved3;
    MSV1_0_CREDENTIAL_KEY_TYPE CredentialKeyType;
    USHORT EncryptedSize;
    MSV1_0_CREDENTIAL_KEY CredentialKeySecret;
    union
    {
        PLAINTEXT_SECRETS PlaintextSecrets;
        BYTE encryptedBlob[308];
    };
} MSV1_0_SECRETS_WRAPPER, * PMSV1_0_SECRETS_WRAPPER;

typedef struct {
    UCHAR Response[MSV1_0_NTLM3_RESPONSE_LENGTH];
    UCHAR ChallengeFromClient[MSV1_0_CHALLENGE_LENGTH];
} MSV1_0_LM3_RESPONSE, * PMSV1_0_LM3_RESPONSE;

typedef struct {
    UCHAR Data[MSV1_0_USER_SESSION_KEY_LENGTH];
} USER_SESSION_KEY, * PUSER_SESSION_KEY;

typedef struct _NT_CHALLENGE {
    UCHAR Data[MSV1_0_CHALLENGE_LENGTH];
} NT_CHALLENGE, * PNT_CHALLENGE;

typedef NT_CHALLENGE LM_SESSION_KEY;