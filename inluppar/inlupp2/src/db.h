#ifndef DB_H
#define DB_H

#include "linked_list.h"
#include "hash_table.h"
#include <stdbool.h>

typedef struct stock {
    char *shelf;      
    int quantity;
} stock_t; 

typedef struct merch {
    char *name;       
    char *desc;       
    int price;
    ioopm_list_t *locations;     // list of stock_t* (for ordered iteration)
    ioopm_hash_table_t *shelf_map; // NEW: shelf -> stock_t* (for O(1) lookups)
    int total_stock;
    int reserved;
} merch_t;

typedef struct cart {
    int id;
    ioopm_hash_table_t *items;  // name -> quantity
} cart_t;

typedef struct db {
    ioopm_hash_table_t *merch_ht;  // name -> merch_t*
    ioopm_hash_table_t *shelf_ht;  // shelf -> merch_t*
    ioopm_list_t *carts;           // list of cart_t*
    int next_cart_id;
} db_t;

// Function declarations remain the same...
db_t *create_db(void);
void destroy_db(db_t *db);
bool add_merch(db_t *db, char *name, char *desc, int price);
void print_merchandise(db_t *db);
bool remove_merch(db_t *db, char *name, char *conf_string);
bool change_merch(db_t *db, char *old_name, char *new_name, int new_price, char *new_desc, char *conf_string);
void print_stock(db_t *db, elem_t merch_name);
bool replenish_stock(db_t *db, char *storage_loc, elem_t merch_name, int no_item);
cart_t *create_cart(db_t *db);
bool remove_cart(db_t *db, int cart_id, char *conf_string);
bool add_to_cart(db_t *db, merch_t *merch, int amnt, cart_t *cart);
bool remove_from_cart(db_t *db, merch_t *merch, int amnt, int cart_id);
int calculate_cost(db_t *db, int cart_id);
bool checkout_cart(db_t *db, int cart_id);

// // Add to function declarations
// void populate_shelf_map(merch_t *merch);

#endif