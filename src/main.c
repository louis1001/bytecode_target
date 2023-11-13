#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "opcodes.h"
#include "program.h"
#include "vm.h"
#include "program_builder.h"

void dump_program_to_file(Program *program, char *file_path) {
    FILE *file = fopen(file_path, "wb");
    fwrite(program->code, sizeof(u8), program->size, file);
    fclose(file);
}

void build_program(ProgramBuilder *builder) {
    // FizzBuzz
    emit_push(builder, 1);

    LABEL_T loop_label = create_label(builder);
        
    LABEL_T newline_label = create_label(builder);
    LABEL_T check_buzz_label = create_label(builder);
    LABEL_T print_fizz_label = create_label(builder);
    LABEL_T print_buzz_label = create_label(builder);
    LABEL_T else_label = create_label(builder);

    link_label(builder, loop_label);

    emit_plain_instruction(builder, DUP);
    emit_push(builder, 3);
    emit_plain_instruction(builder, MOD);
    emit_push(builder, 0);
    emit_plain_instruction(builder, EQU);

    emit_jump_if_true(builder, print_fizz_label);
    emit_push(builder, 0); // did_fizz = false
    emit_plain_instruction(builder, SWP);
    emit_jump(builder, check_buzz_label);

    link_label(builder, print_fizz_label);
    emit_str(builder, "Fizz");
    emit_plain_instruction(builder, PNT);
    emit_push(builder, 1); // did_fizz = true
    emit_plain_instruction(builder, SWP);

    link_label(builder, check_buzz_label);
    emit_plain_instruction(builder, DUP);
    emit_push(builder, 5);
    emit_plain_instruction(builder, MOD);
    emit_push(builder, 0);
    emit_plain_instruction(builder, EQU);

    emit_jump_if_true(builder, print_buzz_label);
    
    emit_plain_instruction(builder, SWP);

    // Check if it did fizz
    emit_push(builder, 1);
    emit_plain_instruction(builder, EQU);
    emit_jump_if_true(builder, newline_label); // If it did fizz, don't print the number
    emit_jump(builder, else_label);

    link_label(builder, print_buzz_label);
    // Fizz check cleanup
    emit_plain_instruction(builder, SWP);
    emit_plain_instruction(builder, DRP);

    emit_str(builder, "Buzz");
    emit_plain_instruction(builder, PNT);
    emit_jump(builder, newline_label);

    link_label(builder, else_label);
    emit_plain_instruction(builder, DUP);
    emit_plain_instruction(builder, DBG);

    link_label(builder, newline_label);
    emit_str(builder, "\n");
    emit_plain_instruction(builder, PNT);

    emit_plain_instruction(builder, INC);

    emit_plain_instruction(builder, DUP);
    
    emit_push(builder, 100);
    emit_plain_instruction(builder, LT);
    emit_jump_if_true(builder, loop_label);

    emit_plain_instruction(builder, EXT);
}

int main(void) {
    ProgramBuilder pb = {0};
    init_program_builder(&pb);

    build_program(&pb);

    Program program = create_program();
    clone_to_program(&pb, &program);

    // #if DEBUG
    debug_print_program_builder(&pb);
    // #endif

    free_program_builder(&pb);

    #if DEBUG
    print_program(&program);
    #endif

    execute(&program);
    // debug_execute(&program);

    destroy_program(&program);

    return 0;
}
