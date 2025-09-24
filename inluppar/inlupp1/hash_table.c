#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct entry entry_t;

struct entry
{
    int key;        // holds the key
    char *value;    // holds the value
    entry_t *next;  // points to the next entry (possibly NULL)
};

typedef struct hash_table
{
    entry_t buckets[17]; // dummy heads for each bucket
} ioopm_hash_table_t;

/// Forward declarations 
unsigned long hash_int(int key);
ioopm_hash_table_t *ioopm_hash_table_create();
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);
void entry_input(ioopm_hash_table_t *ht, int key, char *value);
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value);
entry_t* find_previous_entry_for_key(entry_t *bucket_head, int key);

/// Implementation-

unsigned long hash_int(int key)
{
    return key % 17; // fixed number of buckets
}

ioopm_hash_table_t *ioopm_hash_table_create()
{
    ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));

    // Initialize dummy heads (already zeroed by calloc)
    for (int i = 0; i < 17; i++)
    {
        ht->buckets[i].key = 0;       // dummy key
        ht->buckets[i].value = NULL;  // dummy value
        ht->buckets[i].next = NULL;   // start empty
    }

    // Example entries for demonstration (like Figure 5)
    entry_input(ht, 1, "foo");
    entry_input(ht, 2, "bar");
    entry_input(ht, 3, "baz");
    entry_input(ht, 4, "previous_of_208");
    entry_input(ht, 208, "some_value");

    return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    for (int i = 0; i < 17; i++)
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

void entry_input(ioopm_hash_table_t *ht, int key, char *value)
{
    int bucket = hash_int(key);

    entry_t *new_entry = malloc(sizeof(entry_t));
    new_entry->key = key;
    new_entry->value = value;

    // Attach after dummy head
    new_entry->next = ht->buckets[bucket].next;
    ht->buckets[bucket].next = new_entry;
}

// void entry_create(int key, char* value, entry_t* next) {
//       entry_t new_entry;
//       new_entry.key = key;
//       new_entry.value = value;
//       new_entry.next = next;
// }


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

/// Find the previous entry for a given key in a bucket
entry_t* find_previous_entry_for_key(entry_t *bucket_head, int key)
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

int ioopm_hash_table_has_key(ioopm_hash_table_t *ht, int key)
{
    int bucket = hash_int(key);
    entry_t *current = ht->buckets[bucket].next;

    while (current != NULL)
    {
        if (current->key == key){
        return 1;
        current = current->next;
        }
    }
    return 0;
}

char *ioopm_hash_table_get(ioopm_hash_table_t *ht, int key)
{
    int bucket = hash_int(key);
    entry_t *current = ht->buckets[bucket].next;

    while (current != NULL)
    {
        if (current->key == key){
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

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key)
{
    entry_t *prev = find_previous_entry_for_key(&ht->buckets[key % 17], key);

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
// 1. Find the previous entry to the one we would like to remove
//    1.1 If such an entry cannot be found, do nothing
//    1.2 Otherwise, unlink the entry by updating the previous
//        entry's next field so that it "skips over the entry" and
//        return the memory that the entry reserved to the system so
//        that it can be reused

void ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key)
{
    entry_t *prev = find_previous_entry_for_key(&ht->buckets[key % 17], key);

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

/// ---------------------- Test Main
// int main(void)
// {
//     ioopm_hash_table_t *ht = ioopm_hash_table_create();

//     // Print all entries in bucket 4 (skip dummy)
//     printf("Bucket 4 contents:\n");
//     entry_t *current = ht->buckets[4].next;
//     while (current != NULL)
//     {
//         printf("Key: %d, Value: %s\n", current->key, current->value);
//         current = current->next;
//     }

//     ioopm_hash_table_destroy(ht);
//     return 0;
// }
