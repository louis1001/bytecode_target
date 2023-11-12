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

// https://www.decompile.com/cpp/faq/file_and_line_error_string.htm

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__) ": "

#define LOG(x...) do { if (DEBUG) { printf("INFO: " x); } } while(0)
#define VERBOSE_LOG(x...) do { if (VERBOSE) { printf("INFO: " x); } } while(0)
#define ERROR(x...) printf(AT"Error: " x); exit(-1)
#define TODO(x...) printf(AT"Not implemented: " x); exit(-1)
#define ASSERT(condition, x...) do { if (!(condition)) { printf(AT "Assertion Error [" #condition "]: " x); exit(-1); } } while(0)

// Int types
#define u8 u_int8_t
#define u64 u_int64_t
#define u32 u_int32_t
#define usize size_t

// What type of data is stored in the stack and registers
#define BASE_T u64
// TODO: Figure out a macro for string format of BASE_T conditionally (u64 -> %llu, u32 -> %u, etc.)

#endif // CORE_H
