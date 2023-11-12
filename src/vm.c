#include "vm.h"
#include "opcodes.h"
#include <string.h>

void push_to_stack(Stack* st, BASE_T value) {
    ASSERT(st->sp < MAX_STACK_SIZE, "Max stack size exceeded.\n");

    st->storage[st->sp++] = value;
}

BASE_T pop_from_stack(Stack* st) {
    ASSERT(st->sp > 0, "Not enough elements on the stack.\n");

    return st->storage[--st->sp];
}

void debug_stack(Stack *stack) {
    printf("STACK:\n");
    for (usize i = 0; i < stack->sp; i++) {
        printf("[%03zu] %llu\n", i, stack->storage[i]);
    }
    printf("BOTTOM OF STACK\n");
}

BASE_T allocate_string(VM* vm, char* str) {
    //      Find the length of the string, plus
    //          the '\0' character.
    unsigned long len = strlen(str);
    vm->pc += len+1;

    //      Allocate the string on the heap
    char* allocated = malloc(len);
    if (allocated == NULL) {
        ERROR("Couldn't allocate string");
    } else {
        LOG("Allocated string %s\n", str);
    }

    //      Copy the memory from code to the heap
    strlcpy(allocated, str, len+1);

    //      Store the string pointer so it can be freed later
    BASE_T index = vm->current_string++;
    vm->strings[index] = allocated;
    vm->allocated_strings[vm->current_allocated_string++] = allocated;

    return index;
}

void destroy_vm(VM* vm) {
    for (BASE_T i = 0; i < vm->current_allocated_string; i++) {
        char* ptr = vm->allocated_strings[i];
        VERBOSE_LOG("Freeing string %s\n", ptr);
        
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
            VERBOSE_LOG("[%zx] Saving a string\n", vm->pc);

            u8 str = save_string(vm);
            push_to_stack(&vm->stack, str);
            break;
        }
        case PNT: {
            VERBOSE_LOG("[%zx] Printing string\n", vm->pc);

            u8 index = pop_from_stack(&vm->stack);
            printf("%s", vm->strings[index]);
            break;
        }
        case ADD: {
            VERBOSE_LOG("[%zx] Adding two ints\n", vm->pc);

            BASE_T a = pop_from_stack(&vm->stack);
            BASE_T b = pop_from_stack(&vm->stack);
            push_to_stack(&vm->stack, a + b);
            break;
        }
        case JMP: {
            VERBOSE_LOG("[%zx] Jumping\n", vm->pc);

            BASE_T target = pop_from_stack(&vm->stack);
            vm->pc = (usize) target; //FIXME: Do 32bit targets
            break;
        }
        case JPT: {
            VERBOSE_LOG("[%zx] Jumping if true\n", vm->pc);

            BASE_T target = pop_from_stack(&vm->stack);

            BASE_T condition = pop_from_stack(&vm->stack);
            if (condition) {
                vm->pc = (usize) target; //FIXME: Do 32bit targets
            }
            break;
        }
        case JPF: {
            VERBOSE_LOG("[%zx] Jumping if true\n", vm->pc);

            BASE_T target = pop_from_stack(&vm->stack);

            BASE_T condition = pop_from_stack(&vm->stack);
            if (!condition) {
                vm->pc = (usize) target; //FIXME: Do 32bit targets
            }
            break;
        }
        case EQU: {
            VERBOSE_LOG("[%zx] Checking if equal\n", vm->pc);

            BASE_T a = pop_from_stack(&vm->stack);
            BASE_T b = pop_from_stack(&vm->stack);

            push_to_stack(&vm->stack, (BASE_T) a == b);
            break;
        }
        case LT: {
            VERBOSE_LOG("[%zx] Checking if less than\n", vm->pc);

            BASE_T a = pop_from_stack(&vm->stack);
            BASE_T b = pop_from_stack(&vm->stack);

            push_to_stack(&vm->stack, (BASE_T) b < a);
            break;
        }
        case DBG: {
            BASE_T num = pop_from_stack(&vm->stack);
            printf("Debug: %llu\n", num);
            break;
        }
        case EXT: {
            VERBOSE_LOG("[%zx] Exiting\n", vm->pc);

            vm->pc = vm->program->size;
            break;
        }
        case INC: {
            VERBOSE_LOG("[%zx] Incrementing the top stack value\n", vm->pc);

            BASE_T value = pop_from_stack(&vm->stack);
            push_to_stack(&vm->stack, value + 1);
            break;
        }
        case DEC: {
            VERBOSE_LOG("[%zx] Decrementing the top stack value\n", vm->pc);

            BASE_T value = pop_from_stack(&vm->stack);
            push_to_stack(&vm->stack, value - 1);
            break;
        }
        case PSH: {
            VERBOSE_LOG("[%zx] Pushing to the stack\n", vm->pc);

            BASE_T value = vm->program->code[vm->pc++];
            push_to_stack(&vm->stack, value);
            break;
        }
        case DUP: {
            VERBOSE_LOG("[%zx] Duplicating the top stack value\n", vm->pc);

            BASE_T value = pop_from_stack(&vm->stack);
            push_to_stack(&vm->stack, value);
            push_to_stack(&vm->stack, value);
            break;
        }
        case ROT: {
            VERBOSE_LOG("[%zx] Duplicating the top stack value\n", vm->pc);
            ASSERT(vm->stack.sp >= 3, "Stack has enough values");

            BASE_T a = pop_from_stack(&vm->stack);
            BASE_T b = pop_from_stack(&vm->stack);
            BASE_T c = pop_from_stack(&vm->stack);

            push_to_stack(&vm->stack, a);
            push_to_stack(&vm->stack, c);
            push_to_stack(&vm->stack, b);
            break;
        }
        case BKP: {
            VERBOSE_LOG("[%zx] Hit breakpoint\n", vm->pc);

            printf("The stack at this point:\n");
            debug_stack(&vm->stack);

            vm->pc = vm->program->size;
            break;
        }
        default: {
            char *opcode_name = opcode_to_str(op);
            if (opcode_name != NULL) {
                ERROR("Opcode %s not implemented\n", opcode_name);
            } else {
                ERROR("Invalid opcode 0x%x\n", op);
            }
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