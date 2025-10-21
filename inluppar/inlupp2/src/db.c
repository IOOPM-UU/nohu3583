// db.c  -- patched version with consistent strdup-on-insert ownership policy

#include "db.h"
#include "linked_list.h"
#include "hash_table.h"
#include "sort.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Helper: insert a duplicated key into a hash table.
 * The function strdup's the key so the hash table owns its copy.
 */
static void hash_insert_dup(ioopm_hash_table_t *ht, const char *key, elem_t value)
{
    char *kdup = strdup(key);
    ioopm_hash_table_insert(ht, ptr_elem(kdup), value);
}

/* Helper: remove old_key from ht (frees its duplicate key if ht->should_free_keys)
 * and insert new_key (duplicated) with given value.
 * Must be done since the new name could be hashed into a new bucket. If we want to look in the real bucket using hash_str it wont work if we dont remove then add.
 */
static bool hash_rekey(ioopm_hash_table_t *ht, const char *old_key, const char *new_key, elem_t value)
{
    ioopm_hash_table_remove(ht, ptr_elem((char*)old_key)); // remove old duplicate key
    char *kdup = strdup(new_key);
    ioopm_hash_table_insert(ht, ptr_elem(kdup), value);
    return true;
}

/* Create / destroy database */
db_t *create_db(void) {
    db_t *db = calloc(1, sizeof(db_t));

    db->merch_ht = ioopm_hash_table_create(hash_str, str_eq); //assuming that we use str
    db->merch_ht->should_free_keys = true; //Sets as true since we use strdup to alloc

    db->shelf_ht = ioopm_hash_table_create(hash_str, str_eq); //assuming that we use str
    db->shelf_ht->should_free_keys = true; //Sets as true since we use strdup to alloc

    db->carts = ioopm_linked_list_create(NULL);
    db->next_cart_id = 1;

    return db;
}

/* Destroy one merch and its stocks; also remove corresponding shelf_ht entries.
 * NOTE: caller must ensure merch is removed from merch_ht (or that it's safe that merch->name is freed here).
 */
static void destroy_merch_and_shelves(db_t *db, merch_t *merch)
{
    if (!merch) return;

    // For every stock, remove shelf_ht entry (by string content) before freeing stock->shelf
    while (merch->locations->size > 0) {
        stock_t *stock = ioopm_linked_list_get(merch->locations, 0).p;
        // remove mapping from shelf_ht (this will free the duplicate key that was inserted into shelf_ht)
        ioopm_hash_table_remove(db->shelf_ht, ptr_elem(stock->shelf));
        // free the stock-owned copy
        free(stock->shelf);
        free(stock);
        ioopm_linked_list_remove(merch->locations, 0);
    }

    ioopm_linked_list_destroy(merch->locations);
    free(merch->name);
    free(merch->desc);
    free(merch);
}

void destroy_db(db_t *db) {
    if (!db) return;

    // Destroy all merch structs and their stocks (also remove shelf_ht mappings)
    elem_t *merch_keys = get_keys(db->merch_ht);
    size_t merch_count = ioopm_hash_table_size(db->merch_ht);
    for (size_t i = 0; i < merch_count; ++i) {
        option_t res = ioopm_hash_table_lookup(db->merch_ht, merch_keys[i]);
        if (res.success) {
            merch_t *m = res.value.p;
            destroy_merch_and_shelves(db, m);
        }
    }
    //Free the linked list for all keys
    free(merch_keys);

    // Now destroy the merch hash table (it will free its duplicated keys)
    ioopm_hash_table_destroy(db->merch_ht);

    // Now shelf_ht should be empty (we removed entries when destroying merch), but destroy anyway
    ioopm_hash_table_destroy(db->shelf_ht);

    // Destroy carts (cart->items keys are duplicated; cart->items destroy will free those keys)
    while (db->carts->size > 0) {
        cart_t *cart = ioopm_linked_list_get(db->carts, 0).p;
        ioopm_linked_list_remove(db->carts, 0);
        ioopm_hash_table_destroy(cart->items);
        free(cart);
    }
    //Destroy the cart linked list
    ioopm_linked_list_destroy(db->carts);
    //Lastly free the database
    free(db);
}

