#include "opcodes.h"
#include <stdio.h>

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
