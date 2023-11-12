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
    char* allocated_strings[sizeof(BASE_T)];
    char* strings[sizeof(BASE_T)];
    BASE_T current_allocated_string;
    BASE_T current_string;

    Stack stack;
} VM;

void push_to_stack(Stack*, BASE_T);
BASE_T pop_from_stack(Stack*);
void debug_stack(Stack*);

const char *save_string(VM*);
BASE_T allocate_string(VM*, char*);
void destroy_vm(VM*);
void execute_byte(VM*, OpCode);
void execute(Program*);

#endif // VM_H