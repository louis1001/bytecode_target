#include "opcodes.h"
#include <stdio.h>
#include <string.h>

char *opcode_to_str(OpCode op) {
    #define X(name, val) case name: return #name;
    switch (op) {
        OPCODES
        default: return NULL;
    }
    #undef X
}

void print_opcode(OpCode op) {
    printf("%s", opcode_to_str(op));
}

bool string_to_opcode(OpCode *dst, char *str) {
    #define X(name, val) else if (strcasecmp(str, #name) == 0) {\
        *dst = name;\
        return true; \
    }

    if (str == NULL) { // Why would it? Just to pad the macro
        *dst = NOP;
    }
    OPCODES

    #undef X

    return false;
}
