#ifndef PROGRAM_BUILDER_H
#define PROGRAM_BUILDER_H
#include "core.h"
#include "opcodes.h"
#include "program.h"

// flexible sized array of instructions (u8):
// https://stackoverflow.com/questions/3536153/c-dynamically-growing-array
typedef struct {
    u8 *data;
    usize count;
    usize capacity;
} InstructionArray;

void init_array(InstructionArray*, usize initial_capacity);

void grow_array_to_fit(InstructionArray*, usize new_elements);
void insert_array(InstructionArray*, u8 element);

void free_array(InstructionArray*);

// A utility struct for building a bytecode Program.
typedef struct {
    InstructionArray instructions;
    usize labels[256];
    u8 current_label;
} ProgramBuilder;

void init_program_builder(ProgramBuilder *builder);
void free_program_builder(ProgramBuilder *builder);

//    To Program:
void clone_to_program(ProgramBuilder*, Program*);

// Instruction creation
void emit_u8(ProgramBuilder*, u8);
void emit_instruction(ProgramBuilder*, OpCode);
void emit_nop(ProgramBuilder*);
void emit_push(ProgramBuilder*, u8 value);
void emit_str(ProgramBuilder*, char*);
void emit_jump(ProgramBuilder*, u8);
void emit_jump_if_true(ProgramBuilder*, u8 label);

// Labels
u8 create_label(ProgramBuilder*);
void link_label(ProgramBuilder*, u8);

#endif // PROGRAM_BUILDER_H