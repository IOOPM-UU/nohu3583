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

static void lowercase_inplace(char *s)
{
    for (unsigned char *p = (unsigned char *) s; *p; ++p)
    {
        *p = (char) tolower(*p);
    }
}

// Convert hash table keys into a sorted array
elem_t *get_keys(ioopm_hash_table_t *ht, size_t *out_size)
{
    ioopm_list_t *keys_list = ioopm_hash_table_keys(ht);
    size_t size = keys_list->size;

    if (size == 0)
    {
        ioopm_linked_list_destroy(keys_list);
        *out_size = 0;
        return NULL;
    }

    elem_t *arr = calloc(size, sizeof(elem_t));
    for (size_t i = 0; i < size; i++)
    {
        arr[i] = ioopm_linked_list_get(keys_list, i);
    }

    ioopm_linked_list_destroy(keys_list);
    *out_size = size;
    return arr;
}

// Print one key-frequency pair
void print_key_frequency(ioopm_hash_table_t *ht, elem_t key)
{
    option_t result = ioopm_hash_table_lookup(ht, key);
    if (result.success)
    {
        printf("%s: %d\n", (char *)key.p, result.value.i);
    }
}

// Sort array of keys lexicographically
void sort_keys(elem_t *arr, size_t size)
{
    qsort(arr, size, sizeof(elem_t), cmp_str);
}

/// Insert a single word into the hash table
void process_word(const char *word, ioopm_hash_table_t *ht)
{
    char *key_copy = strdup(word);
    if (!key_copy) {
        fprintf(stderr, "Memory allocation failed for word: %s\n", word);
        exit(EXIT_FAILURE);
    }

    lowercase_inplace(key_copy); 
    ioopm_hash_table_insert_freq(ht, ptr_elem(key_copy));
}

/// Read a file and process all words
void process_file(const char *filename, ioopm_hash_table_t *ht)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return;
    }

    char *buf = NULL;
    size_t len = 0;

    while (getline(&buf, &len, f) != -1)
    {
        // Tokenize the line using delimiters
        for (char *word = strtok(buf, Delimiters);
             word != NULL;
             word = strtok(NULL, Delimiters))
        {
            if (*word)  // skip empty tokens
            {
                process_word(word, ht);
            }
        }
    }

    free(buf);   // free the line buffer
    fclose(f);
}

int main(int argc, char *argv[])
{   
    for(int i = 0; i < 1; i++){
        if (argc < 2)
        {
            puts("Usage: freq-count file1 ... filen");
            return 1;
        }

        // Create hash table
        ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_str, str_eq);
        ht->should_free_keys = true;

        // Process all input files
        for (int i = 1; i < argc; i++)
        {
            process_file(argv[i], ht);
        }

        // Extract keys and sort them
        size_t size = 0;
        elem_t *keys = get_keys(ht, &size);

        if (keys)
        {
            sort_keys(keys, size);

            // Print word frequencies
            for (size_t i = 0; i < size; i++)
            {
                print_key_frequency(ht, keys[i]);
            }   
            
            // CLEANUP: Remove each entry from hash table and free the strdup'ed key
            for (size_t i = 0; i < size; i++)
            {
                ioopm_hash_table_remove(ht, keys[i]);
                free(keys[i].p);  // Free the strdup'ed string
            }
            free(keys);
        }

        // Destroy hash table (now empty, so no leaks)
        ioopm_hash_table_destroy(ht);
    }
    return 0;
}