/* Helper to create a merch */
static merch_t *create_merch(char *name, char *desc, int price) {
    merch_t *merch = calloc(1, sizeof(merch_t));

    merch->name = strdup(name);  // merch struct owns its copy
    merch->desc = strdup(desc); //same as above
    merch->price = price;
    merch->locations = ioopm_linked_list_create(NULL); //A function is not needed now
    merch->total_stock = 0;
    merch->reserved = 0;

    return merch;
}

/* Add merchandise: create merch struct and insert into merch_ht.
 * We duplicate the name into merch struct and also duplicate key into hash via hash_insert_dup.
 */
bool add_merch(db_t *db, char *name, char *desc, int price) {
    if (ioopm_hash_table_has_key(db->merch_ht, ptr_elem(name))) {
        printf("Merch already exists\n"); //We don't do anything if the merch already exist
        return false;
    }

    merch_t *merch = create_merch(name, desc, price);
    if (!merch) return false; //If something is wrong don't do anything

    // Insert into merch_ht: duplicate key for hash ownership (separate from merch->name)
    hash_insert_dup(db->merch_ht, merch->name, ptr_elem(merch));
    return true;
}

/* Print merchandise names sorted, 20 at a time */
void print_merchandise(db_t *db) {
    ioopm_hash_table_t *ht = db->merch_ht;
    size_t size = ioopm_hash_table_size(ht);
    if (size == 0) {
        printf("(no merchandise)\n");
        return;
    }

    elem_t *arr = get_keys(ht);
    sort_keys(arr, size);

    size_t count = 0;
    while (count < size) {
        for (int i = 0; i < 20 && count < size; ++i, ++count) {
            printf("%s\n", (char *)arr[count].p);
        }
        if (count < size) {
            printf("Continue listing? (N/n to stop): "); //here we should use ask_question
            char input[16];
            if (!fgets(input, sizeof(input), stdin)) break; //remove this line when above is changed.
            if (input[0] == 'N' || input[0] == 'n') break;
        }
    }
    free(arr);
}

/* Remove merchandise: require confirmation; reject if any cart references the merch.
 * Removes merch from merch_ht (which frees the duplicate key) and destroys merch & shelves.
 */
bool remove_merch(db_t *db, char *name, char *conf_string) {
    if (!(conf_string[0] == 'Y' || conf_string[0] == 'y')) {
        printf("Wrong confirmation code\n");
        return false;
    }

    option_t res = ioopm_hash_table_lookup(db->merch_ht, ptr_elem(name));
    if (!res.success) {
        return false;
    }

    merch_t *merch = res.value.p;

    // Reject deletion if any cart contains this merch
    ioopm_link_t *c = db->carts->head;
    while (c) {
        cart_t *cart = c->element.p;
        option_t q = ioopm_hash_table_lookup(cart->items, ptr_elem(merch->name));
        if (q.success) {
            printf("Cannot delete: item present in one or more carts\n"); //either this or just remove the item from cart
            return false;                                                 //Right now blocks the change
        }
        c = c->next;
    }

    // Remove merch key from hash (frees the duplicated key in merch_ht)
    ioopm_hash_table_remove(db->merch_ht, ptr_elem(name));

    // Destroy merch and remove corresponding shelf entries
    destroy_merch_and_shelves(db, merch);

    return true;
}

/* Change merch (rename / change desc / price). Must rekey merch_ht and rekey all carts.
 * Returns true on success.
 */
