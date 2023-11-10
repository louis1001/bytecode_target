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

    emit_nop(&pb);
    emit_push(&pb, 42);
    emit_push(&pb, 43);
    emit_instruction(&pb, ADD);
    u8 label = create_label(&pb);

    emit_push(&pb, 86);
    emit_instruction(&pb, EQU);
    
    emit_jump_if_true(&pb, label);
    emit_push(&pb, 0x32);
    emit_push(&pb, 0x63);
    emit_instruction(&pb, ADD);
    emit_instruction(&pb, DBG);
    
    emit_instruction(&pb, EXT);

    Program program2 = create_program();
    clone_to_program(&pb, &program2);

    print_program(&program2);

    free_program_builder(&pb);

    // execute(&program2);

    destroy_program(&program2);

    return 0;
}