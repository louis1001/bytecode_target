#include "program_builder.h"
#include <string.h>
#if DEBUG
#include <stdio.h>
#endif // DEBUG

// const char* get_opcode_name(OpCode opcode) {
//     return opcode_names[opcode];
// }

// Array:
void init_array(InstructionArray *a, usize initial_capacity) {
    a->data = (u8*)malloc(initial_capacity * sizeof(u8));
    if (a->data == NULL) {
        ERROR("Could not allocate the memory for instruction array");
    }
    a->count = 0;
    a->capacity = initial_capacity;
}

void grow_array_to_fit(InstructionArray* array, usize new_elements) {
    // Check if the array should grow to accomodate new_elements, or if the current capacity minus current count is enough.
    // If it is, do nothing.
    if (array->capacity - array->count >= new_elements) { return; }

    // Otherwise, grow the array in multiples of 2 until it can fit new_elements.
    usize target_capacity = array->capacity + new_elements;
    while (array->capacity < target_capacity) {
        array->capacity *= 2;
    }
    #if DEBUG
    printf("INFO: Growing the array to capacity %zu to fit %zu new elements\n", array->capacity, new_elements);
    #endif // DEBUG
    array->data = (u8*)realloc(array->data, array->capacity * sizeof(u8));
    if (array->data == NULL) {
        ERROR("Could not reallocate the memory for growing instruction array");
    }
}

void insert_array(InstructionArray *a, u8 element) {
  // a->size is the number of used entries, because a->array[a->size++] updates a->size only *after* the array has been accessed.
  // Therefore a->size can go up to a->capacity 
  if (a->count == a->capacity) {
    #if DEBUG
    printf("INFO: Growing the array to %zu\n", a->capacity * 2);
    #endif // DEBUG

    a->capacity *= 2;
    a->data = (u8*)realloc(a->data, a->capacity * sizeof(u8));
    if (a->data == NULL) {
        ERROR("Could not reallocate the memory for growing instruction array");
    }
  }
  a->data[a->count++] = element;
}

void free_array(InstructionArray *a) {
  free(a->data);
  a->data = NULL;
  a->count = a->capacity = 0;
}

void init_program_builder(ProgramBuilder *builder) {
    init_array(&builder->instructions, 4);
}

void free_program_builder(ProgramBuilder *builder) {
    free_array(&builder->instructions);
}

void clone_to_program(ProgramBuilder *builder, Program *program) {
    // Create a temporary array where labels are resolved
    // Labels exist like this in the code: JMP LBL 0x00 where 0x00 is the label index
    // We need to replace the label index with the actual address of the label and remove LBL, which means
    // the size of instructions will change after this.
    InstructionArray temp = {0};
    init_array(&temp, builder->instructions.count);

    // Keep track of how many bytes from the builder array are ignored
    // Because the labels are replaced with their addresses, the size of the array will change
    // and we need to know how to offset the label target
    usize ignored_bytes = 0;

    for (usize i = 0; i < builder->instructions.count; i++) {
        u8 instruction = builder->instructions.data[i];
        if (instruction == LBL) {
            // Replace the label with the address of the label
            u8 label_index = builder->instructions.data[i + 1];
            u8 label_address = builder->labels[label_index];
            ignored_bytes += 2;
            label_address -= ignored_bytes; // Offset the label address by the number of ignored bytes
            #if VERBOSE
            printf("INFO: Replacing label index %d with address 0x%x\n", label_index, label_address);
            #endif // DEBUG
            insert_array(&temp, label_address);
            i++; // Skip the label index
        } else {
            insert_array(&temp, instruction);
        }
    }

    program->size = temp.count;
    program->code = (u8*)malloc(program->size * sizeof(u8));

    memcpy(program->code, temp.data, program->size);
    if (program->code == NULL) {
        ERROR("Could not reallocate the memory for growing instruction array");
    }

    free_array(&temp);

    #if DEBUG
    printf("INFO: Cloned program has %zu bytes\n", program->size);
    #endif // DEBUG
}

void emit_u8(ProgramBuilder *builder, u8 value) {
    insert_array(&builder->instructions, value);
}

void emit_instruction(ProgramBuilder *builder, OpCode opcode) {
    emit_u8(builder, opcode);
}

void emit_nop(ProgramBuilder *builder) {
    emit_instruction(builder, NOP);
}

void emit_push(ProgramBuilder *builder, u8 value) {
    emit_instruction(builder, PSH);
    emit_u8(builder, value);
}

void emit_str(ProgramBuilder *builder, char *str) {
    emit_instruction(builder, STR);
    usize len = strlen(str);

    grow_array_to_fit(&builder->instructions, len + 1); // +1 to fit \0
    char* stack_begin = (char*) &builder->instructions.data[builder->instructions.count];
    strcpy(stack_begin, str);
    builder->instructions.count += len + 1;
}

void emit_jump(ProgramBuilder* builder, u8 target) {
    emit_instruction(builder, PSH);
    emit_instruction(builder, LBL);
    emit_u8(builder, target);
    emit_instruction(builder, JMP);
}

void emit_jump_if_true(ProgramBuilder* builder, u8 target) {
    emit_instruction(builder, PSH);
    emit_instruction(builder, LBL);
    emit_u8(builder, target);
    emit_instruction(builder, JPT);
}

// Labels
u8 create_label(ProgramBuilder* builder) {
    return builder->current_label++;
}

void link_label(ProgramBuilder* builder, u8 addr) {
    builder->labels[addr] = builder->instructions.count;
}