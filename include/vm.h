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
    char* allocated_strings[sizeof(u8)];
    char* strings[sizeof(u8)];
    u8 current_allocated_string;
    u8 current_string;

    Stack stack;
} VM;

void push_to_stack(Stack*, BASE_T);
BASE_T pop_from_stack(Stack*);
void debug_stack(Stack*);

u8 save_string(VM*);
u8 allocate_string(VM*, char*);
void destroy_vm(VM*);
void execute_byte(VM*, OpCode);
void execute(Program*);

#endif // VM_H