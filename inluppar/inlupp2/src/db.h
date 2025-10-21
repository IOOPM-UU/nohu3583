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

// typedef enum {
//     ACTION_ADD_MERCH,
//     ACTION_REMOVE_MERCH,
//     ACTION_EDIT_MERCH,
//     ACTION_REPLENISH,
//     ACTION_ADD_TO_CART,
//     ACTION_REMOVE_FROM_CART,
//     ACTION_CHECKOUT_CART,
//     // ... add more as needed
// } action_type_t;

// typedef struct action {
//     action_type_t type;
//     // Store enough info to undo the action:
//     elem_t merch_name;   
//     merch_t *old_merch; // for remove/edit
//     merch_t *new_merch; // for add/edit
//     elem_t storage_loc;  //storage location
//     int old_total_stock;   // for replenish, cart changes
//     int new_total_stock;    // new total stock for merch
//     int old_shelf_quantity; // old total stock for merch
//     bool shelf_new;         //true if the shelf is new.
//     int cart_id;            //cart_id
//     int index;              //index
//     cart_t *cart;           //cart_t
//     // ... add fields as needed
// } action_t;


/// Represents the entire store database
typedef struct db {
    ioopm_hash_table_t *merch_ht;  // name -> merch_t*
    ioopm_hash_table_t *shelf_ht;  // shelf -> merch name (str_elem->str_elem)
    ioopm_list_t *carts;           // list of cart_t*
    int next_cart_id;              // auto-increment ID
} db_t;


typedef struct entry entry_t;

