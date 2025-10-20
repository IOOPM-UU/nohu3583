#include "db.h"
#include "linked_list.h"
#include "hash_table.h"
#include "sort.h"


db_t *create_db (void){
    db_t *db = calloc(1, sizeof(db_t));

    db->merch_ht = ioopm_hash_table_create(hash_str, str_eq);
    db->shelf_ht = ioopm_hash_table_create(hash_str, str_eq);
    db->carts = ioopm_linked_list_create(NULL);
    db->next_cart_id = 1;
    // db->undo_stack = ioopm_linked_list_create(NULL);
    return db;
}

void destroy_db(db_t *db){

    // TODO: free all merch, shelves, and carts properly later
    //merch destroy
    ioopm_hash_table_destroy(db->merch_ht);
    ioopm_hash_table_destroy(db->shelf_ht); 
    ioopm_linked_list_destroy(db->carts);
    free(db);
}


// 2.1.1 Add Merchandise
// This adds a new merch to the warehouse with a name (string), description (string), and price (integer). 
// A newly added merch is not in stock.
// Adding a new merch with the same name as an existing merch is not allowed.
// The action code shall be "A". The action shall read the name, description and price for the merch.

static merch_t *create_merch(elem_t name, elem_t desc, int price){
    merch_t *merch = calloc(1, sizeof(merch_t));

    merch->name = name;
    merch->desc = desc;
    merch->price = price;
    merch->locations = ioopm_linked_list_create(NULL);
    return merch;
}

bool add_merch(db_t *db, elem_t name, elem_t desc, int price){
    if(ioopm_hash_table_has_key(db->merch_ht, name)){
        printf("Merch already exists");
        return false; //false did not add, ie the item was already in the database.
    }

    merch_t *merch = create_merch(name, desc, price);

    if (!merch){
        return false;
    }
    ioopm_hash_table_insert(db->merch_ht, name, ptr_elem(merch));
    // action_t *action = calloc(1, sizeof(action_t));
    // action->type = ACTION_ADD_MERCH;
    // action->merch_name = name;
    // ioopm_linked_list_append(db->undo_stack, ptr_elem(action));
    return true;
}

// 2.1.2 List Merchandise
// This should list all items in the store. Items should preferably (soft requirement) be printed in alphabetical order on their names. Because there may be more things in the database than might fit on a screen, items should be printed 20 at a time, and the user is asked to continue listing (if possible) or return to the main menu.
// The action code shall be "L". The question to continue listing shall return to the main menu if the input begins with "N" or "n". For all other input (including an empty line) the listing shall continue.

void print_merchandise(db_t *db){
    ioopm_hash_table_t *ht = db->merch_ht;
    elem_t *arr = get_keys(ht);
    size_t size = ht->size; // Number of merchandise items

    sort_keys(arr, size); // Sort all keys

    size_t count = 0;
    while (count < size) {
        // Print up to 20 items at a time
        for (int i = 0; i < 20 && count < size; ++i, ++count) {
            printf("%s\n", (char *)arr[count].p);
        }
        if (count < size) {
            printf("Continue listing? (N/n to stop): ");
            char input[10];
            fgets(input, sizeof(input), stdin);
            if (input[0] == 'N' || input[0] == 'n')
                break;
        }
    }
    free(arr);
}


// 2.1.3 Remove Merchandise
// Removes an item completely from the warehouse, including all its stock.
// The action code shall be "D". The action shall read the name of the merch and a confirmation string. The merch shall be removed only if the confirmation string begins with "Y" or "y".


static void destroy_merch(merch_t *merch){
    if(!merch){
        return;
    }
    size_t size = merch->locations->size;


    for(size_t i = 0; i < size; i++){
        stock_t *stock = ioopm_linked_list_get(merch->locations, i).p;
        free(stock->shelf.p);
        free(stock);
    }
    ioopm_linked_list_destroy(merch->locations);
    free(merch->name.p);
    free(merch->desc.p);
    free(merch);
}

bool remove_merch(db_t *db, elem_t name){
    option_t res = ioopm_hash_table_lookup(db->merch_ht, name);

    if(!res.success){
        return false;
    }

    merch_t *merch = res.value.p; 

    // action_t *action = calloc(1, sizeof(action_t));
    // action->old_merch = merch;
    // action->type = ACTION_REMOVE_MERCH;
    // ioopm_linked_list_append(db->undo_stack, ptr_elem(action));

    destroy_merch(merch);
    ioopm_hash_table_remove(db->merch_ht, name);
    return true;
}


// 2.1.4 Edit Merchandise
// Allows changing the name, description and price of a merch. Note that this does not affect its stock.
// Changing the name of a merch to the name of an existing merch is not allowed..
// Note that changing the name may mean changing the key unless you use a unique id for each merch.
// The action code shall be "E". The action shall read the old name of the merch, the new name, the new description, the new price for the merch and a confirmation string. The merch shall be updated only if the confirmation string begins with "Y" or "y". 

