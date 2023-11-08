#include "program.h"

// A print program that can be used for debugging.
// It figures out the boundaries of the instructions and replaces them with
// their names.

void print_program(Program* p) {
    if(!p) {
        ERROR("Printing a NULL pointer to program.");
    }

    #if DEBUG
    printf("INFO: Program has %zu bytes\n", p->size);
    #endif // DEBUG

    for (usize i = 0; i < p->size; i++) {
        printf("[0x%02zx] -> 0x%x\n", i, p->code[i]);
    }
}

Program* create_program() {
    Program* p = malloc(sizeof(Program));
    if (p == NULL) {
        ERROR("Could not allocate the memory for program");
    }

    return p;
}

void destroy_program(Program* program, bool and_code) {
    if (and_code) {
        free(program->code);
    }

    free(program);
}