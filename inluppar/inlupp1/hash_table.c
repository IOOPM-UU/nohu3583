#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

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

//Enters a value into the hashtable
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

//Creates a new hash table with calloc, and sets dummy values.
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

    // Example entries for demonstration
    entry_input(ht, 1, "foo");
    entry_input(ht, 2, "bar");
    entry_input(ht, 3, "baz");
    entry_input(ht, 4, "previous_of_208");
    entry_input(ht, 208, "some_value");

    return ht;
}

//Destroys/removes the hashtable, by freeing it
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
    }
    free(ht);
}

//Inserts a new value into the hashtable
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value)
{
    int bucket = hash_int(key);
    entry_t *current = ht->buckets[bucket].next; // skip dummy head

    while (current != NULL)
    {
        if (current->key == key)
        {
            // Key exists → update value
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Key not found → create new entry
    entry_input(ht, key, value);
}

//Checks if hash_table has a key. 
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, int key)
{
    int bucket = hash_int(key);
    entry_t *current = ht->buckets[bucket].next;

    while (current != NULL)
    {
        if (current->key == key)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

//Gets a value from the hashtable, same as lookup but used for testing
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

//Looks up a key in the hashtable and returns if it exists or not.
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

//Removes a value from the hashtable, and frees the memory used
void ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key)
{
    entry_t *prev = find_previous_entry_for_key(&ht->buckets[key % No_Buckets], key);

    if (prev != NULL)
    {
        entry_t *target = prev->next; 

        if (target && target->key == key)
        {
            prev->next = target->next; 
            // free(target->value);    
            free(target);
        }
    }
}


/// @brief returns the number of key => value entries in the hash table
/// @param h hash table operated upon
/// @return the number of key => value entries in the hash table
int ioopm_hash_table_size(ioopm_hash_table_t *ht){
    int count = 0;

    for(int i = 0; i < No_Buckets; i++){
      entry_t *current = ht->buckets[i].next;

      while (current != NULL){
        count ++;
        current = current->next;
      }
    }
    return count;
}

/// @brief clear all the entries in a hash table
/// @param h hash table operated upon
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht){
    int amount = ioopm_hash_table_size(ht);

    if (amount == 0){
      return true;
    }
    else{
      return false;
    }
}

/// @brief clear all the entries in a hash table
/// @param h hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht){
    for(int i = 0; i < No_Buckets; i++) {
      entry_t *current = ht->buckets[i].next;
      while (current != NULL)
      {
        entry_t *temp = current;
        current = current->next;

        free(temp);
      }
      ht->buckets[i].next = NULL;
    }
}

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param h hash table operated upon
/// @return an array of keys for hash table h
int *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    int total_keys = ioopm_hash_table_size(ht);  // number of keys in the table
    int *keys = malloc(total_keys * sizeof(int));
    if (!keys) return NULL; // always check malloc

    int index = 0;

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next; // skip dummy
        while (current != NULL)
        {
            keys[index++] = current->key;
            current = current->next;
        }
    }

    return keys;
}

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param h hash table operated upon
/// @return an array of values for hash table h
char **ioopm_hash_table_values(ioopm_hash_table_t *ht){
    int total_keys = ioopm_hash_table_size(ht);  // number of keys in the table
    char **values = malloc(total_keys * sizeof(char));
    if (!values) return NULL; // always check malloc

    int index = 0;

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