bool change_merch(db_t *db, char *old_name, char *new_name, int new_price, char *new_desc, char *conf_string ) {
    if (!(conf_string[0] == 'Y' || conf_string[0] == 'y')) {
        printf("Wrong confirmation code\n");
        return false;
    }

    option_t res_old = ioopm_hash_table_lookup(db->merch_ht, ptr_elem(old_name));
    if (!res_old.success) {
        printf("Merchandise does not exist\n"); //checks if the merch does not exist
        return false;
    }

    // If new name differs, ensure it doesn't exist already
    if (strcmp(old_name, new_name) != 0) {
        option_t res_new = ioopm_hash_table_lookup(db->merch_ht, ptr_elem(new_name));
        if (res_new.success) {
            printf("Merchandise with this new name already exist\n"); //If the new name already exists do nothing.
            return false;
        }
    }

    merch_t *merch = res_old.value.p;

    // rekey merch_ht: remove old key and insert new duplicate under new_name
    ioopm_hash_table_remove(db->merch_ht, ptr_elem(old_name)); // frees old duplicate key
    // update merch struct strings
    free(merch->name);
    free(merch->desc);
    merch->name = strdup(new_name);
    merch->desc = strdup(new_desc);
    merch->price = new_price;
    // insert new duplicate key for merch_ht
    hash_insert_dup(db->merch_ht, merch->name, ptr_elem(merch));

    // Update carts: for each cart, if old_name present rekey entry to new_name
    ioopm_link_t *cur = db->carts->head;
    while (cur) {
        cart_t *cart = cur->element.p;
        option_t q = ioopm_hash_table_lookup(cart->items, ptr_elem(old_name));
        if (q.success) {
            int qty = q.value.i;
            // remove old key (frees its duplicate)
            ioopm_hash_table_remove(cart->items, ptr_elem(old_name));
            // insert new duplicated key
            hash_insert_dup(cart->items, merch->name, int_elem(qty));
        }
        cur = cur->next;
    }

    // Note: shelf_ht stores merch* as value so no rekeying needed there.
    return true;
}

/* Print stock for a merch (sorted). */
void print_stock(db_t *db, elem_t merch_name) {
    option_t res = ioopm_hash_table_lookup(db->merch_ht, merch_name);

    if (!res.success) {
        printf("Merchandise does not exist\n");
        return;
    }

    merch_t *merch = res.value.p;
    ioopm_list_t *locs = merch->locations;
    size_t size = locs->size;
    if (size == 0) {
        printf("(no stock)\n");
        return;
    }

    stock_t **stocks = calloc(size, sizeof(stock_t *));
    for (size_t i = 0; i < size; ++i) {
        stocks[i] = ioopm_linked_list_get(locs, i).p;
    }

    sort_stock(stocks, size);

    for (size_t i = 0; i < size; ++i) {
        printf("%s: %d\n", stocks[i]->shelf, stocks[i]->quantity);
    }

    free(stocks);
}

/* Create stock entry */
static stock_t *create_stock(const char *shelf, int no_items) {
    stock_t *stock = calloc(1, sizeof(stock_t));
    stock->shelf = strdup(shelf);   // stock owns its copy
    stock->quantity = no_items;
    return stock;
}

/* Replenish: add items to storage location (existing or new).
 * We ensure a shelf cannot contain different merch.
 * shelf_ht keys are duplicated here (hash_insert_dup) and values are merch*.
 */
bool replenish_stock(db_t *db, char *storage_loc, elem_t merch_name, int no_item)
{
    if (no_item < 1) {
        printf("No items to be added should be at least 1\n");
        return false;
    }

    option_t res = ioopm_hash_table_lookup(db->merch_ht, merch_name);
    if (!res.success) {
        printf("Merchandise does not exist\n");
        return false;
    }

    merch_t *merch = res.value.p;

    // If storage location already present in shelf_ht, make sure it belongs to this merch
    option_t shelf_lookup = ioopm_hash_table_lookup(db->shelf_ht, ptr_elem(storage_loc));
    if (shelf_lookup.success) {
        merch_t *existing_merch = shelf_lookup.value.p;
        if (existing_merch != merch) {
            printf("Storage location %s already stores a different merchandise\n", storage_loc);
            return false;
        }
        // else same merch: add quantity to existing stock entry
    }

    // find existing stock on merch->locations
    ioopm_link_t *current = merch->locations->head;
    while (current) {
        stock_t *stock = current->element.p;
        if (strcmp(stock->shelf, storage_loc) == 0) {
            stock->quantity += no_item;
            merch->total_stock += no_item;
            return true;
        }
        current = current->next;
    }

    // new stock — create and append
    stock_t *new_stock = create_stock(storage_loc, no_item);
    ioopm_linked_list_append(merch->locations, ptr_elem(new_stock));
    merch->total_stock += no_item;

    // insert duplicate key for shelf_ht (hash owns key) and store merch* as value
    hash_insert_dup(db->shelf_ht, new_stock->shelf, ptr_elem(merch));

    return true;
}

