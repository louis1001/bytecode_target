#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "opcodes.h"
#include "program.h"
#include "vm.h"
#include "program_builder.h"

int main(void) {
    // Now with program builder
    ProgramBuilder pb = {0};
    init_program_builder(&pb);

    // Testing what happens if you link a label that points back in the code
    emit_nop(&pb);
    u8 loop_label = create_label(&pb);
    emit_push(&pb, 0x00);

    link_label(&pb, loop_label); /* -> */ emit_push(&pb, 0x01);
    emit_plain_instruction(&pb, ADD);
    emit_plain_instruction(&pb, DUP);
    emit_plain_instruction(&pb, DBG);

    emit_plain_instruction(&pb, DUP);

    emit_push(&pb, 0xff);
    emit_plain_instruction(&pb, EQU);

    emit_jump_if_false(&pb, loop_label);

    emit_plain_instruction(&pb, EXT);

    Program program2 = create_program();
    clone_to_program(&pb, &program2);

    print_program(&program2);

    free_program_builder(&pb);

    execute(&program2);

    destroy_program(&program2);

    return 0;
}