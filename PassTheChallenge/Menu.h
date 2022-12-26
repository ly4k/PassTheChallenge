#pragma once

#include <Windows.h>
#include <stdio.h>

typedef int COMMAND_PTR(int, LPCCH*);

typedef struct _PARAM {
    LPCCH name;
    LPCCH description;
} PARAM, * PPARAM;

typedef struct _COMMAND {
    LPCCH name;
    LPCCH description;
    PPARAM params;
    COMMAND_PTR* func;
} COMMAND, * PCOMMAND;

COMMAND COMMANDS[];
void PrintMenu();