#include "vm.h"
#include "opcodes.h"
#include <string.h>

void push_to_stack(Stack* st, u8 value) {
    ASSERT(st->sp < MAX_STACK_SIZE, "Max stack size exceeded.\n");

    st->storage[st->sp++] = value;
}

void push_u64_to_stack(Stack* st, u64 value) {
    ASSERT(st->sp + sizeof(u64) < MAX_STACK_SIZE, "Max stack size exceeded.\n");

    memcpy(&st->storage[st->sp], &value, sizeof(u64));
    st->sp += sizeof(u64);
}

u8 pop_from_stack(Stack* st) {
    ASSERT(st->sp > 0, "Not enough elements on the stack.\n");

    return st->storage[--st->sp];
}

u64 pop_u64_from_stack(Stack* st) {
    ASSERT(st->sp >= sizeof(u64), "Not enough elements on the stack.\n");

    u64 value;
    memcpy(&value, &st->storage[st->sp - sizeof(u64)], sizeof(u64));
    st->sp -= sizeof(u64);
    return value;
}

void debug_stack(Stack *stack) {
    printf("STACK:\n");
    for (usize i = 0; i < stack->sp; i++) {
        printf("[%03zu] %hhu\n", i, stack->storage[i]);
    }
    printf("BOTTOM OF STACK\n");
}

u64 get_next_u64_from_program(VM *vm) {
    u64 value;
    memcpy(&value, &vm->program->code[vm->pc], sizeof(u64));
    vm->pc += sizeof(u64);
    return value;
}

u64 get_next_u8_from_program(VM *vm) {
    u8 value = vm->program->code[vm->pc++];
    return value;
}

void destroy_vm(VM* vm) {
    (void) vm;
    // TODO: Free whatever needs to be freed
}

// Execution

