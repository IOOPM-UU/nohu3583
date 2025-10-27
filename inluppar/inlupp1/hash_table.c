    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include <assert.h>
    #include <ctype.h>
    #include "linked_list.h"
    #include "common.h"
    #include "hash_table.h"

    #define No_Buckets 5  


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
        ht->size = 0; 
        ht->should_free_keys = false;
        return ht;
    }

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    if (!ht) return;
    
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL)
        {
            entry_t *next = current->next;
            
            // Only free keys if the flag is set
            if (ht->should_free_keys && current->key.p != NULL) {
                free(current->key.p);
            }
            
            free(current);
            current = next;
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
        ht->size++;
    }


    // Inserts a new value into the hashtable
    void ioopm_hash_table_insert_freq(ioopm_hash_table_t *ht, elem_t key)
    {
        int bucket = ht->func(key);
        entry_t *current = ht->buckets[bucket].next; // skip dummy head

        while (current != NULL)
        {
            if (ht->eq_func(current->key, key))
            {
                // Key exists → increment frequency
                current->value.i++;
                free(key.p);  // only if key.p was dynamically allocated!
                return;
            }
            current = current->next;
        }

        // Key not found → insert new entry with frequency 1
        entry_input(ht, key, int_elem(1));
        ht->size++;
    }
    //Om ett värde finns lägg till ett på valuet, annars sätt value till 0, iterera genom ht till vi kommer till slutet.

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

    #define SuccessElem(e)   (option_t){ .success = true,  .value = (e) }
    #define SuccessInt(i)    (option_t){ .success = true,  .value = (elem_t){ .i = (i) } }
    #define SuccessPtr(p)    (option_t){ .success = true,  .value = (elem_t){ .p = (p) } }
    #define Failure()        (option_t){ .success = false, .value = (elem_t){ .p = NULL } }
    #define Successful(o)    ((o).success == true)
    #define Unsuccessful(o)  ((o).success == false)


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
                
                // FREE THE KEY if should_free_keys is set
                if (ht->should_free_keys && target->key.p != NULL) {
                    free(target->key.p);
                }
                
                free(target);
                ht->size--;
            }
        }
    }
    /// @brief returns the number of key => value entries in the hash table
    size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
    {
        return ht->size;
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
                
                // FREE THE KEY if should_free_keys is set
                if (ht->should_free_keys && tmp->key.p != NULL) {
                    free(tmp->key.p);
                }
                
                free(tmp);
            }
            ht->buckets[i].next = NULL; // reset bucket
        }
        ht->size = 0;
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
    // More efficient: direct lookup without creating key list
    int bucket = ht->func(key);
    entry_t *current = ht->buckets[bucket].next;
    
    while (current != NULL) {
        if (ht->eq_func(current->key, key)) {
            return true;
        }
        current = current->next;
    }
    return false;
}


    /// @brief check if a hash table has an entry with a given value
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
    // More efficient: iterate directly through buckets
    for (int i = 0; i < No_Buckets; i++) {
        entry_t *current = ht->buckets[i].next;
        while (current != NULL) {
            if (ht->eq_func(current->value, value)) {
                return true;
            }
            current = current->next;
        }
    }
    return false;
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
