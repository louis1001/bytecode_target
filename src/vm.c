#include "vm.h"
#include "opcodes.h"
#include <string.h>

void push_to_call_stack(CallStack *st, StackFrame frame) {
    ASSERT(st->sp < MAX_CALLSTACK_SIZE, "Max call stack size exceeded.\n");

    st->storage[st->sp++] = frame;
}

StackFrame pop_from_call_stack(CallStack *st) {
    ASSERT(st->sp > 0, "Not enough elements on the call stack.\n");

    return st->storage[--st->sp];
}

StackFrame *current_stack_frame(CallStack *st) {
    ASSERT(st->sp > 0, "Not enough elements on the call stack.\n");

    return &st->storage[st->sp-1];
}

void push_to_stack(Stack* st, u8 value) {
    ASSERT(st->sp < MAX_STACK_SIZE, "Max stack size exceeded.\n");

    st->storage[st->sp++] = value;
}

void push_n_to_stack(Stack* st, usize n, u8* values) {
    ASSERT((st->sp + n) <= MAX_STACK_SIZE, "Max stack size exceeded.\n");

    u8 *dest = &st->storage[st->sp];
    memcpy(dest, values, n);

    ASSERT(dest != NULL, "Failed to push new values");

    st->sp += n;
}

void push_u64_to_stack(Stack* st, u64 value) {
    ASSERT(st->sp + sizeof(u64) < MAX_STACK_SIZE, "Max stack size exceeded.\n");

    memcpy(&st->storage[st->sp], &value, sizeof(u64));
    st->sp += sizeof(u64);
}

u8 pop_from_stack(VM *vm) {
    Stack *stack = &vm->stack;
    StackFrame *current_frame = current_stack_frame(&vm->call_stack);
    ASSERT(stack->sp - 1 >= current_frame->stack_start, "Invalid access out of stack frame bounds.\n");

    return stack->storage[--stack->sp];
}

void pop_n_from_stack(VM *vm, usize n, u8* out) {
    Stack *stack = &vm->stack;
    StackFrame *current_frame = current_stack_frame(&vm->call_stack);
    ASSERT(
        stack->sp - n >= current_frame->stack_start,
        "Invalid access out of stack frame bounds.\n"
        "Trying to pop %zu elements from stack (%zu), but the bound is at %zu",
        n,
        stack->sp,
        current_frame->stack_start
    );

    memcpy(out, &stack->storage[stack->sp - n], n);
    stack->sp -= n;
}

u64 pop_u64_from_stack(VM *vm) {
    Stack *stack = &vm->stack;
    StackFrame *current_frame = current_stack_frame(&vm->call_stack);
    ASSERT(
        stack->sp - sizeof(u64) >= current_frame->stack_start,
        "Invalid access out of stack frame bounds.\n"
        "Trying to pop 8 elements from stack (%zu), but the bound is at %zu",
        stack->sp,
        current_frame->stack_start
    );

    u64 value;
    memcpy(&value, &stack->storage[stack->sp - sizeof(u64)], sizeof(u64));
    stack->sp -= sizeof(u64);
    return value;
}

u8* peek_n_from_stack_with_offset(VM *vm, usize offset, usize n) {
    // TODO: For now, it just gives back a pointer to the start of sp - offset.
    // Doesn't do anything with n
    Stack *stack = &vm->stack;
    StackFrame *current_frame = current_stack_frame(&vm->call_stack);
    ASSERT(offset >= n, "Invalid access to stack: offset is less than the fetch size\n");
    ASSERT(stack->sp - offset >= current_frame->stack_start, "Invalid access out of stack frame bounds.\n");

    return &stack->storage[stack->sp - offset];
}

