#include "assembler.h"
#include "core.h"
#include "program.h"
#include "program_builder.h"
#include "vm.h"
#include <ctype.h>

void init_assembler(Assembler *assembler) {
    init_hash_map(&assembler->labels);
}

void free_assembler(Assembler *assembler) {
    free_hash_map(&assembler->labels);
}

void assemble_ignore_spaces(Assembler *assembler) {
    while (
        ((assembler->current_pos) < assembler->count) &&
        isspace(assembler->code[assembler->current_pos])
    ) {
        assembler->current_pos++;
    }
}

HashEntry *assemble_label_literal(Assembler *assembler, bool *existing) {
    StringBuffer literal = create_string_buffer(4);
    usize code_len = assembler->count;
    for (;assembler->current_pos < code_len; assembler->current_pos++) {
        char c = assembler->code[assembler->current_pos];
        if (isspace(c)) { break; }

        append_char_string_buffer(&literal, c);
    }
    
    HashMap *labels = &assembler->labels;

    HashEntry *entry = find_entry(labels, literal.str);
    if (!entry) {
        *existing = false;
        entry = insert_hash_map(labels, literal.str, 0);
    } else {
        *existing = true;
    }
    
    return entry;
}

u64 assemble_u64_literal(Assembler *assembler) {
    StringBuffer literal = create_string_buffer(4);
    usize code_len = assembler->count;
    for (;assembler->current_pos < code_len; assembler->current_pos++) {
        char c = assembler->code[assembler->current_pos];
        if (isspace(c)) { break; }

        append_char_string_buffer(&literal, c);
    }

    u64 result = strtoull(literal.str, NULL, 0);
    VERBOSE_LOG("Trying to parse `%s` as numeric operand: `%llu`\n", literal.str, result);

    free_string_buffer(&literal);

    return result;
}

StringBuffer assemble_string_literal(Assembler *assembler) {
    if (assembler->code[assembler->current_pos] != '\"') {
        ERROR("Expected a string literal starting with \"\n");
    }

    assembler->current_pos++;
    StringBuffer sb = create_string_buffer(8);

    while (assembler->code[assembler->current_pos] != '\"') {
        char c = assembler->code[assembler->current_pos];
        if (c == '\\') {
            assembler->current_pos++;
            ASSERT(assembler->current_pos < assembler->count, "Unexpected end of file trying to escape character");

            char escaped = assembler->code[assembler->current_pos];
            switch (escaped) {
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                default:
                    c = escaped;
                    break;
            }
        }
        
        append_char_string_buffer(&sb, c);
        assembler->current_pos++;

        if (assembler->current_pos > assembler->count) {
            ERROR("String literal was not closed.\n");
        }
    }

    VERBOSE_LOG("Closing string literal with '%c'\n", assembler->code[assembler->current_pos]);
    assembler->current_pos++;
    return sb;
}

void resolve_instruction(Assembler *assembler, StringBuffer *buf, ProgramBuilder *pb) {
    // Then I have a complete instruction
    OpCode opcode = NOP;
    ASSERT(string_to_opcode(&opcode, buf->str), "Invalid instruction `%s`", buf->str);

    LOG("Found instruction `%s`\n", buf->str);

    char c;

    switch (opcode) {
        case PSH: {
            assemble_ignore_spaces(assembler);
            c = assembler->code[assembler->current_pos];

            if (c == '\'') {
                // Handle a label operand
                assembler->current_pos++;

                bool is_existing = false;
                HashEntry *label_in_table = assemble_label_literal(assembler, &is_existing);

                u64 operand;
                if (is_existing) {
                    operand = label_in_table->value;
                } else {
                    u64 label_id = create_label(pb);
                    operand = label_id;
                    label_in_table->value = label_id;
                }
                emit_push_label(pb, operand);
            } else {
                u64 operand = assemble_u64_literal(assembler);
                emit_push(pb, operand);
            }
            
            break;
        }
        case STR: {
            assemble_ignore_spaces(assembler);
            StringBuffer literal = assemble_string_literal(assembler);

            emit_str(pb, literal.str);

            free_string_buffer(&literal);
            break;
        }
        default: { // In case the instruction has no operands, just emit a plain instruction
            emit_plain_instruction(pb, opcode);
            break;
        }
    }
}

Program assemble(Assembler *assembler) {
    ProgramBuilder pb = {0};
    init_program_builder(&pb);
    StringBuffer buf = {0};
    init_string_buffer(&buf, 4);

    usize code_len = assembler->count;
    for (; assembler->current_pos < code_len; assembler->current_pos++) {
        char c = assembler->code[assembler->current_pos];
        if (c == '#') { // Ignore comments until new line
            while (++assembler->current_pos < code_len) {
                c = assembler->code[assembler->current_pos];
                if (c == '\n') {
                    break;
                }
            }
        }

        if (isspace(c)) {
            if (buf.count > 0) {
                resolve_instruction(assembler, &buf, &pb);
            } else {
                continue;
            }

            free_string_buffer(&buf);
            init_string_buffer(&buf, 4);

            continue;
        } else if (c == ':') {
            assembler->current_pos++;

            HashEntry *entry = find_entry(&assembler->labels, buf.str);

            u64 label_id;
            if (!entry) {
                LOG("Found a new label called `%s`\n", buf.str);
                label_id = create_label(&pb);
                entry = insert_hash_map(&assembler->labels, buf.str, label_id);
            } else {
                label_id = entry->value;
            }

            link_label(&pb, label_id);

            free_string_buffer(&buf);
            init_string_buffer(&buf, 4);

            continue;
        } else if (buf.count == 0) {}

        append_char_string_buffer(&buf, c);
    }

    if (buf.count > 0) {
        resolve_instruction(assembler, &buf, &pb);
    }

    free_string_buffer(&buf);
    Program p = {0};
    clone_to_program(&pb, &p);
    debug_print_program_builder(&pb);

    free_program_builder(&pb);

    return p;
}

void debug_print_hash_map(HashMap *map) {
    printf("Debug Hash Map:\n");

    for (usize i = 0; i < map->capacity; i++) {
        HashEntry *bucket = &map->data[i];

        if(bucket->taken) {
            printf("[%zu] `%s` -> %llu\n", i, bucket->key.str, bucket->value);
        } else {
            printf("[%zu] Empty bucket\n", i);
        }
    }
}

Program assemble_file(char *input_file) {
    Assembler assembler = {0};
    init_assembler(&assembler);
    usize file_size;
    char *contents = read_all_from_file(input_file, &file_size);

    assembler.current_pos = 0;
    assembler.count = file_size;
    assembler.code = contents;

    Program program = assemble(&assembler);

    #if DEBUG
    debug_print_hash_map(&assembler.labels);
    #endif // DEBUG
    
    free_assembler(&assembler);
    free(contents);

    return program;
}
