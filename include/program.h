#ifndef PROGRAM_H
#define PROGRAM_H
#include "core.h"

// Code stream
typedef struct {
    usize size;
    u8* code;
} Program;

void print_program(Program*);
Program* create_program();
void destroy_program(Program*, bool and_code);

#endif // PROGRAM_H