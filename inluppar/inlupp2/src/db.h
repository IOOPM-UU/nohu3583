#pragma once
#include "hash_table.h"
#include "linked_list.h"


/// Represents a stock item on a specific shelf
typedef struct stock {
    char *shelf;      
    int quantity;
} stock_t; 

typedef struct merch {
    char *name;       
    char *desc;       
    int price;
    ioopm_list_t *locations; // list of stock_t*
    int total_stock;
    int reserved;
} merch_t;

typedef struct cart {
    int id;
    ioopm_hash_table_t *items;  // name -> quantity
} cart_t;


/// Represents the entire store database
typedef struct db {
    ioopm_hash_table_t *merch_ht;  // name -> merch_t*
    ioopm_hash_table_t *shelf_ht;  // shelf -> merch name (str_elem->str_elem)
    ioopm_list_t *carts;           // list of cart_t*
    int next_cart_id;              // auto-increment ID
} db_t;


typedef struct entry entry_t;





/* Create / destroy database */
db_t *create_db(void);


void destroy_db(db_t *db);

/* Add merchandise: create merch struct and insert into merch_ht.
 * We duplicate the name into merch struct and also duplicate key into hash via hash_insert_dup.
 */
bool add_merch(db_t *db, char *name, char *desc, int price);

/* Print merchandise names sorted, 20 at a time */
void print_merchandise(db_t *db);

/* Remove merchandise: require confirmation; reject if any cart references the merch.
 * Removes merch from merch_ht (which frees the duplicate key) and destroys merch & shelves.
 */
bool remove_merch(db_t *db, char *name, char *conf_string);

/* Change merch (rename / change desc / price). Must rekey merch_ht and rekey all carts.
 * Returns true on success.
 */
bool change_merch(db_t *db, char *old_name, char *new_name, int new_price, char *new_desc, char *conf_string );

/* Print stock for a merch (sorted). */
void print_stock(db_t *db, elem_t merch_name);

/* Replenish: add items to storage location (existing or new).
 * We ensure a shelf cannot contain different merch.
 * shelf_ht keys are duplicated here (hash_insert_dup) and values are merch*.
 */
bool replenish_stock(db_t *db, char *storage_loc, elem_t merch_name, int no_item);

/* Create Cart: cart->items must duplicate keys for the strings inserted into it */
cart_t *create_cart(db_t *db);

/* Remove cart */
bool remove_cart(db_t *db, int cart_id, char *conf_string);

/* Add to cart: checks reservation invariants and updates merch->reserved.
 * We use strdup-on-insert for cart->items keys: remove old key before insert to avoid leaking duplicate keys.
 */
bool add_to_cart(db_t *db, merch_t *merch, int amnt, cart_t *cart);

/* Remove from cart: update reserved and remove/replace key accordingly */
bool remove_from_cart(db_t *db, merch_t *merch, int amnt, int cart_id);

/* Calculate cost: iterate cart keys via get_keys() */
int calculate_cost(db_t *db, int cart_id);

/* Checkout: validate availability, remove quantity from shelves deterministically (left-to-right),
 * remove shelf_ht entries when shelf becomes empty, update total_stock and reserved, remove cart.
 */
bool checkout_cart(db_t *db, int cart_id);