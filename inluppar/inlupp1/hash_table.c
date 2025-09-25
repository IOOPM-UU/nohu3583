#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

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

    return ht;
}

//Destroys/removes the hashtable, by freeing it
// Destroys the hashtable and frees all entries + their values
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
            free(current->value);
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Key not found → create new entry
    entry_input(ht, key, value);
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
            free(target->value);    
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

/// @brief checks if the hash table is empty
/// @param h hash table operated upon
/// @return true is size == 0, else false
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht){
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


/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param h hash table operated upon
/// @return an array of keys for hash table h
int *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    int total_keys = ioopm_hash_table_size(ht);  // number of keys in the table
    int *keys = malloc(sizeof(int) * total_keys);
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
    char **values = malloc(sizeof(char *) * total_keys);

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

/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, int key){
  int *keys = ioopm_hash_table_keys(ht);
  int n_keys = ioopm_hash_table_size(ht);
  
  if (n_keys == 0) return false;

  for(int i = 0; i < n_keys; i++){
      if(keys[i] == key){
        free(keys);
        return true;
      }
  }
  free(keys);
  return false;
}

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, char *value)
{
    int n_keys = ioopm_hash_table_size(ht);
    char **values = ioopm_hash_table_values(ht);

    if (!values || n_keys == 0) return false;

    for (int i = 0; i < n_keys; i++)
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

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg){
    for (int i = 0; i < No_Buckets; i++) {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL) {
            if (!pred(current->key, current->value, arg)) {
                return false; 
            }
            current = current->next; 
        }
    }
    return true;
}


/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg){
    for (int i = 0; i < No_Buckets; i++) {
        entry_t *current = ht->buckets[i].next; 
        while (current != NULL) {
            if (pred(current->key, current->value, arg)) {
                return true; 
            }
            current = current->next;
    }
  }
    return false; 
}

static bool key_equiv(int key, char *value_ignored, void *x)
{
    (void)value_ignored; // suppress unused warning
    int *other_key_ptr = x;
    return key == *other_key_ptr;
}

static bool val_equiv(int key_ignored, char *value, void *x)
{
    (void)key_ignored; // suppress unused warning
    char *other_value = x;
    return strcmp(value, other_value) == 0;
}


/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
bool ioopm_hash_table_has_key_new(ioopm_hash_table_t *ht, int key)
{
  return ioopm_hash_table_any(ht, key_equiv, &key);
}

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
bool ioopm_hash_table_has_value_new(ioopm_hash_table_t *ht, char *value)
{
  return ioopm_hash_table_any(ht, val_equiv, &value);
}

//functions and predicate for testing new functions

static bool is_uppercase(int key, char *value, void *x)
{
    (void)key;   // unused
    (void)x; // unused

    for (char *p = value; p && *p; p++) {
        if (!isupper((unsigned char)*p)) {
            return false;
        }
    }
    return true;
}

static bool is_lowercase(int key, char *value, void *x)
{
    (void)key;   
    (void)x; 

    for (char *p = value; p && *p; p++) {
        if (isupper((unsigned char)*p)) {
            return false;
        }
    }
    return true;
}

static void to_upper(int key, char **value, void *x){
    (void)key; 
    (void)x;

    for(char *p = *value; p && *p; p++){
        *p = (char) toupper((unsigned char) *p);
    }
}

static void to_lower(int key, char **value, void *x){
    (void)key; 
    (void)x;

    for(char *p = *value; p && *p; p++){
        *p = (char) tolower((unsigned char) *p);
    }
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
// int main(void) {
//     ioopm_hash_table_t *ht = ioopm_hash_table_create();

//     ioopm_hash_table_insert(ht, 8, "b");
//     ioopm_hash_table_insert(ht, 214, "s");

//     int n_keys = ioopm_hash_table_size(ht);
//     int *array = ioopm_hash_table_keys(ht);

//     for (int i = 0; i < n_keys; i++) {
//         printf("%d\n", array[i]);  // print each key
//     }

//     free(array); // don't forget to free the allocated keys array
//     ioopm_hash_table_destroy(ht);
//     return 0;
// }