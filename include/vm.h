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

// CallStack
typedef struct {
    u64 caller_site;
    u64 callee;
    usize stack_start;
} StackFrame;

#define MAX_CALLSTACK_SIZE (10 * 1024)
typedef struct {
    StackFrame storage[MAX_CALLSTACK_SIZE];
    usize sp;
} CallStack;

// VM
typedef struct {
    usize pc;
    Program* program;
    BASE_T registers[32];
    char* strings[sizeof(BASE_T)];
    BASE_T current_string;

    Stack stack;
    CallStack call_stack;
} VM;

void push_to_call_stack(CallStack*, StackFrame);
StackFrame pop_from_call_stack(CallStack*);
StackFrame * current_stack_frame(CallStack*);

void push_to_stack(Stack*, u8);
void push_n_to_stack(Stack*, usize, u8*);
void push_u64_to_stack(Stack*, u64);

u8 pop_from_stack(VM *);
void pop_n_from_stack(VM *, usize n, u8* out);
void pop_n_from_stack(VM *, usize n, u8* out);
u64 pop_u64_from_stack(VM *);

u8* peek_n_from_stack(VM *, usize n);
u8* peek_n_from_stack_with_offset(VM *, usize offset, usize n);

u64 get_next_u64_from_program(VM*);
u64 get_next_u8_from_program(VM*);

void debug_stack(Stack*);

const char *save_string(VM*);
void destroy_vm(VM*);
void execute_byte(VM*, OpCode);
void execute(Program*);
void debug_execute(Program*);

#endif // VM_H