bool change_merch(db_t *db, elem_t name){
    option_t res = ioopm_hash_table_lookup(db->merch_ht, name);

    if(!res.success){
        printf("Merchandise does not exist");
        return false;
    }

    merch_t *merch = res.value.p;
    
    //what attribute should be changed. 
    // action_t *action = calloc(1, sizeof(action_t));
    // action->old_merch = merch;
    // action->type = ACTION_EDIT_MERCH;
    // ioopm_linked_list_append(db->undo_stack, ptr_elem(action));
    //before changing save old merch_t

}


// 2.1.5 Show Stock
// List all the storage locations for a particular merch, along with the quantities stored on each location. Storage locations should preferably be listed in alphabetical order (e.g., A20 before B01 and C01 before C10).
// Names of storage locations follow this format always: one capital letter (A-Z) followed by two digits (0-9).
// The action code shall be "S. The action shall read the name of the merch.

void print_stock(db_t *db, elem_t merch_name){
    option_t res = ioopm_hash_table_lookup(db->merch_ht, merch_name);

    if(!res.success){
        printf("Merchandise does not exist");
        return;
    }

    merch_t *merch = res.value.p;
    ioopm_list_t *locs = merch->locations;
    size_t size = locs->size;
    stock_t **stocks = calloc(size, sizeof(stock_t *));
    for (size_t i = 0; i < size; ++i) {
        stocks[i] = ioopm_linked_list_get(locs, i).p;

    }
    sort_stock(stocks, size);

    // Print sorted shelves and their quantities
    for (size_t i = 0; i < size; ++i) {
        printf("%s: %d\n", (char *)stocks[i]->shelf.p, stocks[i]->quantity);
    }

    free(stocks);
}

// 2.1.6 Replenish
// Increases the stock of a merch by at least one.
// You can replenish on an existing storage location or a new one.
// The stock for a merch is the sum of all items on all storage locations holding that merch.
// A storage location stocks items of one (type of) merch, never more.
// For simplicity, there is no limit to the amount of storage locations nor is there a limit on the number of items a location can hold.
// The action code shall be "P". The action shall read the id of the storage location, the name of the merch and the number of items to add.

stock_t *create_stock(elem_t shelf, int no_items){
    stock_t *stock = calloc(1, sizeof(stock_t));
    stock->shelf = shelf;
    stock->quantity = no_items;
    return stock;
}

bool replenish_stock(db_t *db, elem_t storage_loc, elem_t merch_name, int no_item){
    if(no_item < 1){
        printf("No items to be added should be more than 1");
        return false;
    }
    option_t res = ioopm_hash_table_lookup(db->merch_ht, merch_name);

    if(!res.success){
        printf("Merchandise does not exist"); 
        return false;
    }
    merch_t *merch = res.value.p; 
    ioopm_link_t *current = merch->locations->head;
    // action_t *action = calloc(1, sizeof(action_t));
    // action->type = ACTION_REPLENISH;
    // action->old_total_stock = merch->total_stock;

    while(current != NULL){
        stock_t *stock = current->element.p; // <-- Correct way to get stock_t*
        if(merch->locations->func(stock->shelf, storage_loc)){
            stock->quantity += no_item;
            return true;
        }
        current = current->next;
    }
    stock_t *new_stock = create_stock(storage_loc, no_item);
    ioopm_linked_list_append(merch->locations, ptr_elem(new_stock));
    merch->total_stock += no_item;
    return true;
}


// 2.1.7 Create Cart
// Creates a new shopping cart in the system which is empty.
// A shopping cart represents a possible order.
// Adding/removing merch to/from a cart does not change the stock for that merch – stocks are changed only during checkout.
// Shopping carts are identified by a monotonically increasing number, i.e., the number of the i’th shopping cart created is i, regardless of how many shopping carts have been removed.
// The action code shall be "C".
cart_t *create_cart(){
    cart_t *cart = calloc(1, sizeof(cart_t));
    cart->items = ioopm_hash_table_create(hash_str, str_eq);
    return cart;
}

//helper function that check if cart_id exist in list of carts.
static int cart_index(db_t *db, int cart_id){ 
    //should iterate over the linked list and find the item that needs to be removed
    ioopm_list_t *list = db->carts;
    ioopm_link_t *current = list->head;
    int index = 0;
    while(current != NULL){
        if(list->func(int_elem(cart_id), current->element)){
            return index;
        }
        current = current->next;
        index++;
    }
    return -1;

}

// 2.1.8 Remove Cart
// Removes a shopping cart from the system.
// The action code shall be "R". The action shall read the id of the cart and a confirmation string. The cart shall be removed only if the confirmation string begins with "Y" or "y".
bool remove_cart(db_t *db, int cart_id) {
    int index = cart_index(db, cart_id);

    if(index == -1){
        printf("The requested stock does not exist");
        return false;
    }
    elem_t removed = ioopm_linked_list_remove(db->carts, index);
    cart_t *cart = removed.p;
    ioopm_hash_table_destroy(cart->items);
    free(cart);
    return true;
}

