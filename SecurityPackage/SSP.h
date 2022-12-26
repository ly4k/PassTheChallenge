#pragma once

#define SECURITY_WIN32

#include <Windows.h>
#include <ntsecapi.h>
#include <sspi.h>
#include <ntsecpkg.h>

DWORD WINAPI CreateServer(LPVOID lParam);