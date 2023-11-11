#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "opcodes.h"
#include "program.h"
#include "vm.h"
#include "program_builder.h"

void build_program(ProgramBuilder *builder) {
    // A fibonacci program:
    emit_push(builder, 1); // b
    emit_push(builder, 0); // a

    u8 loop_label = create_label(builder);
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
    emit_push(builder, 200);
    emit_plain_instruction(builder, LT); // c < 0xff

    // emit_plain_instruction(builder, BKP);

    emit_jump_if_true(builder, loop_label);

    emit_plain_instruction(builder, ADD); // stack is: 1

    emit_plain_instruction(builder, EXT);
}

int main(void) {
    // Now with program builder
    ProgramBuilder pb = {0};
    init_program_builder(&pb);

    build_program(&pb);

    Program program2 = create_program();
    clone_to_program(&pb, &program2);

    #if DEBUG
    print_program(&program2);
    #endif // DEBUG

    free_program_builder(&pb);

    execute(&program2);

    destroy_program(&program2);

    return 0;
}