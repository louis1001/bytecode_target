#ifndef PROGRAM_BUILDER_H
#define PROGRAM_BUILDER_H
#include "core.h"
#include "opcodes.h"
#include "program.h"

// A union containing operands, which can be u8, u32 or u64.
typedef union {
    u8 u8;
    u32 u32;
    u64 u64;
} OperandData;

// An enum containing the types of operands.
typedef enum {
    OPERAND_U8 = sizeof(u8),
    OPERAND_U32 = sizeof(u32),
    OPERAND_U64 = sizeof(u64),
} OperandType; // aka the size of the operand

// A struct containing an operand and its type.
typedef struct {
    OperandType type;
    OperandData as;
} Operand;

typedef struct {
    Operand *data;
    usize count;
    usize capacity;
} OperandArray;

void init_operand_array(OperandArray*, usize initial_capacity);
void grow_operand_array_to_fit(OperandArray*, usize new_elements);
void insert_operand_array(OperandArray*, Operand operand);
void free_operand_array(OperandArray*);

typedef struct {
    OpCode opcode;
    OperandArray operands;
    bool operand_is_label;
} Instruction;

// flexible sized array of instructions (BASE_T):
typedef struct {
    Instruction *data;
    usize count;
    usize capacity;
} InstructionArray;

void init_inst_array(InstructionArray*, usize initial_capacity);
void grow_inst_array_to_fit(InstructionArray*, usize new_elements);
Instruction *insert_inst_array(InstructionArray*, Instruction element);
void free_inst_array(InstructionArray*);

// A utility struct for building a bytecode Program.
#define LABEL_T u8
typedef struct {
    InstructionArray instructions;
    usize labels[256];
    LABEL_T current_label;
} ProgramBuilder;

void init_program_builder(ProgramBuilder *builder);
void free_program_builder(ProgramBuilder *builder);

void debug_print_program_builder(ProgramBuilder *builder);

//    To Program:
void clone_to_program(ProgramBuilder*, Program*);

// Instruction creation
// emit_instructions needs to be called with a variable number of arguments
Instruction *emit_plain_instruction(ProgramBuilder*, OpCode);
Instruction *emit_instruction(ProgramBuilder*, OpCode, usize count, Operand first, ...);
Instruction *emit_instruction_with_operands(ProgramBuilder*, OpCode, Operand *, usize operands_count);
void emit_nop(ProgramBuilder*);
void emit_push(ProgramBuilder*, BASE_T value);
void emit_push_label(ProgramBuilder*, BASE_T label);
void emit_str(ProgramBuilder*, char*);
void emit_sized_instruction(ProgramBuilder*, OpCode, u64);
void emit_jump(ProgramBuilder*, LABEL_T);
void emit_jump_if_true(ProgramBuilder*, LABEL_T label);
void emit_jump_if_false(ProgramBuilder*, LABEL_T label);

// Labels
LABEL_T create_label(ProgramBuilder*);
void link_label(ProgramBuilder*, LABEL_T);

#endif // PROGRAM_BUILDER_H
