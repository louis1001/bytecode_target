#ifndef OPCODES_H
#define OPCODES_H

// A macro to define the list of opcodes (with a defined u8 value) and their names
// https://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c
#define OPCODES \
    X(NOP, 0x00) \
    X(LDR, 0x02) \
    X(ADD, 0x03) \
    X(SUB, 0x04) \
    X(EQU, 0x06) \
    X(LT , 0x09) \
    X(DBG, 0x10) \
    X(JMP, 0x0A) \
    X(STR, 0x0E) \
    X(JPT, 0x0B) \
    X(PNT, 0x0F) \
    X(PCH, 0x10) \
    X(INC, 0x11) \
    X(DEC, 0x12) \
    X(PSH, 0x13) \
    X(LBL, 0xFE) \
    X(EXT, 0xFF)

#define X(name, val) name = val,
typedef enum {
    OPCODES
} OpCode;
#undef X

// PNT -> Considers the top of the stack as an index to a string and prints it
// STR <u8 array of characters> -> Allocates a string on the heap and pushes the index into strings to the stack
// LBL is a Placeholder. To be replaced by a label content
// Besides STR, all opcodes operate on the heap

#endif // OPCODES_H