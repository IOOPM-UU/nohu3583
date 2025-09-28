#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include "linked_list.h"

#define No_Buckets 17

typedef struct entry entry_t;

struct entry
{
    int key;        // holds the key
    char *value;    // holds the value
    entry_t *next;  // points to the next entry (possibly NULL)
};

typedef struct hash_table
{
    entry_t buckets[No_Buckets]; // dummy heads for each bucket
} ioopm_hash_table_t;

unsigned long hash_int(int key)
{
    return key % No_Buckets; // fixed number of buckets
}

/// Find the previous entry for a given key in a bucket
static entry_t* find_previous_entry_for_key(entry_t *bucket_head, int key)
{
    entry_t *current = bucket_head; // bucket_head should be dummy
    while (current->next != NULL)
    {
        if (current->next->key == key)
            return current; // previous entry
        current = current->next;
    }
    return NULL;
}

// Enters a value into the hashtable
static void entry_input(ioopm_hash_table_t *ht, int key, char *value)
{
    int bucket = hash_int(key);

    entry_t *new_entry = malloc(sizeof(entry_t));
    new_entry->key = key;
    new_entry->value = value;

    // Attach after dummy head
    new_entry->next = ht->buckets[bucket].next;
    ht->buckets[bucket].next = new_entry;
}

// Creates a new hash table with calloc, and sets dummy values.
ioopm_hash_table_t *ioopm_hash_table_create()
{
    ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));

    // Initialize dummy heads (already zeroed by calloc)
    for (int i = 0; i < No_Buckets; i++)
    {
        ht->buckets[i].key = 0;       // dummy key
        ht->buckets[i].value = NULL;  // dummy value
        ht->buckets[i].next = NULL;   // start empty
    }

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
            free(tmp->value);   // free strdup'ed string
            free(tmp);
        }
        ht->buckets[i].next = NULL;
    }
    free(ht);
}

// Inserts a new value into the hashtable
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value)
{
    int bucket = hash_int(key);
    entry_t *current = ht->buckets[bucket].next; // skip dummy head

    while (current != NULL)
    {
        if (current->key == key)
        {
            // Key exists → update value
            free(current->value);
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Key not found → create new entry
    entry_input(ht, key, value);
}

// Gets a value from the hashtable, same as lookup but used for testing
char *ioopm_hash_table_get(ioopm_hash_table_t *ht, int key)
{
    int bucket = hash_int(key);
    entry_t *current = ht->buckets[bucket].next;

    while (current != NULL)
    {
        if (current->key == key)
        {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

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

// Looks up a key in the hashtable and returns if it exists or not.
option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key)
{
    entry_t *prev = find_previous_entry_for_key(&ht->buckets[key % No_Buckets], key);

    if (prev != NULL)
    {
        entry_t *curr = prev->next;
        if (curr && curr->key == key && curr->value)
        {
            return Success(curr->value);
        }
    }
    return Failure();
}

// Removes a value from the hashtable, and frees the memory used
void ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key)
{
    entry_t *prev = find_previous_entry_for_key(&ht->buckets[key % No_Buckets], key);

    if (prev != NULL)
    {
        entry_t *target = prev->next; 

        if (target && target->key == key)
        {
            prev->next = target->next; 
            free(target->value);    
            free(target);
        }
    }
}

/// @brief returns the number of key => value entries in the hash table
/// @param ht hash table operated upon
/// @return the number of key => value entries in the hash table
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
            free(tmp->value);
            free(tmp);
        }
        ht->buckets[i].next = NULL; // reset bucket
    }
}

/// @brief return the keys for all entries in a hash map (in no particular order)
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    size_t total_keys = ioopm_hash_table_size(ht);  // number of keys in the table
    if (total_keys == 0) return NULL;

    ioopm_list_t *list = ioopm_linked_list_create();

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next; // skip dummy
        while (current != NULL)
        {
            ioopm_linked_list_append(list, current->key);
            current = current->next;
        }
    }

    return list;
}

/// @brief return the values for all entries in a hash map (in no particular order)
char **ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    size_t total_keys = ioopm_hash_table_size(ht);  // number of keys in the table
    char **values = malloc(sizeof(char *) * total_keys);

    if (!values) return NULL; // always check malloc

    size_t index = 0;

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next; // skip dummy
        while (current != NULL)
        {
            values[index++] = current->value;
            current = current->next;
        }
    }

    return values;
}

/// @brief check if a hash table has an entry with a given key
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, int key)
{
    ioopm_list_t *keys_list = ioopm_hash_table_keys(ht);
    if (keys_list == NULL) return false;

    size_t n_keys = ioopm_linked_list_size(keys_list);
    bool found = false;

    for (size_t i = 0; i < n_keys; i++)
    {
        int k = ioopm_linked_list_get(keys_list, i);
        if (k == key)
        {
            found = true;
            break;  // stop looping once found
        }
    }

    ioopm_linked_list_destroy(keys_list); // free temporary keys list
    return found;
}

/// @brief check if a hash table has an entry with a given value
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, char *value)
{
    size_t n_keys = ioopm_hash_table_size(ht);
    char **values = ioopm_hash_table_values(ht);

    if (!values || n_keys == 0) return false;

    for (size_t i = 0; i < n_keys; i++)
    {
        if (strcmp(values[i], value) == 0)
        {
            free(values);
            return true;
        }
    }

    free(values);
    return false;
}

typedef bool ioopm_predicate(int key, char *value, void *extra);
typedef void ioopm_apply_function(int key, char **value, void *extra);

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
