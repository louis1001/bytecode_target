#ifndef CORE_H
#define CORE_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define ERROR(x...) printf("Error: " x); exit(-1)

// Int types
#define u8 u_int8_t
#define u64 u_int64_t
#define usize size_t
// What type of data is stored in the stack and registers
#define BASE_T u8

#endif // CORE_H