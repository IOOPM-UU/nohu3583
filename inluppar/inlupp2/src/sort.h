#define _POSIX_C_SOURCE 200809L
#include "common.h"
#include "hash_table.h"
#include "iterator.h"
#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define Delimiters "+-#@()[]{}.,:;!? \t\n\r"

// Comparison function for qsort
int cmp_str(const void *a, const void *b);


// Convert hash table keys into a sorted array
elem_t *get_keys(ioopm_hash_table_t *ht); 


// Sort array of keys lexicographically
elem_t *sort_keys(elem_t *arr, size_t size);


elem_t *sort_stock(elem_t *arr, size_t size);

