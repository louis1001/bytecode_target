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
    // A fibonacci program:
    emit_push(builder, 1); // b
    emit_push(builder, 0); // a

    u64 loop_label = create_label(builder);
    link_label(builder, loop_label);
    // Stack => top | a b | bottom
    emit_plain_instruction(builder, DUP);
    // Stack => | a a b |
    emit_plain_instruction(builder, ROT);
    // Stack => | b a a |
    emit_plain_instruction(builder, ADD);
    // Stack => | c b |

    emit_plain_instruction(builder, DUP);
    emit_plain_instruction(builder, DBG);

    emit_plain_instruction(builder, DUP); // c
    emit_push(builder, 0xffffffffff);
    emit_plain_instruction(builder, LT); // c < 0xff

    emit_jump_if_true(builder, loop_label);

    emit_str(builder, "Done!\n");

    emit_plain_instruction(builder, PNT);

    emit_plain_instruction(builder, EXT);
}

int main(void) {
    ProgramBuilder pb = {0};
    init_program_builder(&pb);

    build_program(&pb);

    Program program = create_program();
    clone_to_program(&pb, &program);

    debug_print_program_builder(&pb);

    free_program_builder(&pb);
    print_program(&program);

    execute(&program);

    destroy_program(&program);

    return 0;
}
