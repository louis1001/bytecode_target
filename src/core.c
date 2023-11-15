#include "core.h"
#include <string.h>

#define PRIMES_COUNT 26
usize capacity_primes[PRIMES_COUNT] = {
    53,
    97,
    193,
    389,
    769,
    1543,
    3079,
    6151,
    12289,
    24593,
    49157,
    98317,
    196613,
    393241,
    786433,
    1572869,
    3145739,
    6291469,
    12582917,
    25165843,
    50331653,
    100663319,
    201326611,
    402653189,
    805306457,
    1610612741
};

StringBuffer create_string_buffer(usize initial_capacity) {
    StringBuffer buf = {0};
    init_string_buffer(&buf, initial_capacity);

    return buf;
}

void init_string_buffer(StringBuffer* sb, usize initial_capacity) {
    sb->str = calloc(initial_capacity + 1, sizeof(char));
    ASSERT(sb->str != NULL, "Able to allocate initial space for string\n");
    sb->capacity = initial_capacity + 1;
}

void free_string_buffer(StringBuffer* sb) {
    free(sb->str);
    sb->capacity = sb->count = 0;
}

void append_string_buffer(StringBuffer* sb, char* new_str) {
    usize len = strlen(new_str);
    grow_string_buffer_to_fit(sb, len);

    char *end_pos = sb->str + sb->count;
    memcpy(end_pos, new_str, len);

    sb->count += len;
}

void grow_string_buffer_to_fit(StringBuffer* sb, usize new_elements) {
    usize needed_space = sb->count + new_elements + 1;

    if (sb->capacity < needed_space) {
        // Allocate more space for the string
        usize new_capacity = sb->capacity * 2;
        while (new_capacity < needed_space) {
            new_capacity *= 2;
        }

        sb->str = realloc(sb->str, new_capacity * sizeof(char));
        ASSERT(sb->str != NULL, "Able to resize memory for the string\n");
        memset(sb->str + (sb->count * sizeof(char)), 0, (new_capacity - sb->count) * sizeof(char));
        sb->capacity = new_capacity;
    }
}

void append_char_string_buffer(StringBuffer* sb, char c) {
    grow_string_buffer_to_fit(sb, 1);

    sb->str[sb->count] = c;
    sb->count += 1;
}

bool strbeingswith(char* str, char* prefix) {
    usize len = strlen(prefix);
    return strncmp(str, prefix, len);
}


// HashMap

HashMap create_hash_map(void) {
    HashMap map = {0};

    init_hash_map(&map);

    return map;
}

void init_hash_map(HashMap* map) {
    map->capacity_prime_index = 0;
    usize initial_capacity = capacity_primes[0];

    map->data = calloc(initial_capacity, sizeof(HashEntry));
    if (map->data == NULL) {
        ERROR("Could not allocate initial space for HashMap.\n");
    }

    map->capacity = initial_capacity;
    map->capacity_prime_index++;
}

void free_hash_map(HashMap* map) {
    for(usize i = 0; i < map->capacity; i++) {
        HashEntry *entry = &map->data[i];
        if (entry->key.capacity > 0) { // FIXME: Puede fallar?
            free_string_buffer((StringBuffer*) &entry->key); // const cast c:
        }
    }

    free(map->data);
}

void grow_hash_map(HashMap* map) {
    HashEntry *old_entries = map->data;
    usize old_capacity = map->capacity;

    usize new_capacity;
    if (map->capacity_prime_index >= PRIMES_COUNT) {
        new_capacity = old_capacity * 2 + 1;
    } else {
        new_capacity = capacity_primes[map->capacity_prime_index++];
    }

    map->data = calloc(new_capacity, sizeof(HashEntry));
    if (map->data == NULL) {
        ERROR("Could not allocate new space for growing HashMap.\n");
    }

    map->capacity = new_capacity;

    for(usize i = 0; i < old_capacity; i++) {
        HashEntry *bucket = &old_entries[i];

        if (bucket->taken && bucket->key.capacity > 0) {
            insert_hash_map(map, bucket->key.str, bucket->value); // FIXME: This remakes the StringBuffer.
            free_string_buffer(&bucket->key);
        }
    }
    
    free(old_entries);
}

// https://github.com/haipome/fnv
u64 hash_string(char *str)
{
    u64 hval = 0xcbf29ce484222325ULL;
    u8 *s = (u8*)str;	/* unsigned string */

    /*
     * FNV-1 hash each octet of the string
     */
    while (*s) {
        hval += (hval << 1) + (hval << 4) + (hval << 5) +
            (hval << 7) + (hval << 8) + (hval << 40);

        /* xor the bottom with the current octet */
        hval ^= (u64)*s++;
    }

    /* return our new hash value */
    return hval;
}

HashEntry *find_entry(HashMap* map, char* key) {
    u64 hash = hash_string(key);

    usize index = hash % map->capacity;

    printf("Finding value for `%s`\n", key);
    printf("The hash for `%s` is: 0x%llx\n", key, hash);
    printf("The first bucket selected is %zu\n", index);

    HashEntry *bucket = &map->data[index];
    if (bucket->taken) {
        return bucket;
    }

    return NULL;
}

HashEntry *insert_hash_map(HashMap *map, char *key, u64 value) {
    float usage = 1.0 * map->count / map->capacity;
    if (usage > 0.75) {
        grow_hash_map(map);
    }
    
    u64 hash = hash_string(key);

    u64 index = hash%map->capacity;
    u64 original_index = index;

    printf("Inserting value for `%s`\n", key);
    printf("The hash for `%s` is: 0x%llx\n", key, hash);
    printf("The first bucket selected is %llu\n", index);
    HashEntry *bucket = &map->data[index];
    while (bucket->taken) {
        if (strcmp(bucket->key.str, key) == 0) {
            // If it exists, just update the value.
            bucket->value = value;
            return bucket;
        }

        index = (index + 1) % map->capacity;
        if (index == original_index) {
            // Could not find a valid bucket. Should grow.
            ERROR("Could not find a valid bucket. TODO: This should never happen.");
        }
    }

    if (!bucket->taken) {
        if (bucket->key.capacity > 0) {
            free_string_buffer((StringBuffer *)&bucket->key);
        }
        StringBuffer new_key = create_string_buffer(strlen(key));

        append_string_buffer(&new_key, key);
        bucket->key = new_key;
        bucket->value = value;
        bucket->taken = true;
        map->count++;

        return bucket;
    }

    return NULL;
}

