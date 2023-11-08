#ifndef VM_H
#define VM_H
#include "core.h"
#include "opcodes.h"
#include "program.h"

// Stack
#define MB *1024
#define MAX_STACK_SIZE (2 MB)/sizeof(BASE_T)
typedef struct {
    BASE_T storage[MAX_STACK_SIZE];
    usize sp;
} Stack;

// VM
typedef struct {
    usize pc;
    Program* program;
    BASE_T registers[32];
    char* strings[sizeof(u8)];
    u8 current_string;

    Stack stack;
} VM;

void push_to_stack(Stack*, BASE_T);
BASE_T pop_from_stack(Stack*);

u8 save_string(VM*);
void destroy_vm(VM*);
void execute_byte(VM*, OpCode);
void execute(Program*);

#endif // VM_H