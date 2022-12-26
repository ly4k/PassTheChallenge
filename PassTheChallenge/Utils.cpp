#include "Utils.h"

inline int HexCharToBin(const CHAR hex, BYTE* out)
{
    if (out == NULL)
        return 0;

    if (hex >= '0' && hex <= '9') {
        *out = hex - '0';
    }
    else if (hex >= 'A' && hex <= 'F') {
        *out = hex - 'A' + 10;
    }
    else if (hex >= 'a' && hex <= 'f') {
        *out = hex - 'a' + 10;
    }
    else {
        return 0;
    }

    return 1;
}

BYTE* HexDecode(LPSTR hex)
{
    DWORD hexLength = lstrlenA(hex);

    if (hexLength % 2 != 0) {
        printf("[-] Invalid hex length\n");
        return NULL;
    }

    BYTE* out = (BYTE*)LocalAlloc(LMEM_FIXED, hexLength / 2 + 2);
    DWORD i, n;
    BYTE b1, b2;

    if (out == NULL) {
        printf("[-] Failed to allocate buffer on the heap\n");
        return NULL;
    }

    for (i = 0, n = 0; i < hexLength; i += 2, n++) {
        if (!HexCharToBin(hex[i], &b1) || !HexCharToBin(hex[i + 1], &b2)) {
            LocalFree((HLOCAL)out);
            return NULL;
        }
        out[n] = (b1 << 4) | b2;
    }

    out[n] = '\0';
    out[n + 1] = '\0';

    return out;
}

BOOL DecodeChallenge(LPCSTR _challenge, BYTE** domain, BYTE** username, BYTE** serverName, BYTE** serverChallenge)
{
    if (domain == NULL || username == NULL || serverChallenge == NULL || serverName == NULL)
        return FALSE;

    *domain = NULL;
    *username = NULL;
    *serverChallenge = NULL;
    *serverName = NULL;

    DWORD challengeLength = lstrlenA(_challenge);
    LPSTR challenge = _strdup(_challenge);

    DWORD i, n;

    for (i = 0, n = 0; i < challengeLength; i++) {
        if (challenge[i] == ':') {
            challenge[i] = '\0';
            n++;
        }
    }

    if (n != 3) goto bad;

    *domain = HexDecode(challenge);
    if (*domain == NULL) goto bad;

    challenge += lstrlenA(challenge) + 1;
    *username = HexDecode(challenge);
    if (*username == NULL) goto bad;

    challenge += lstrlenA(challenge) + 1;
    *serverName = HexDecode(challenge);
    if (*serverName == NULL) goto bad;

    challenge += lstrlenA(challenge) + 1;
    *serverChallenge = HexDecode(challenge);
    if (*serverChallenge == NULL) goto bad;

    return TRUE;
bad:
    free(challenge);
    return FALSE;
}