void execute_byte(VM *vm, OpCode op) {
    switch (op) {
        case NOP: {
            break;
        }
        case STR: {
            VERBOSE_LOG("[%zx] Saving a string\n", vm->pc);

            u64 str_length = get_next_u64_from_program(vm);
            char *str = (char*) &vm->program->code[vm->pc];

            // Increment the program counter by the length of the string
            vm->pc += str_length;

            push_u64_to_stack(&vm->stack, (u64) str);
            push_u64_to_stack(&vm->stack, str_length);
            break;
        }
        case PTS: {
            VERBOSE_LOG("[%zx] Printing string\n", vm->pc);

            u64 str_length = pop_u64_from_stack(&vm->stack);
            const char *str = (const char*)pop_u64_from_stack(&vm->stack);
            
            // Since the string is not null terminated, we need to print it manually
            for (usize i = 0; i < str_length; i++) {
                putc(str[i], stdout);
            }
            break;
        }
        case PTC: {
            VERBOSE_LOG("[%zx] Printing char\n", vm->pc);

            u8 c = pop_from_stack(&vm->stack);
            putc((int)c, stdout);
            break;
        }
        case ADD: {
            VERBOSE_LOG("[%zx] Adding two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);
            push_u64_to_stack(&vm->stack, b + a);
            break;
        }
        case MOD: {
            VERBOSE_LOG("[%zx] Modulo two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);
            push_u64_to_stack(&vm->stack, b % a);
            break;
        }
        case MUL: {
            VERBOSE_LOG("[%zx] Dividing two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);

            push_u64_to_stack(&vm->stack, b * a);
            break;
        }
        case DIV: {
            VERBOSE_LOG("[%zx] Dividing two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);

            push_u64_to_stack(&vm->stack, b / a);
            break;
        }
        case CLL: {
            // This will jump to the latest label, pushing the current position to the stack
            u64 pos = vm->pc;
            u64 target = pop_u64_from_stack(&vm->stack);

            push_u64_to_stack(&vm->stack, pos);
            LOG("Calling to address 0x%llx\n", target);
            // printf("Pushed return address: 0x%llx\n", pos);
            vm->pc = (usize) target;
            VERBOSE_LOG("The new pc is 0x%zx\n", vm->pc);
            break;
        }
        case JMP: {
            VERBOSE_LOG("[%zx] Jumping\n", vm->pc);

            u64 target = pop_u64_from_stack(&vm->stack);
            vm->pc = (usize) target;
            break;
        }
        case JPT: {
            VERBOSE_LOG("[%zx] Jumping if true\n", vm->pc);

            u64 target = pop_u64_from_stack(&vm->stack);

            u8 condition = pop_from_stack(&vm->stack);
            if (condition) {
                VERBOSE_LOG("[%zx] Was true, jumping\n", vm->pc);
                vm->pc = (usize) target;
            } else {
                VERBOSE_LOG("[%zx] Was false, not jumpint\n", vm->pc);
            }
            break;
        }
        case JPF: {
            VERBOSE_LOG("[%zx] Jumping if false\n", vm->pc);

            u64 target = pop_u64_from_stack(&vm->stack);

            u8 condition = pop_from_stack(&vm->stack);
            if (!condition) {
                VERBOSE_LOG("[%zx] Was false, jumping\n", vm->pc);
                vm->pc = (usize) target;
            } else {
                VERBOSE_LOG("[%zx] Was true, not jumping\n", vm->pc);
            }
            break;
        }
        case EQU: {
            VERBOSE_LOG("[%zx] Checking if equal\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);

            push_to_stack(&vm->stack, (u8) a == b);
            break;
        }
        case LT: {
            VERBOSE_LOG("[%zx] Checking if less than\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);

            bool result = b < a;

            push_to_stack(&vm->stack, result);
            break;
        }
        case GT: {
            VERBOSE_LOG("[%zx] Checking if greater than\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);

            bool result = b > a;

            push_to_stack(&vm->stack, result);
            break;
        }
        case NOT: {
            VERBOSE_LOG("[%zx] Negating a value\n", vm->pc);

            bool a = pop_from_stack(&vm->stack);

            bool result = !a;

            push_to_stack(&vm->stack, result);
            break;
        }
        case OR: {
            VERBOSE_LOG("[%zx] Negating a value\n", vm->pc);

            bool a = pop_from_stack(&vm->stack);
            bool b = pop_from_stack(&vm->stack);

            bool result = a || b;

            push_to_stack(&vm->stack, result);
            break;
        }
        case REF: {
            VERBOSE_LOG("[%zx] Dereferencing a pointer\n", vm->pc);
            u64 ptr_num = pop_u64_from_stack(&vm->stack);
            u64 value = *((u64*)ptr_num);

            push_u64_to_stack(&vm->stack, value);
            break;
        }
        case RF8: {
            VERBOSE_LOG("[%zx] Dereferencing a u8 pointer\n", vm->pc);
            u64 ptr_num = pop_u64_from_stack(&vm->stack);
            u8 value = *((u64*)ptr_num);

            push_to_stack(&vm->stack, value);
            break;
        }
        case DBG: {
            u64 num = pop_u64_from_stack(&vm->stack);
            printf("%llu", num);
            break;
        }
        case EXT: {
            VERBOSE_LOG("[%zx] Exiting\n", vm->pc);

            vm->pc = vm->program->size;
            break;
        }
        case INC: {
            VERBOSE_LOG("[%zx] Incrementing the top stack value\n", vm->pc);

            u64 value = pop_u64_from_stack(&vm->stack);
            push_u64_to_stack(&vm->stack, value + 1);
            break;
        }
        case DEC: {
            VERBOSE_LOG("[%zx] Decrementing the top stack value\n", vm->pc);

            u64 value = pop_u64_from_stack(&vm->stack);
            push_u64_to_stack(&vm->stack, value - 1);
            break;
        }
        case PSH: {
            VERBOSE_LOG("[%zx] Pushing to the stack\n", vm->pc);

            u64 value = get_next_u64_from_program(vm);

            push_u64_to_stack(&vm->stack, value);
            break;
        }
        case PS8: {
            VERBOSE_LOG("[%zx] Pushing a byte to the stack\n", vm->pc);

            u8 value = get_next_u8_from_program(vm);

            push_to_stack(&vm->stack, value);
            break;
        }
        case DUP: {
            VERBOSE_LOG("[%zx] Duplicating the top stack value\n", vm->pc);

            u64 value = pop_u64_from_stack(&vm->stack);
            push_u64_to_stack(&vm->stack, value);
            push_u64_to_stack(&vm->stack, value);
            break;
        }
        case SWP: {
            VERBOSE_LOG("[%zx] Swapping the top two stack values\n", vm->pc);

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);

            push_u64_to_stack(&vm->stack, a);
            push_u64_to_stack(&vm->stack, b);
            break;
        }
        case DRP: {
            (void) pop_u64_from_stack(&vm->stack);
            break;
        }
        case ROT: {
            VERBOSE_LOG("[%zx] Rotating top 3 values in the stack\n", vm->pc);
            ASSERT(vm->stack.sp >= 3, "Stack has enough values\n");

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);
            u64 c = pop_u64_from_stack(&vm->stack);

            push_u64_to_stack(&vm->stack, b);
            push_u64_to_stack(&vm->stack, a);
            push_u64_to_stack(&vm->stack, c);
            break;
        }
        case OVR: {
            VERBOSE_LOG("[%zx] Duplicating the value below the top of the stack\n", vm->pc);
            ASSERT(vm->stack.sp >= 2, "Stack has enough values\n");

            u64 a = pop_u64_from_stack(&vm->stack);
            u64 b = pop_u64_from_stack(&vm->stack);

            push_u64_to_stack(&vm->stack, b);
            push_u64_to_stack(&vm->stack, a);
            push_u64_to_stack(&vm->stack, b);
            break;
        }
        case BKP: {
            VERBOSE_LOG("[%zx] Hit breakpoint\n", vm->pc);

            printf("The stack at this point:\n");
            debug_stack(&vm->stack);
            // vm->pc = vm->program->size;
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
        OpCode op = get_next_u8_from_program(&vm);

        execute_byte(&vm, op);
    }

    destroy_vm(&vm);
}

void debug_execute(Program *program) {
    VM vm = {0};
    vm.program = program;

    while (vm.pc < program->size) {
        usize inst_position = vm.pc;
        OpCode op = get_next_u8_from_program(&vm);

        execute_byte(&vm, op);

        if (op == BKP) {
            printf("Stopping at 0x%03zx\n", inst_position);
            printf("Press enter to continue or q to exit:");
            int input = getchar();

            while (input != '\n') {
                if (input != EOF) {
                    break;
                }

                input = getchar();
            }

            if (input == 'q') {
                break;
            }
        }
    }

    destroy_vm(&vm);
}
