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
    // Stack => | c a |

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

### A FizzBuzz Example
```c
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
```

This code would translate to this assembly:
```asm
    PSH 0x00000001
loop:
    DUP 
    PSH 0x00000003
    MOD 
    PSH 0x00000000
    EQU
    PSH print_fizz
    JPT 
    PSH 0x00000000 
    SWP 
    PSH check_buzz
    JMP 
print_fizz:
    STR "Fizz"
    PNT 
    PSH 0x00000001 
    SWP 
check_buzz:
    DUP 
    PSH 0x00000005 
    MOD 
    PSH 0x00000000 
    EQU 
    PSH print_buzz
    JPT 
    SWP 
    PSH 0x00000001 
    EQU 
    PSH newline
    JPT 
    PSH else
    JMP 
print_buzz:
    SWP 
    DRP 
    STR "Buzz"
    PNT 
    PSH newline
    JMP 
else:
    DUP 
    DBG 
newline:
    STR "\n"
    PNT 
    INC 
    DUP 
    PSH 0x00000064
    LT 
    PSH loop
    JPT 
    EXT
```