#ifndef CORE_H
#define CORE_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef DEBUG
#define DEBUG 0
#endif // DEBUG
#ifndef VERBOSE
#define VERBOSE 0
#endif // DEBUG

#define LOG(x...) do { if (DEBUG) { printf("INFO: " x); } } while(0)
#define VERBOSE_LOG(x...) do { if (VERBOSE) { printf("INFO: " x); } } while(0)
#define ERROR(x...) printf("Error: " x); exit(-1)
#define ASSERT(condition, x...) do { if (!(condition)) { printf("Assertion Error [" #condition "]: " x); exit(-1); } } while(0)

// Int types
#define u8 u_int8_t
#define u64 u_int64_t
#define usize size_t
// What type of data is stored in the stack and registers
#define BASE_T u8

#endif // CORE_H