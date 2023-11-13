#ifndef VM_H
#define VM_H
#include "core.h"
#include "opcodes.h"
#include "program.h"

// Stack
#define MB *1024
#define MAX_STACK_SIZE (2 MB)
typedef struct {
    u8 storage[MAX_STACK_SIZE];
    usize sp;
} Stack;

// VM
typedef struct {
    usize pc;
    Program* program;
    BASE_T registers[32];
    char* strings[sizeof(BASE_T)];
    BASE_T current_string;

    Stack stack;
} VM;

void push_to_stack(Stack*, u8);
void push_u64_to_stack(Stack*, u64);

u8 pop_from_stack(Stack*);
u64 pop_u64_from_stack(Stack*);

u64 get_next_u64_from_program(VM*);
u64 get_next_u8_from_program(VM*);

void debug_stack(Stack*);

const char *save_string(VM*);
void destroy_vm(VM*);
void execute_byte(VM*, OpCode);
void execute(Program*);
void debug_execute(Program*);

#endif // VM_H
