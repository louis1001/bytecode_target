#include "program.h"

// A print program that can be used for debugging.
void print_program(Program* p) {
    if(!p) {
        ERROR("Printing a NULL pointer to program.");
    }

    LOG("Program has %zu bytes\n", p->size * sizeof(u8));

    u8 *code = (u8*)p->code;

    usize len = p->size * sizeof(u8);
    for (usize i = 0; i < len; i++) {
        printf("[%02zx]: 0x%02x\n", i, code[i]);
    }
}

Program create_program(void) {
    Program p = {0};

    return p;
}

void destroy_program(Program* program) {
    free(program->code);
}
