#include "vm.h"
#include "opcodes.h"
#include <string.h>

void push_to_stack(Stack* st, BASE_T value) {
    if (st->sp >= MAX_STACK_SIZE) {
        ERROR("Max stack size exceeded.");
    }

    st->storage[st->sp++] = value;
}

BASE_T pop_from_stack(Stack* st) {
    if (st->sp <= 0) {
        ERROR("Not enough elements on the stack.");
    }

    return st->storage[--st->sp];
}

u8 save_string(VM* vm) {
    //      C automatically will handle the zero-terminated string
    //          within the code array.
    char* str = (char*) &(vm->program->code[vm->pc]);

    u8 index = vm->current_string++;
    vm->strings[index] = str;

    return index;
}

u8 allocate_string(VM* vm, char* str) {
    //      Find the length of the string, plus
    //          the '\0' character.
    unsigned long len = strlen(str);
    vm->pc += len+1;

    //      Allocate the string on the heap
    char* allocated = malloc(len);
    if (allocated == NULL) {
        ERROR("Couldn't allocate string");
    } else {
        #if DEBUG
            printf("INFO: Allocated string %s\n", str);
        #endif // DEBUG
    }

    //      Copy the memory from code to the heap
    strlcpy(allocated, str, len+1);

    //      Store the string pointer so it can be freed later
    u8 index = vm->current_string++;
    vm->strings[index] = allocated;
    vm->allocated_strings[vm->current_allocated_string++] = allocated;

    return index;
}

void destroy_vm(VM* vm) {
    for (int i = 0; i < vm->current_allocated_string; i++) {
        char* ptr = vm->allocated_strings[i];
        #if VERBOSE
            printf("INFO: Freeing string %s\n", ptr);
            // flush stdout
            fflush(stdout);
        #endif // VERBOSE
        
        if(ptr != NULL) {
            free(ptr);
        }
    }
}

// Execution

void execute_byte(VM *vm, OpCode op) {
    switch (op) {
        case NOP: {
            break;
        }
        case STR: {
            #if VERBOSE
            printf("[%zx] Allocating a string\n", vm->pc);
            #endif
            u8 str = save_string(vm);
            push_to_stack(&vm->stack, str);
            break;
        }
        case PNT: {
            #if VERBOSE
            printf("[%zx] Printing string\n", vm->pc);
            #endif
            u8 index = pop_from_stack(&vm->stack);
            printf("%s", vm->strings[index]);
            break;
        }
        case ADD: {
            #if VERBOSE
            printf("[%zx] Adding two ints\n", vm->pc);
            #endif
            u8 a = pop_from_stack(&vm->stack);
            u8 b = pop_from_stack(&vm->stack);
            push_to_stack(&vm->stack, a + b);
            break;
        }
        case JMP: {
            #if VERBOSE
            printf("[%zx] Jumping\n", vm->pc);
            #endif
            u8 target = pop_from_stack(&vm->stack);
            vm->pc = (usize) target; //FIXME: Do 32bit targets
            break;
        }
        case JPT: {
            #if VERBOSE
            printf("[%zx] Jumping if true\n", vm->pc);
            #endif
            u8 target = pop_from_stack(&vm->stack);

            u8 condition = pop_from_stack(&vm->stack);
            if (condition) {
                vm->pc = (usize) target; //FIXME: Do 32bit targets
            }
            break;
        }
        case EQU: {
            printf("[%zx] Checking if equal\n", vm->pc);
            u8 a = pop_from_stack(&vm->stack);
            u8 b = pop_from_stack(&vm->stack);

            push_to_stack(&vm->stack, (u8) a == b);
            break;
        }
        case LT: {
            #if VERBOSE
            printf("[%zx] Checking if less than\n", vm->pc);
            #endif
            u8 a = pop_from_stack(&vm->stack);
            u8 b = pop_from_stack(&vm->stack);

            push_to_stack(&vm->stack, (u8) a < b);
            break;
        }
        case DBG: {
            u8 num = pop_from_stack(&vm->stack);
            printf("Debug: %u\n", num);
            break;
        }
        case EXT: {
            #if VERBOSE
            printf("[%zx] Exiting\n", vm->pc);
            #endif
            vm->pc = vm->program->size;
            break;
        }
        case INC: {
            #if VERBOSE
            printf("[%zx] Incrementing the top stack value\n", vm->pc);
            #endif
            u8 value = pop_from_stack(&vm->stack);
            push_to_stack(&vm->stack, value + 1);
            break;
        }
        case DEC: {
            #if VERBOSE
            printf("[%zx] Decrementing the top stack value\n", vm->pc);
            #endif
            u8 value = pop_from_stack(&vm->stack);
            push_to_stack(&vm->stack, value - 1);
            break;
        }
        case PSH: {
            #if VERBOSE
            printf("[%zx] Pushing to the stack\n", vm->pc);
            #endif
            u8 value = vm->program->code[vm->pc++];
            push_to_stack(&vm->stack, value);
            break;
        }
        default: {
            ERROR("Invalid opcode 0x%x", op);
            break;
        }
    }
}

void execute(Program *program) {
    VM vm = {0};
    vm.program = program;

    while (vm.pc < program->size) {
        if (vm.pc < 0) { ERROR("Invalid program counter (%zu) into bytecode array.\n", vm.pc); }

        OpCode op = program->code[vm.pc++];

        execute_byte(&vm, op);
    }

    destroy_vm(&vm);
}