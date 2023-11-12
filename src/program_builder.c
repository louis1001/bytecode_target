#include "program_builder.h"
#include <string.h>
#include <stdarg.h>
#if DEBUG
#include <stdio.h>
#endif // DEBUG

// const char* get_opcode_name(OpCode opcode) {
//     return opcode_names[opcode];
// }

BASE_T min(BASE_T a, BASE_T b) {
    return a < b ? a : b;
}

BASE_T max(BASE_T a, BASE_T b) {
    return a < b ? a : b;
}

// Operand Array:
void init_operand_array(OperandArray *a, usize initial_capacity) {
    usize actual_capacity = initial_capacity == 0 ? 1 : initial_capacity;
    a->data = malloc(actual_capacity * sizeof(BASE_T));
    if (a->data == NULL) {
        ERROR("Could not allocate the memory for instruction array");
    }
    a->count = 0;
    a->capacity = actual_capacity;
}

void grow_operand_array_to_fit(OperandArray* array, usize new_elements) {
    // Check if the array should grow to accomodate new_elements, or if the current capacity minus current count is enough.
    // If it is, do nothing.
    if (array->capacity - array->count >= new_elements) { return; }

    // Otherwise, grow the array in multiples of 2 until it can fit new_elements.
    usize target_capacity = array->capacity + new_elements;
    while (array->capacity < target_capacity) {
        array->capacity *= 2;
    }

    LOG("Growing the array to capacity %zu to fit %zu new elements\n", array->capacity, new_elements);

    array->data = realloc(array->data, array->capacity * sizeof(BASE_T));
    if (array->data == NULL) {
        ERROR("Could not reallocate the memory for growing instruction array");
    }
}

void insert_operand_array(OperandArray *a, BASE_T element) {
  // a->size is the number of used entries, because a->array[a->size++] updates a->size only *after* the array has been accessed.
  // Therefore a->size can go up to a->capacity 
  if (a->count == a->capacity) {
    LOG("INFO: Growing the array to %zu\n", a->capacity * 2);

    a->capacity *= 2;
    a->data = realloc(a->data, a->capacity * sizeof(BASE_T));
    if (a->data == NULL) {
        ERROR("Could not reallocate the memory for growing instruction array");
    }
  }
  a->data[a->count++] = element;
}

void free_operand_array(OperandArray *a) {
    free(a->data);
    a->data = NULL;
    a->count = a->capacity = 0;
}

// Instruction Array:
void init_inst_array(InstructionArray *a, usize initial_capacity) {
    usize actual_capacity = initial_capacity == 0 ? 1 : initial_capacity;
    a->data = malloc(actual_capacity * sizeof(Instruction));
    if (a->data == NULL) {
        ERROR("Could not allocate the memory for instruction array");
    }
    a->count = 0;
    a->capacity = actual_capacity;
}

void grow_inst_array_to_fit(InstructionArray* array, usize new_elements) {
    // Check if the array should grow to accomodate new_elements, or if the current capacity minus current count is enough.
    // If it is, do nothing.
    if (array->capacity - array->count >= new_elements) { return; }

    // Otherwise, grow the array in multiples of 2 until it can fit new_elements.
    usize target_capacity = array->capacity + new_elements;
    while (array->capacity < target_capacity) {
        array->capacity *= 2;
    }

    LOG("Growing the array to capacity %zu to fit %zu new elements\n", array->capacity, new_elements);

    array->data = realloc(array->data, array->capacity * sizeof(Instruction));
    if (array->data == NULL) {
        ERROR("Could not reallocate the memory for growing instruction array");
    }
}

Instruction *insert_inst_array(InstructionArray *a, Instruction element) {
  // a->size is the number of used entries, because a->array[a->size++] updates a->size only *after* the array has been accessed.
  // Therefore a->size can go up to a->capacity 
  if (a->count == a->capacity) {
    LOG("Growing the array to %zu\n", a->capacity * 2);

    a->capacity *= 2;
    a->data = realloc(a->data, a->capacity * sizeof(Instruction));
    if (a->data == NULL) {
        ERROR("Could not reallocate the memory for growing instruction array");
    }
  }
  usize index = a->count++;
  a->data[index] = element;

  return a->data + index;
}

void free_inst_array(InstructionArray *a) {
    for (usize i = 0; i < a->count; i++) {
        free_operand_array(&a->data[i].operands);
    }
    free(a->data);
    a->data = NULL;
    a->count = a->capacity = 0;
}

void init_program_builder(ProgramBuilder *builder) {
    init_inst_array(&builder->instructions, 4);
}

void free_program_builder(ProgramBuilder *builder) {
    free_inst_array(&builder->instructions);
}