u8* peek_n_from_stack(VM *vm, usize n) {
    return peek_n_from_stack_with_offset(vm, n, n);
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

            u64 str_length = pop_u64_from_stack(vm);
            const char *str = (const char*)pop_u64_from_stack(vm);
            
            // Since the string is not null terminated, we need to print it manually
            for (usize i = 0; i < str_length; i++) {
                putc(str[i], stdout);
            }
            break;
        }
        case PTC: {
            VERBOSE_LOG("[%zx] Printing char\n", vm->pc);

            u8 c = pop_from_stack(vm);
            putc((int)c, stdout);
            break;
        }
        case ADD: {
            VERBOSE_LOG("[%zx] Adding two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);
            push_u64_to_stack(&vm->stack, b + a);
            break;
        }
        case SUB: {
            VERBOSE_LOG("[%zx] Adding two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);
            push_u64_to_stack(&vm->stack, b - a);
            break;
        }
        case MOD: {
            VERBOSE_LOG("[%zx] Modulo two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);
            push_u64_to_stack(&vm->stack, b % a);
            break;
        }
        case MUL: {
            VERBOSE_LOG("[%zx] Dividing two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);

            push_u64_to_stack(&vm->stack, b * a);
            break;
        }
        case DIV: {
            VERBOSE_LOG("[%zx] Dividing two ints\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);

            push_u64_to_stack(&vm->stack, b / a);
            break;
        }
        case CLL: {
            VERBOSE_LOG("[%zx] Calling to function pointer\n", vm->pc);
            // This will jump to the latest label, pushing the current position to the stack
            u64 pos = vm->pc;
            u64 target = pop_u64_from_stack(vm);

            StackFrame sf = {
                .callee = target,
                .caller_site = pos,
                .stack_start = vm->stack.sp
            };

            push_to_call_stack(&vm->call_stack, sf);
            LOG("Calling to address 0x%llx\n", target);
            // printf("Pushed return address: 0x%llx\n", pos);
            vm->pc = (usize) target;
            VERBOSE_LOG("The new pc is 0x%zx\n", vm->pc);
            break;
        }
        case RET: {
            VERBOSE_LOG("[%zx] Returning\n", vm->pc);

            StackFrame current_frame = pop_from_call_stack(&vm->call_stack);

            // vm->stack.sp = current_frame.stack_start;
            vm->pc = current_frame.caller_site;

            VERBOSE_LOG("Returning to %#llx\n", current_frame.caller_site);
            break;
        }
        case TKS: {
            VERBOSE_LOG("[%zx] Setting the args size\n", vm->pc);

            u64 args_size = pop_u64_from_stack(vm);
            CallStack *call_stack = &vm->call_stack;
            StackFrame *current_frame = current_stack_frame(&vm->call_stack);
            ASSERT(vm->stack.sp >= args_size, "Not enough elements on the stack for function args.\n");
            ASSERT(vm->stack.sp == current_frame->stack_start, "Opcode TKS must be used when the stack hasn't moved since calling the function.\n");

            for (usize i = call_stack->sp-1; i > 0; i++) {
                StackFrame *frame = &call_stack->storage[i];
                if (vm->stack.sp - args_size < frame->stack_start) {
                    frame->stack_start = vm->stack.sp - args_size;
                } else {
                    break;
                }
            }
            
            break;
        }
        case JMP: {
            VERBOSE_LOG("[%zx] Jumping\n", vm->pc);

            u64 target = pop_u64_from_stack(vm);
            vm->pc = (usize) target;
            break;
        }
        case JPT: {
            VERBOSE_LOG("[%zx] Jumping if true\n", vm->pc);

            u64 target = pop_u64_from_stack(vm);

            u8 condition = pop_from_stack(vm);
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

            u64 target = pop_u64_from_stack(vm);

            u8 condition = pop_from_stack(vm);
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

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);

            push_to_stack(&vm->stack, (u8) a == b);
            break;
        }
        case LT: {
            VERBOSE_LOG("[%zx] Checking if less than\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);

            bool result = b < a;

            push_to_stack(&vm->stack, result);
            break;
        }
        case GT: {
            VERBOSE_LOG("[%zx] Checking if greater than\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);

            bool result = b > a;

            push_to_stack(&vm->stack, result);
            break;
        }
        case NOT: {
            VERBOSE_LOG("[%zx] Negating a value\n", vm->pc);

            bool a = pop_from_stack(vm);

            bool result = !a;

            push_to_stack(&vm->stack, result);
            break;
        }
        case OR: {
            VERBOSE_LOG("[%zx] Negating a value\n", vm->pc);

            bool a = pop_from_stack(vm);
            bool b = pop_from_stack(vm);

            bool result = a || b;

            push_to_stack(&vm->stack, result);
            break;
        }
        case REF: {
            VERBOSE_LOG("[%zx] Dereferencing a pointer\n", vm->pc);
            u64 ptr_num = pop_u64_from_stack(vm);
            u64 value = *((u64*)ptr_num);

            push_u64_to_stack(&vm->stack, value);
            break;
        }
        case RF8: {
            VERBOSE_LOG("[%zx] Dereferencing a u8 pointer\n", vm->pc);
            u64 ptr_num = pop_u64_from_stack(vm);
            u8 value = *((u64*)ptr_num);

            push_to_stack(&vm->stack, value);
            break;
        }
        case DBG: {
            u64 num = pop_u64_from_stack(vm);
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

            u64 value = pop_u64_from_stack(vm);
            push_u64_to_stack(&vm->stack, value + 1);
            break;
        }
        case DEC: {
            VERBOSE_LOG("[%zx] Decrementing the top stack value\n", vm->pc);

            u64 value = pop_u64_from_stack(vm);
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

            u64 value = pop_u64_from_stack(vm);
            push_u64_to_stack(&vm->stack, value);
            push_u64_to_stack(&vm->stack, value);
            break;
        }
        case SWP: {
            VERBOSE_LOG("[%zx] Swapping the top two stack values\n", vm->pc);

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);

            push_u64_to_stack(&vm->stack, a);
            push_u64_to_stack(&vm->stack, b);
            break;
        }
        case DRP: {
            (void) pop_u64_from_stack(vm);
            break;
        }
        case ROT: {
            VERBOSE_LOG("[%zx] Rotating top 3 values in the stack\n", vm->pc);
            ASSERT(vm->stack.sp >= 3, "Stack has enough values\n");

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);
            u64 c = pop_u64_from_stack(vm);

            push_u64_to_stack(&vm->stack, b);
            push_u64_to_stack(&vm->stack, a);
            push_u64_to_stack(&vm->stack, c);
            break;
        }
        case OVR: {
            VERBOSE_LOG("[%zx] Duplicating the value below the top of the stack\n", vm->pc);
            ASSERT(vm->stack.sp >= 2, "Stack has enough values\n");

            u64 a = pop_u64_from_stack(vm);
            u64 b = pop_u64_from_stack(vm);

            push_u64_to_stack(&vm->stack, b);
            push_u64_to_stack(&vm->stack, a);
            push_u64_to_stack(&vm->stack, b);
            break;
        }
        // FIXME: Is this fine? Does it defeat the purpose of a stack machine?
        case DRPZ: {
            VERBOSE_LOG("[%zx] Droping n bytes from the stack\n", vm->pc);
            u64 n = get_next_u64_from_program(vm);

            u8 result[n];
            pop_n_from_stack(vm, n, &result[0]);
            break;
        }
        case DUPZ: {
            VERBOSE_LOG("[%zx] Duping n bytes on the stack\n", vm->pc);
            u64 off = get_next_u64_from_program(vm);
            u64 n = get_next_u64_from_program(vm);

            u8 *result = peek_n_from_stack_with_offset(vm, off, n);

            push_n_to_stack(&vm->stack, n, result);
            break;
        }
        case BKP: {
            VERBOSE_LOG("[%zx] Hit breakpoint\n", vm->pc);

            printf("The stack at this point:\n");
            debug_stack(&vm->stack);

            printf("The call stack at this point:\n");
            for (usize i = 0; i < vm->call_stack.sp; i++) {
                StackFrame *frame = &vm->call_stack.storage[i];
                printf("Frame %#llx (called from %#llx)\n", frame->callee, frame->caller_site);
            }
            printf("-------\n");

            // vm->pc = vm->program->size;
            break;
        }
        case SWPZ: {
            VERBOSE_LOG("[%zx] Swapping (with sizing) the top two stack values\n", vm->pc);
            u64 n = get_next_u64_from_program(vm);

            u8 a[n];
            pop_n_from_stack(vm, n, a);

            u8 b[n];
            pop_n_from_stack(vm, n, b);

            push_n_to_stack(&vm->stack, n, a);
            push_n_to_stack(&vm->stack, n, b);
            break;
        }
        default: {
            char *opcode_name = opcode_to_str(op);
            if (opcode_name != NULL) {
                ERROR("Opcode %s not implemented\n", opcode_name);
            } else {
                ERROR("Invalid opcode %#x\n", op);
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
    StackFrame global_stack_frame = {
        .caller_site = 0,
        .callee = 0,
        .stack_start = 0
    };
    push_to_call_stack(&vm.call_stack, global_stack_frame);
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
