#pragma once
#include <stdbool.h>

/// ---------------------- Types ----------------------

// Forward declaration of internal entry struct
typedef struct entry entry_t;

/// Hash table type
typedef struct hash_table ioopm_hash_table_t;

/// ---------------------- API ----------------------

/**
 * Create a new hash table with 17 buckets (each with a dummy head)
 * Returns a pointer to the allocated hash table
 */
ioopm_hash_table_t *ioopm_hash_table_create(void);

/**
 * Destroy a hash table and free all dynamically allocated memory
 */
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/**
 * Insert a key/value pair into the hash table.
 * If the key exists, updates the value.
 */
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value);

/**
 * Find the previous entry in a bucket for a given key
 * Returns pointer to the previous entry, or NULL if key not found
 */
entry_t* find_previous_entry_for_key(entry_t *bucket_head, int key);

/**
 * Compute the bucket index for a given key (internal use)
 */
unsigned long hash_int(int key);

//Test functions to see if everything is working:
int ioopm_hash_table_has_key(ioopm_hash_table_t *ht, int key);
char *ioopm_hash_table_get(ioopm_hash_table_t *ht, int key);

typedef struct option option_t;
struct option
{
  bool success;
  char *value;
};

#define Success(v)      (option_t){ .success = true,  .value = v }
#define Failure()       (option_t){ .success = false, .value = NULL }
#define Successful(o)   ((o).success == true)
#define Unsuccessful(o) ((o).success == false)

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key);
void ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key);



