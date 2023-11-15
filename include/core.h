#ifndef CORE_H
#define CORE_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

// Float types
#define f32 float32_t
#define f64 float64_t

// What type of data is stored in the stack and registers
#define BASE_T u64
// TODO: Figure out a macro for string format of BASE_T conditionally (u64 -> %llu, u32 -> %u, etc.)

// String Buffer
typedef struct {
    usize count;
    usize capacity;
    char* str;
} StringBuffer;

StringBuffer create_string_buffer(usize initial_capacity);
void init_string_buffer(StringBuffer* buffer, usize initial_capacity);
void free_string_buffer(StringBuffer* buffer);

void grow_string_buffer_to_fit(StringBuffer* buffer, usize new_elements);

void append_string_buffer(StringBuffer* buffer, char* new_str);
void append_char_string_buffer(StringBuffer* buffer, char c);

// String functions
bool strbeingswith(char* str, char* prefix);

typedef struct {
    bool taken;
    StringBuffer key;
    u64 value;
} HashEntry;

// HashMap
typedef struct {
    usize capacity;
    u32 capacity_prime_index;
    HashEntry *data;
    usize count;
} HashMap;

HashMap create_hash_map(void);
void init_hash_map(HashMap* map);
void free_hash_map(HashMap* map);

void grow_hash_map(HashMap* map);

HashEntry *find_entry(HashMap* map, char* key); // FIXME: Accept StringBuffer too?
HashEntry *insert_hash_map(HashMap* map, char* key, u64 value);

#endif // CORE_H
