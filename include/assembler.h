#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include "core.h"
#include "program.h"

typedef struct {
    char *code;
    usize count;
    usize current_pos;

    HashMap labels;
} Assembler;

void init_assembler(Assembler *assembler);
void free_assembler(Assembler *assembler);
void assemble_ignore_spaces(Assembler *assembler);

HashEntry *assemble_label_literal(Assembler *assembler, bool *exists);
u64 assemble_u64_literal(Assembler *assembler);
StringBuffer assemble_string_literal(Assembler *assembler);

Program assemble(Assembler *assembler);

int assemble_file(char *input_file);

#endif //ndef ASSEMBLER_H
