#include "common.h"
#include <string.h>



// === Equality functions ===
bool int_eq(elem_t a, elem_t b) { return a.i == b.i; }
bool str_eq(elem_t a, elem_t b)
{
    return strcmp((char *)a.p, (char *)b.p) == 0;
}
bool ptr_eq(elem_t a, elem_t b) { return a.p == b.p; }

// === Hash functions ===
int hash_int(elem_t key)
{
    return key.i % 5; // uses same bucket count as hash_table.c
}

int hash_str(elem_t key)
{
    unsigned char *str = key.p;
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash % 5;
}