// 2.1.9 Add To Cart
// Adds some quantity of a merch to a specific shopping cart.
// All possible orders in the system must be fullfillable. For example, we may only have one or more carts with 12 items of a merch X if the total stock of X in the system at least 12. Thus, if all users go to checkout at the same time, they should all succeed.
// The action code shall be "+". The action shall read the id of the cart, the name of the merch and the number of items.
bool add_to_cart(db_t *db, merch_t *merch, int amnt, cart_t *cart){
    option_t res = ioopm_hash_table_lookup(cart->items, merch->name);

    if(!res.success){
        //the element does not exist in cart. should be added if the list.size if the amount is < stock.
        if(merch->total_stock < amnt){
            printf("The wanted quantity is less than quantity in stock");
            return false;
        }
        else{
            ioopm_hash_table_insert(cart->items, merch->name, int_elem(amnt)); //insert into the hash_table
            return true;
        }
    }
    else{
    int cart_amnt = res.value.i;

    if(cart_amnt + amnt > merch->total_stock){
        printf("Not enough stock for requested quantity.\n");
        return false;
    }
    ioopm_hash_table_insert(cart->items, merch->name, int_elem(cart_amnt + amnt));
    return true;
    }
}


// 2.1.10 Remove From Cart
// Removes zero or more items of some merch from a particular cart.
// The action code shall be "-". The action shall read the id of the cart, the name of the merch and the number of items.
bool remove_from_cart(db_t *db, merch_t *merch, int amnt, int cart_id){
    //firtly check if the cart even exist.
    int index = cart_index(db, cart_id);

    if(index == -1){
        printf("The requested merch is not in the cart");
        return false;
    }
    cart_t *cart = ioopm_linked_list_get(db->carts, index).p;
    option_t res = ioopm_hash_table_lookup(cart->items, merch->name);
    if(!res.success){//Om man väljer fler antal eller samma tas den bort.
        printf("Merchandise not in cart");
        return false;
    }
    int cart_amnt = res.value.i;

    if(amnt >= cart_amnt){
        ioopm_hash_table_remove(cart->items, merch->name); 
    }
    else{
        ioopm_hash_table_insert(cart->items, merch->name, int_elem(cart_amnt - amnt));
    }
    return true;
}

// 2.1.11 Calculate Cost
// Calculate the cost of a shopping cart. If a cart holds 2 items of a merch M1 with a price of 50 and 8 items of a merch M2 with a price of 3 the cost of the cart is 2×50+8×3=124
// The action code shall be "=". The action shall read the id of the cart.

int calculate_cost(db_t *db, int cart_id){
    int index = cart_index(db, cart_id);

    if(index == -1){
        printf("Cart does not exist");
        return -1;
    }
    int sum = 0;
    cart_t *cart = ioopm_linked_list_get(db->carts, index).p;
    ioopm_hash_table_t *items = cart->items;

    //We now have the hash_table of the cart, iterate through it and calculate cost.
    for(size_t i = 0; i < items->size; i++){
        entry_t *entry = items->buckets[i].next;
        while (entry)
        {
            elem_t merch_name = entry->key;
            elem_t quantity = entry->value;

            option_t res = ioopm_hash_table_lookup(db->merch_ht, merch_name);
            if(res.success){
                merch_t *merch = res.value.p;
                int price = merch->price;
                sum += price*quantity.i;
            }
            entry = entry->next;
        }
    }
    return sum;
}

// 2.1.12 Checkout
// This action represents the user going through with a purchase of all the items in a particular shopping cart.
// Decrease the stock for the merches in the cart.
// Remove the shopping cart from the system.
// The action code shall be "O". The action shall read the id of the cart.

bool checkout_cart(db_t *db, int cart_id){
    int index = cart_index(db, cart_id);

    if(index == -1){
        printf("Cart does not exist"); 
        return false;
    }
    cart_t *cart = ioopm_linked_list_get(db->carts, index).p;
    ioopm_hash_table_t *items = cart->items;

    // For each item in the cart
    for(size_t i = 0; i < items->size; i++){
        entry_t *entry = items->buckets[i].next;
        while(entry){
            elem_t merch_name = entry->key;
            int quantity = entry->value.i;

            option_t res = ioopm_hash_table_lookup(db->merch_ht, merch_name);

            if(res.success){
                merch_t *merch = res.value.p;
                if(merch->total_stock < quantity){
                    printf("Not enough stock for %s\n", (char *)merch_name.p);
                    return false;
                    // Optionally: return false; to abort checkout if any item can't be fulfilled
                } else {
                    merch->total_stock -= quantity;

                    // Remove quantity from shelves (locations)
                    int remaining = quantity;
                    for(size_t j = 0; j < merch->locations->size && remaining > 0; j++){
                        stock_t *stock = ioopm_linked_list_get(merch->locations, j).p;
                        if(stock->quantity >= remaining){
                            stock->quantity -= remaining;
                            remaining = 0;
                        } else {
                            remaining -= stock->quantity;
                            stock->quantity = 0;
                        }
                    }
                }
            }
            entry = entry->next;
        }
    }

    // Remove and free the cart
    ioopm_linked_list_remove(db->carts, index);
    ioopm_hash_table_destroy(cart->items);
    free(cart);

    return true;
}

