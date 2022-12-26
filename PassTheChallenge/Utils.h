#pragma once

#include <Windows.h>
#include <stdio.h>

inline int HexCharToBin(const CHAR hex, BYTE* out);
BYTE* HexDecode(LPSTR hex);
BOOL DecodeChallenge(LPCSTR _challenge, BYTE** domain, BYTE** username, BYTE** serverName, BYTE** serverChallenge);