#include "Menu.h"

PARAM CHALLENGE_PARAMS[] = {
    {"<addresses>", "<context handle>:<proxy info>"},
    {"<encrypted blob>", "<HEX>"},
    {"<server challenge>", "<UTF16_HEX domain>:<UTF16_HEX username>:<HEX server name>:<HEX server challenge>"},
    {NULL, NULL}
};

PARAM NTHASH_PARAMS[] = {
    {"<addresses>", "<context handle>:<proxy info>"},
    {"<encrypted blob>", "<HEX>"},
    {"[<server challenge>]", "If omitted, a static challenge of 1122334455667788 will be used"},
    {NULL, NULL}
};

PARAM PROTECT_PARAMS[] = {
    {"<addresses>", "<context handle>:<proxy info>"},
    {"<nt hash>", "<HEX>"},
    {NULL, NULL}
};

PARAM COMPARE_PARAMS[] = {
    {"<addresses>", "<context handle>:<proxy info>"},
    {"<encrypted blob>", "<HEX>"},
    {"<encrypted blob/NT hash>", "<HEX>"},
    {NULL, NULL}
};

COMMAND_PTR Inject, Ping, Challenge, Compare, NtHash, Protect;

COMMAND COMMANDS[] = {
    { "inject", "Inject module and start PtC-RPC server inside LSASS", NULL, Inject},
    { "ping", "Ping the PtC-RPC server inside LSASS", NULL, Ping },
    { "challenge", "Calculate NTLMv2 Response using encrypted credentials", CHALLENGE_PARAMS, Challenge },
    { "nthash", "Calculate NTLMv1 Response using encrypted credentials", NTHASH_PARAMS, NtHash },
    { "protect", "Convert NT hash to encrypted blob", PROTECT_PARAMS, Protect },
    { "compare", "Compare two encrypted blobs or an encrypted blob with a NT hash", COMPARE_PARAMS, Compare },
    { NULL, NULL }
};

LPCCH EXAMPLES[] = {
    "PtC.exe inject [<module>]",
    "PtC.exe ping",
    "PtC.exe challenge 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...] 6c0079[...]:610064[...]:020008[...]:66a98b[...]",
    "PtC.exe nthash 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...]",
    "PtC.exe protect 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...]",
    "PtC.exe compare 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...] 66a98b[...]",
    NULL
};

void PrintMenu() {
    printf("Usage: <command> [<parameters...>]\n");
    printf("\nCommands:\n");
    for (int i = 0; ; i++) {
        if (COMMANDS[i].name == NULL) break;
        printf("    %s - %s\n", COMMANDS[i].name, COMMANDS[i].description);

        if (COMMANDS[i].params != NULL) {
            for (int n = 0; ; n++) {
                if (COMMANDS[i].params[n].name == NULL) break;
                printf("      %s - %s\n", COMMANDS[i].params[n].name, COMMANDS[i].params[n].description);
            }
        }
    }
    printf("\nExamples:\n");
    for (int i = 0; ; i++) {
        if (EXAMPLES[i] == NULL) break;
        printf("    %s\n", EXAMPLES[i]);
    }
}

