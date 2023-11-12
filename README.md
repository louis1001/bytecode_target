# Generic VM

A test Virtual Machine I'm developing to be used as compilation target for my
personal programming languages.

## Execution
### Building

```bash
$ make
```

### Running

```bash
$ ./vm
```

## Usage
### A counter from 0x00 to 0xff
Using the `ProgramBuilder` API, this is how you would create a program that
counts from 0x00 to 0xff:
```c
    u64 loop_label = create_label(builder);
    emit_push(builder, 0x00);

    link_label(builder, loop_label);
    emit_push(builder, 0x01);
    emit_plain_instruction(builder, ADD);
    emit_plain_instruction(builder, DUP);
    emit_plain_instruction(builder, DBG);

    emit_plain_instruction(builder, DUP);

    emit_push(builder, 0xff);
    emit_plain_instruction(builder, EQU);

    emit_jump_if_false(builder, loop_label);

    emit_plain_instruction(builder, EXT);
```

This code would translate to this assembly:
```asm
    PSH 0x00
loop_label:
    PSH 0x01
    ADD
    DUP
    DBG
    DUP
    PSH 0xff
    EQU
    JPF loop_label
    EXT
```

### A fibonacci sequence

```c
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
    emit_push(builder, 200);
    emit_plain_instruction(builder, LT); // c < 0xff

    // emit_plain_instruction(builder, BKP);

    emit_jump_if_true(builder, loop_label);

    emit_plain_instruction(builder, EXT);
```

This code would translate to this assembly:
```asm
    PSH 0x01 # b
    PSH 0x00 # a
loop_label:
    DUP
    ROT
    ADD
    DUP
    DBG
    DUP
    PSH 200
    LT
    JPT loop_label
    EXT
```