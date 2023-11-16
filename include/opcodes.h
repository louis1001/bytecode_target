#ifndef OPCODES_H
#define OPCODES_H
#include "core.h"

// A macro to define the list of opcodes (with a defined BASE_T value) and their names
// https://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c
#define OPCODES \
    X(NOP, 0x00) \
    X(LDR, 0x02) \
    X(ADD, 0x03) \
    X(SUB, 0x04) \
    X(MOD, 0x05) \
    X(DIV, 0x06) \
    X(MUL, 0x07) \
    X(EQU, 0x08) \
    X(LT , 0x09) \
    X(GT , 0x1B) \
    X(JMP, 0x0A) \
    X(JPT, 0x0B) \
    X(JPF, 0x0C) \
    X(CLL, 0x0d) \
    X(STR, 0x0E) \
    X(PNT, 0x0F) \
    X(DBG, 0x10) \
    X(PCH, 0x20) \
    X(INC, 0x11) \
    X(DEC, 0x12) \
    X(PSH, 0x13) \
    X(DUP, 0x14) \
    X(SWP, 0x15) \
    X(DRP, 0x16) \
    X(ROT, 0x17) \
    X(OVR, 0x18) \
    X(BKP, 0xFE) \
    X(EXT, 0xFF)

#define X(name, val) name = val,
typedef enum {
    OPCODES
} OpCode;
#undef X

// PNT -> Considers the top of the stack a number with the length and a pointer to string and prints it
// By default, all stack operations will work on 64-bit values

char *opcode_to_str(OpCode op);

void print_opcode(OpCode op);

bool string_to_opcode(OpCode *dst, char *str);

#endif // OPCODES_H
