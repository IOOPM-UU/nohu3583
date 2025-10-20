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
int cmp_str(const void *a, const void *b)
{
    const elem_t *x = a;
    const elem_t *y = b;
    return strcmp((char *)x->p, (char *)y->p);
}

int cmp_str_num(const void *a, const void *b)
{
    const elem_t *x = a;
    const elem_t *y = b;
    const char *sx = (char *)x->p;
    const char *sy = (char *)y->p;

    // Compare first letter
    if (sx[0] != sy[0])
        return (unsigned char)sx[0] - (unsigned char)sy[0];

    // Compare numeric part
    int nx = atoi(sx + 1);
    int ny = atoi(sy + 1);
    return nx - ny;
}

// Convert hash table keys into a sorted array
elem_t *get_keys(ioopm_hash_table_t *ht)
{
    ioopm_list_t *keys_list = ioopm_hash_table_keys(ht);
    size_t size = keys_list->size;

    if (size == 0)
    {
        ioopm_linked_list_destroy(keys_list);
        return NULL;
    }

    elem_t *arr = calloc(size, sizeof(elem_t));
    for (size_t i = 0; i < size; i++)
    {
        arr[i] = ioopm_linked_list_get(keys_list, i);
    }

    ioopm_linked_list_destroy(keys_list);
    return arr;
}


// Sort array of keys lexicographically
elem_t *sort_keys(elem_t *arr, size_t size)
{
    qsort(arr, size, sizeof(elem_t), cmp_str);
    return arr;
}

elem_t *sort_stock(elem_t *arr, size_t size){
    qsort(arr,size, sizeof(elem_t), cmp_str_num);
}
