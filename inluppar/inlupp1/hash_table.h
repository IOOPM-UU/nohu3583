#pragma once
#include <stdbool.h>
#include "linked_list.h"

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
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_func *func, ioopm_eq_function *eq_func);

/**
 * Destroy a hash table and free all dynamically allocated memory
 */
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/**
 * Insert a key/value pair into the hash table.
 * If the key exists, updates the value.
 */
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);


elem_t ioopm_hash_table_get(ioopm_hash_table_t *ht, elem_t key); 

typedef struct option option_t;
struct option
{
    bool success;
    elem_t value;
};

#define Success(v)      (option_t){ .success = true,  .value = v }
#define Failure()       (option_t){ .success = false, .value = (elem_t){ .p = NULL } }
#define Successful(o)   ((o).success == true)
#define Unsuccessful(o) ((o).success == false)

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key);
void ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key);

/// @brief returns the number of key => value entries in the hash table
/// @param h hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht); 

/// @brief checks if the hash table is empty
/// @param h hash table operated upon
/// @return true is size == 0, else false
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param h hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param h hash table operated upon
/// @return an array of keys for hash table h
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht);

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param h hash table operated upon
/// @return an array of values for hash table h
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht);

/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief return the values for all entries in a hash map (in no particular order)
// should be a linked list instead.
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht);

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value);

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg);

/// @brief apply a function to all entries in a hash table
/// @param h hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function *apply_fun, void *arg);

