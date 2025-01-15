#ifndef OPCODES_H
#define OPCODES_H
#include "core.h"

// A macro to define the list of opcodes (with a defined BASE_T value) and their names
// https://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c
#define OPCODES \
    X(NOP, 0x00) \
/* u64 opcodes */\
    X(ADD, 0x01) \
    X(SUB, 0x02) \
    X(MOD, 0x03) \
    X(DIV, 0x04) \
    X(MUL, 0x05) \
    X(EQU, 0x06) \
    X(LT , 0x07) \
    X(DBG, 0x08) \
    X(INC, 0x09) \
    X(DEC, 0x0A) \
    X(PSH, 0x0B) \
    X(DUP, 0x0C) \
    X(SWP, 0x0D) \
    X(DRP, 0x0E) \
    X(OVR, 0x0F) \
    X(GT , 0x10) \
    X(REF, 0x11) \
    X(ROT, 0x12) \
    X(ALC, 0x13) \
    X(WRT, 0x14) \
    X(FRE, 0x15) \
/* u8 opcodes */ \
    X(RF8, 0x30) \
    X(PS8, 0x31) \
    X(WR8, 0x32) \
/* func related opcodes */ \
    X(RET, 0x40) \
    X(TKS, 0x41) \
    X(CLL, 0x42) \
/* typed opcodes */ \
    X(ADDZ, 0x81) \
    X(SUBZ, 0x82) \
    X(MODZ, 0x83) \
    X(DIVZ, 0x84) \
    X(MULZ, 0x85) \
    X(EQUZ, 0x86) \
    X(LTZ , 0x87) \
    X(DBGZ, 0x88) \
    X(INCZ, 0x89) \
    X(DECZ, 0x8A) \
    X(PSHZ, 0x8B) \
    X(DUPZ, 0x8C) \
    X(SWPZ, 0x8D) \
    X(DRPZ, 0x8E) \
    X(OVRZ, 0x8F) \
    X(GTZ , 0x90) \
    X(REFZ, 0x91) \
    X(WRTZ, 0x92) \
/* logical opcodes*/ \
    X(NOT, 0xA0) \
    X(OR , 0xA1) \
/* misc opcodes */ \
    X(JMP, 0xB0) \
    X(JPT, 0xB1) \
    X(JPF, 0xB2) \
    X(PTC, 0xB3) \
    X(PTS, 0xB4) \
    X(STR, 0xB5) \
    X(BKP, 0xFE) \
    X(EXT, 0xFF)

#define X(name, val) name = val,
enum _opcode {
    OPCODES
};
typedef enum _opcode OpCode;
#undef X

const int OPCODE_COUNT;

// PNT -> Considers the top of the stack a number with the length and a pointer to string and prints it
// By default, all stack operations will work on 64-bit values

char *opcode_to_str(OpCode op);

void print_opcode(OpCode op);

bool string_to_opcode(OpCode *dst, char *str);

#endif // OPCODES_H