/* Create Cart: cart->items must duplicate keys for the strings inserted into it */
cart_t *create_cart(db_t *db) {
    cart_t *cart = calloc(1, sizeof(cart_t));
    cart->items = ioopm_hash_table_create(hash_str, str_eq);
    cart->items->should_free_keys = true;
    cart->id = db->next_cart_id++;
    ioopm_linked_list_append(db->carts, ptr_elem(cart));
    return cart;
}

/* Helper: find cart index by id */
static int cart_index(db_t *db, int cart_id) {
    ioopm_list_t *list = db->carts;
    ioopm_link_t *current = list->head;
    int index = 0;

    while (current != NULL) {
        cart_t *cart = current->element.p;
        if (cart->id == cart_id) {
            return index;
        }
        current = current->next;
        index++;
    }

    return -1;
}

/* Remove cart */
bool remove_cart(db_t *db, int cart_id, char *conf_string) {
    if (!(conf_string[0] == 'Y' || conf_string[0] == 'y')) {
        printf("Wrong confirmation code\n");
        return false;
    }

    int index = cart_index(db, cart_id);
    if (index == -1) {
        printf("The requested cart does not exist\n");
        return false;
    }

    elem_t removed = ioopm_linked_list_remove(db->carts, index);
    cart_t *cart = removed.p;

    // When destroying cart->items the hash will free duplicated keys
    ioopm_hash_table_destroy(cart->items);
    free(cart);

    return true;
}

/* Add to cart: checks reservation invariants and updates merch->reserved.
 * We use strdup-on-insert for cart->items keys: remove old key before insert to avoid leaking duplicate keys.
 */
bool add_to_cart(db_t *db, merch_t *merch, int amnt, cart_t *cart)
{
    if (amnt <= 0) {
        printf("Amount must be positive\n");
        return false;
    }

    // available extra we can reserve (excluding this cart's current reservation)
    int available = merch->total_stock - merch->reserved; // how much additional can be reserved system-wide

    option_t res = ioopm_hash_table_lookup(cart->items, ptr_elem(merch->name));
    if (!res.success) {
        if (amnt > available) {
            printf("The wanted quantity is more than quantity in stock\n");
            return false;
        }
        // insert duplicated key for cart's hash
        hash_insert_dup(cart->items, merch->name, int_elem(amnt));
        merch->reserved += amnt;
        return true;
    } else {
        int cart_amnt = res.value.i;
        // extra to add is 'amnt'
        if (amnt > available) {
            printf("Not enough stock for requested quantity.\n");
            return false;
        }
        // remove old key (frees the old duplicate)
        ioopm_hash_table_remove(cart->items, ptr_elem(merch->name));
        // insert new key with updated amount
        hash_insert_dup(cart->items, merch->name, int_elem(cart_amnt + amnt));
        merch->reserved += amnt;
        return true;
    }
}

/* Remove from cart: update reserved and remove/replace key accordingly */
bool remove_from_cart(db_t *db, merch_t *merch, int amnt, int cart_id)
{
    if (amnt < 0) {
        printf("Amount must be non-negative\n");
        return false;
    }

    int index = cart_index(db, cart_id);
    if (index == -1) {
        printf("The requested cart does not exist\n");
        return false;
    }

    cart_t *cart = ioopm_linked_list_get(db->carts, index).p;
    option_t res = ioopm_hash_table_lookup(cart->items, ptr_elem(merch->name));

    if (!res.success) {
        printf("Merchandise not in cart\n");
        return false;
    }

    int cart_amnt = res.value.i;

    if (amnt >= cart_amnt) {
        // remove the entry (frees duplicated key)
        ioopm_hash_table_remove(cart->items, ptr_elem(merch->name));
        merch->reserved -= cart_amnt;
    } else {
        // replace with decreased count
        ioopm_hash_table_remove(cart->items, ptr_elem(merch->name));
        hash_insert_dup(cart->items, merch->name, int_elem(cart_amnt - amnt));
        merch->reserved -= amnt;
    }

    return true;
}

