#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include "linked_list.h"
#include "common.h"

#define No_Buckets 17

typedef struct entry entry_t;

struct entry
{
    elem_t key;       // holds the key
    elem_t value;     // holds the value
    entry_t *next;    // points to the next entry (possibly NULL)
};

typedef struct hash_table
{
    entry_t buckets[No_Buckets]; // dummy heads for each bucket
    ioopm_hash_func *func;
    ioopm_eq_function *eq_func;

} ioopm_hash_table_t;

/// Find the previous entry for a given key in a bucket
static entry_t* find_previous_entry_for_key(ioopm_hash_table_t *ht ,entry_t *bucket_head, elem_t key)
{
    entry_t *current = bucket_head; // bucket_head should be dummy
    while (current->next != NULL)
    {
        if (ht->eq_func(current->next->key, key))
            return current; // previous entry
        current = current->next;
    }
    return NULL;
}

// Enters a value into the hashtable
static void entry_input(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
    int bucket = ht->func(key);

    entry_t *new_entry = malloc(sizeof(entry_t));
    new_entry->key = key;
    new_entry->value = value;

    // Attach after dummy head
    new_entry->next = ht->buckets[bucket].next;
    ht->buckets[bucket].next = new_entry;
}

// Creates a new hash table with calloc, and sets dummy values.
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_func *func, ioopm_eq_function *eq_func)
{
    ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
    ht->func = func;
    ht->eq_func = eq_func;
    return ht;
}

// Destroys/removes the hashtable, by freeing it
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next; // skip dummy head
        while (current != NULL)
        {
            entry_t *tmp = current;
            current = current->next;
            free(tmp);
        }
        ht->buckets[i].next = NULL;
    }
    free(ht);
}

// Inserts a new value into the hashtable
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
    int bucket = ht->func(key);
    entry_t *current = ht->buckets[bucket].next; // skip dummy head

    while (current != NULL)
    {
        if (ht->eq_func(current->key, key))
        {
            // Key exists → update value
            current->value = value;
            return;
        }
        current = current->next;
    }

    entry_input(ht, key, value);
}

// Gets a value from the hashtable, same as lookup but used for testing
elem_t ioopm_hash_table_get(ioopm_hash_table_t *ht, elem_t key)
{
    int bucket = ht->func(key);
    entry_t *current = ht->buckets[bucket].next;

    while (current != NULL)
    {
        if (ht->eq_func(current->key, key))
        {
            return current->value;
        }
        current = current->next;
    }
    return (elem_t){ .p = NULL };
}

/* ---------------- OPTION + SUCCESS / FAILURE ---------------- */

typedef struct option option_t;
struct option
{
    bool success;
    elem_t value;
};

#define SuccessElem(e)   (option_t){ .success = true,  .value = (e) }
#define SuccessInt(i)    (option_t){ .success = true,  .value = (elem_t){ .i = (i) } }
#define SuccessPtr(p)    (option_t){ .success = true,  .value = (elem_t){ .p = (p) } }
#define Failure()        (option_t){ .success = false, .value = (elem_t){ .p = NULL } }
#define Successful(o)    ((o).success == true)
#define Unsuccessful(o)  ((o).success == false)

/* ---------------- LOOKUP ---------------- */

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
    entry_t *prev = find_previous_entry_for_key(ht, &ht->buckets[ht->func(key)], key);

    if (prev != NULL)
    {
        entry_t *curr = prev->next;
        if (curr && ht->eq_func(curr->key, key))
        {
            return SuccessElem(curr->value);   // return the union directly
        }
    }

    return Failure();
}

// Removes a value from the hashtable, and frees the memory used
void ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
    entry_t *prev = find_previous_entry_for_key(ht, &ht->buckets[ht->func(key)], key);

    if (prev != NULL)
    {
        entry_t *target = prev->next; 

        if (target && ht->eq_func(target->key, key))
        {
            prev->next = target->next; 
            free(target);
        }
    }
}

/// @brief returns the number of key => value entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
    size_t count = 0;

    for(int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL)
        {
            count++;
            current = current->next;
        }
    }

    return count;
}

/// @brief checks if the hash table is empty
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
    return ioopm_hash_table_size(ht) == 0;
}

// Removes all entries from the hashtable but keeps the table itself
void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL)
        {
            entry_t *tmp = current;
            current = current->next;
            free(tmp);
        }
        ht->buckets[i].next = NULL; // reset bucket
    }
}

/// @brief return the keys for all entries in a hash map (in no particular order)
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    size_t total_keys = ioopm_hash_table_size(ht);
    if (total_keys == 0) return NULL;

    ioopm_list_t *list = ioopm_linked_list_create(ht->eq_func);

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL)
        {
            ioopm_linked_list_append(list, current->key);
            current = current->next;
        }
    }

    return list;
}

/// @brief return the values for all entries in a hash map (in no particular order)
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    size_t total_keys = ioopm_hash_table_size(ht);
    if (!total_keys) return NULL;

    ioopm_list_t *list = ioopm_linked_list_create(ht->eq_func);

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL)
        {
            ioopm_linked_list_append(list, current->value);
            current = current->next;
        }
    }

    return list;
}

/// @brief check if a hash table has an entry with a given key
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
    ioopm_list_t *keys_list = ioopm_hash_table_keys(ht);
    if (keys_list == NULL) return false;

    size_t n_keys = ioopm_linked_list_size(keys_list);
    bool found = false;

    for (size_t i = 0; i < n_keys; i++)
    {
        elem_t k = ioopm_linked_list_get(keys_list, i);
        if (ht->eq_func(k, key))
        {
            found = true;
            break;
        }
    }

    ioopm_linked_list_destroy(keys_list);
    return found;
}

/// @brief check if a hash table has an entry with a given value
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
    size_t n_keys = ioopm_hash_table_size(ht);
    ioopm_list_t *key_list = ioopm_hash_table_keys(ht);
    bool found = false;

    if (key_list == NULL) return false;

    for (size_t i = 0; i < n_keys; i++)
    {
        elem_t value_l = ioopm_linked_list_get(key_list, i);
        if(ht->eq_func(value_l,value)){
            found = true;
        }
    }
    ioopm_linked_list_destroy(key_list);
    return found;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function *func, void *arg)
{
    for (int i = 0; i < No_Buckets; i++) {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL) {
            func(current->key, &current->value, arg);
            current = current->next; 
        }
    }
}

/// @brief check if a predicate is satisfied by all entries in a hash table
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg){
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL)
        {
            if(!pred(current->key, current->value, arg)){
                return false;
            }
            current = current->next;
        }
    }
    return true;
}

/// @brief check if a predicate is satisfied by any entry in a hash table
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg){
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL)
        {
            if(pred(current->key, current->value, arg)){
                return true;
            }
            current = current->next;
        }
    }
    return false;
}
