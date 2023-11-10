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
    
    u8 label1 = create_label(&pb);
    u8 label2 = create_label(&pb);
    u8 label3 = create_label(&pb);
    u8 label4 = create_label(&pb);

    emit_instruction(&pb, PSH);
    emit_instruction(&pb, LBL);
    emit_u8(&pb, label1);
    emit_nop(&pb);
    emit_nop(&pb);
    emit_nop(&pb);
    link_label(&pb, label1);

    emit_instruction(&pb, PSH);
    emit_instruction(&pb, LBL);
    emit_u8(&pb, label2);
    emit_nop(&pb);
    emit_nop(&pb);
    emit_nop(&pb);
    link_label(&pb, label2);

    emit_instruction(&pb, PSH);
    emit_instruction(&pb, LBL);
    emit_u8(&pb, label3);
    emit_nop(&pb);
    emit_nop(&pb);
    emit_nop(&pb);
    link_label(&pb, label3);

    emit_instruction(&pb, PSH);
    emit_instruction(&pb, LBL);
    emit_u8(&pb, label4);
    emit_nop(&pb);
    emit_nop(&pb);
    emit_nop(&pb);
    link_label(&pb, label4);

    emit_instruction(&pb, EXT);

    Program program2 = create_program();
    clone_to_program(&pb, &program2);

    print_program(&program2);

    free_program_builder(&pb);

    // execute(&program2);

    destroy_program(&program2);

    return 0;
}