/* Calculate cost: iterate cart keys via get_keys() */
int calculate_cost(db_t *db, int cart_id) {
    int index = cart_index(db, cart_id);
    if (index == -1) {
        printf("Cart does not exist\n");
        return -1;
    }

    int sum = 0;
    cart_t *cart = ioopm_linked_list_get(db->carts, index).p;
    ioopm_hash_table_t *items = cart->items;

    size_t n = ioopm_hash_table_size(items);
    if (n == 0) return 0;

    elem_t *keys = get_keys(items);
    for (size_t i = 0; i < n; ++i) {
        option_t q = ioopm_hash_table_lookup(items, keys[i]);
        int qty = q.value.i;
        option_t res = ioopm_hash_table_lookup(db->merch_ht, keys[i]);
        if (res.success) {
            merch_t *merch = res.value.p;
            sum += merch->price * qty;
        } else {
            // If merch no longer exists, ignore or handle error
            printf("Warning: merchandise %s in cart no longer exists\n", (char *)keys[i].p);
        }
    }
    free(keys);
    return sum;
}

/* Checkout: validate availability, remove quantity from shelves deterministically (left-to-right),
 * remove shelf_ht entries when shelf becomes empty, update total_stock and reserved, remove cart.
 */
bool checkout_cart(db_t *db, int cart_id) {
    int index = cart_index(db, cart_id);
    if (index == -1) {
        printf("Cart does not exist\n");
        return false;
    }

    cart_t *cart = ioopm_linked_list_get(db->carts, index).p;
    ioopm_hash_table_t *items = cart->items;

    size_t n = ioopm_hash_table_size(items);
    if (n == 0) {
        // empty cart: simply remove it
        ioopm_linked_list_remove(db->carts, index);
        ioopm_hash_table_destroy(cart->items);
        free(cart);
        return true;
    }

    elem_t *keys = get_keys(items);

    // Validation pass: ensure all quantities available
    for (size_t i = 0; i < n; ++i) {
        option_t q = ioopm_hash_table_lookup(items, keys[i]);
        int qty = q.value.i;
        option_t res = ioopm_hash_table_lookup(db->merch_ht, keys[i]);
        if (!res.success) {
            free(keys);
            printf("Merchandise %s no longer exists\n", (char*)keys[i].p);
            return false;
        }
        merch_t *merch = res.value.p;
        if (qty > merch->total_stock) {
            free(keys);
            printf("Not enough stock for %s\n", merch->name);
            return false;
        }
    }

    // Apply pass: remove stock from shelves and update totals/reserved
    for (size_t i = 0; i < n; ++i) {
        option_t q = ioopm_hash_table_lookup(items, keys[i]);
        int qty = q.value.i;
        merch_t *merch = ioopm_hash_table_lookup(db->merch_ht, keys[i]).value.p;

        int remaining = qty;
        // iterate over locations, removing or decrementing stock
        size_t j = 0;
        while (j < merch->locations->size && remaining > 0) {
            stock_t *stock = ioopm_linked_list_get(merch->locations, j).p;
            if (stock->quantity > remaining) {
                stock->quantity -= remaining;
                remaining = 0;
                j++;
            } else {
                // Use up this shelf completely: remove shelf_ht mapping first, then free
                remaining -= stock->quantity;
                ioopm_hash_table_remove(db->shelf_ht, ptr_elem(stock->shelf)); // frees shelf_ht key duplicate
                free(stock->shelf);
                free(stock);
                ioopm_linked_list_remove(merch->locations, j); // don't increment j
            }
        }

        if (remaining != 0) {
            // inconsistent state (shouldn't happen after validation)
            free(keys);
            printf("Checkout failed: inconsistent stock for %s\n", merch->name);
            return false;
        }

        merch->total_stock -= qty;
        merch->reserved -= qty;
    }

    free(keys);

    // remove cart from db and free it
    ioopm_linked_list_remove(db->carts, index);
    ioopm_hash_table_destroy(cart->items);
    free(cart);

    return true;
}