void clone_to_program(ProgramBuilder *builder, Program *program) {
    // First, figure out the size of the program and the address for each instruction
    usize size = 0;
    usize addresses[builder->instructions.count];
    for (usize i = 0; i < builder->instructions.count; i++) {
        Instruction *inst = &builder->instructions.data[i];
        addresses[i] = size;
        size += 1 + inst->operands.count;
    }

    // Then resolve the labels.
    // Each value in builder->labels is the index of the instruction that the label points to.
    // We need to replace the label with the address of the instruction.
    for (usize i = 0; i < builder->instructions.count; i++) {
        Instruction *inst = &builder->instructions.data[i];
        if (inst->operand_is_label) {
            // labels should have only one operand
            ASSERT(inst->operands.count == 1, "[%zu]0x%x instruction has more than one operand, not %zu\n", i, inst->opcode, inst->operands.count);
            BASE_T index = inst->operands.data[0];
            usize label_addr = builder->labels[index];
            usize target_addr = addresses[label_addr];
            inst->operands.data[0] = (BASE_T) target_addr;

            LOG("Replaced label %llu value from %zu to 0x%zx\n", index, label_addr, target_addr);
        }
    }

    // Now we can allocate the memory for the program
    program->size = size;
    program->code = malloc(size * sizeof(BASE_T));
    if (program->code == NULL) {
        ERROR("Could not allocate the memory for program");
    }

    // And copy the instructions
    for (usize i = 0; i < builder->instructions.count; i++) {
        Instruction *inst = &builder->instructions.data[i];
        program->code[addresses[i]] = inst->opcode;
        memcpy(program->code + addresses[i] + 1, inst->operands.data, inst->operands.count * sizeof(BASE_T));
    }
}

Instruction *emit_plain_instruction(ProgramBuilder *builder, OpCode opcode) {
    Instruction instruction = {0};
    instruction.opcode = opcode;
    init_operand_array(&instruction.operands, 0);

    return insert_inst_array(&builder->instructions, instruction);
}

Instruction *emit_instruction(ProgramBuilder* pb, OpCode opcode, usize count, usize first, ...) {
    // Take the opcode and the operands, and create an instruction.
    // Then insert the instruction into the instruction array.
    Instruction instruction = {0};
    instruction.opcode = opcode;
    init_operand_array(&instruction.operands, count);

    insert_operand_array(&instruction.operands, first);

    // Get the operands from the variadic arguments
    va_list args;
    va_start(args, first);
    for (int i = 0; i < ((int) count)-1; i++) {
        BASE_T operand = (BASE_T)va_arg(args, int);
        insert_operand_array(&instruction.operands, operand);
    }
    va_end(args);

    // Insert the instruction into the instruction array
    return insert_inst_array(&pb->instructions, instruction);
}

Instruction *emit_instruction_with_operands(ProgramBuilder* pb, OpCode opcode, BASE_T *operands, usize operands_count) {
    // Take the opcode and the operands, and create an instruction.
    // Then insert the instruction into the instruction array.
    Instruction instruction = {0};
    instruction.opcode = opcode;
    init_operand_array(&instruction.operands, operands_count);

    // copy directly the operands, since they have to fit given the capacity
    memcpy(instruction.operands.data, operands, operands_count * sizeof(BASE_T));
    instruction.operands.count = operands_count;

    // Insert the instruction into the instruction array
    return insert_inst_array(&pb->instructions, instruction);
}

void emit_nop(ProgramBuilder *builder) {
    emit_plain_instruction(builder, NOP);
}

void emit_push(ProgramBuilder *builder, BASE_T value) {
    emit_instruction(builder, PSH, 1, value);
}

void emit_push_label(ProgramBuilder *builder, BASE_T label) {
    Instruction *inst = emit_instruction(builder, PSH, 0, label);
    inst->operand_is_label = true;
}

void emit_str(ProgramBuilder *builder, char *str) {
    // Pad the end of the string so that it doesn't include garbage memory when copying to the operands array
    usize base_size = sizeof(BASE_T);
    usize len = strlen(str);
    // FIXME: Don't use base_size to store the length when we go to variable length bytecode
    //                 the first base_size is for the length of the string stored in the operands
    usize padded_len = base_size + len + (base_size - (len % base_size));
    char *padded_str = malloc(padded_len);
    if (padded_str == NULL) {
        ERROR("Could not allocate the memory for string");
    }
    memset(padded_str, 0, padded_len);
    memcpy(padded_str + base_size, str, len);

    usize operans_count = (padded_len / base_size) + 1;

    // Now, make the string pointer a pointer of BASE_T with length padded_len / base_size
    BASE_T *str_ptr = (BASE_T *) padded_str;

    *str_ptr = len;

    emit_instruction_with_operands(builder, STR, str_ptr, operans_count);

    free(padded_str);
}

void emit_jump(ProgramBuilder* builder, LABEL_T target) {
    emit_push_label(builder, target); // target is the label index

    emit_plain_instruction(builder, JMP);
}

void emit_jump_if_true(ProgramBuilder* builder, LABEL_T target) {
    emit_push_label(builder, target);
    emit_plain_instruction(builder, JPT);
}

void emit_jump_if_false(ProgramBuilder* builder, LABEL_T target) {
    emit_push_label(builder, target);
    emit_plain_instruction(builder, JPF);
}

// Labels
LABEL_T create_label(ProgramBuilder* builder) {
    return builder->current_label++;
}

void link_label(ProgramBuilder* builder, LABEL_T addr) {
    builder->labels[addr] = builder->instructions.count;
}