#define _POSIX_C_SOURCE 200809L
#include "CUnit/Basic.h"
#include "db.h"
#include <stdlib.h>
#include <string.h>

/* Suite setup/teardown */
int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/* --- TESTS BEGIN --- */

void test_create_and_destroy_db(void)
{
    db_t *db = create_db();
    CU_ASSERT_PTR_NOT_NULL(db);
    CU_ASSERT_PTR_NOT_NULL(db->merch_ht);
    CU_ASSERT_PTR_NOT_NULL(db->shelf_ht);
    CU_ASSERT_PTR_NOT_NULL(db->carts);
    CU_ASSERT_EQUAL(db->next_cart_id, 1);
    destroy_db(db);
}

/* add_merch + lookup */
void test_add_merch(void)
{
    db_t *db = create_db();
    bool ok = add_merch(db, "Banana", "Yellow fruit", 10);
    CU_ASSERT_TRUE(ok);
    option_t res = ioopm_hash_table_lookup(db->merch_ht, ptr_elem("Banana"));
    CU_ASSERT_TRUE(res.success);
    merch_t *m = res.value.p;
    CU_ASSERT_STRING_EQUAL(m->name, "Banana");
    CU_ASSERT_STRING_EQUAL(m->desc, "Yellow fruit");
    CU_ASSERT_EQUAL(m->price, 10);
    destroy_db(db);
}

/* add_merch duplicate rejection */
void test_add_merch_duplicate(void)
{
    db_t *db = create_db();
    CU_ASSERT_TRUE(add_merch(db, "Apple", "Red fruit", 5));
    CU_ASSERT_FALSE(add_merch(db, "Apple", "Green fruit", 7));
    destroy_db(db);
}

/* replenish_stock and print_stock */
void test_replenish_stock(void)
{
    db_t *db = create_db();
    add_merch(db, "Apple", "Red fruit", 5);
    CU_ASSERT_TRUE(replenish_stock(db, "A1", ptr_elem("Apple"), 10));
    CU_ASSERT_TRUE(replenish_stock(db, "A2", ptr_elem("Apple"), 5));
    merch_t *m = ioopm_hash_table_lookup(db->merch_ht, ptr_elem("Apple")).value.p;
    CU_ASSERT_EQUAL(m->total_stock, 15);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(db->shelf_ht), 2);
    destroy_db(db);
}

/* replenish_stock conflict detection */
void test_replenish_conflict(void)
{
    db_t *db = create_db();
    add_merch(db, "Banana", "Yellow", 5);
    add_merch(db, "Mango", "Sweet", 8);
    CU_ASSERT_TRUE(replenish_stock(db, "B1", ptr_elem("Banana"), 5));
    CU_ASSERT_FALSE(replenish_stock(db, "B1", ptr_elem("Mango"), 3));
    destroy_db(db);
}

/* change_merch */
void test_change_merch(void)
{
    db_t *db = create_db();
    add_merch(db, "Book", "Novel", 20);
    CU_ASSERT_TRUE(change_merch(db, "Book", "Book2", 25, "Sci-fi", "Y"));
    option_t res = ioopm_hash_table_lookup(db->merch_ht, ptr_elem("Book2"));
    CU_ASSERT_TRUE(res.success);
    merch_t *m = res.value.p;
    CU_ASSERT_STRING_EQUAL(m->name, "Book2");
    CU_ASSERT_EQUAL(m->price, 25);
    destroy_db(db);
}

/* remove_merch */
void test_remove_merch(void)
{
    db_t *db = create_db();
    add_merch(db, "Chair", "Wooden", 30);
    CU_ASSERT_TRUE(remove_merch(db, "Chair", "Y"));
    option_t res = ioopm_hash_table_lookup(db->merch_ht, ptr_elem("Chair"));
    CU_ASSERT_FALSE(res.success);
    destroy_db(db);
}

/* cart creation and removal */
void test_cart_create_remove(void)
{
    db_t *db = create_db();
    cart_t *cart = create_cart(db);
    CU_ASSERT_PTR_NOT_NULL(cart);
    CU_ASSERT_EQUAL(cart->id, 1);
    CU_ASSERT_TRUE(remove_cart(db, 1, "Y"));
    CU_ASSERT_EQUAL(db->carts->size, 0);
    destroy_db(db);
}

/* add_to_cart and calculate_cost */
void test_add_to_cart_and_cost(void)
{
    db_t *db = create_db();
    add_merch(db, "Book", "Novel", 50);
    replenish_stock(db, "S1", ptr_elem("Book"), 10);
    cart_t *cart = create_cart(db);
    merch_t *m = ioopm_hash_table_lookup(db->merch_ht, ptr_elem("Book")).value.p;

    CU_ASSERT_TRUE(add_to_cart(db, m, 2, cart));
    CU_ASSERT_EQUAL(m->reserved, 2);
    CU_ASSERT_EQUAL(calculate_cost(db, cart->id), 100);
    destroy_db(db);
}

/* remove_from_cart */
void test_remove_from_cart(void)
{
    db_t *db = create_db();
    add_merch(db, "Pen", "Blue ink", 3);
    replenish_stock(db, "P1", ptr_elem("Pen"), 10);
    cart_t *cart = create_cart(db);
    merch_t *m = ioopm_hash_table_lookup(db->merch_ht, ptr_elem("Pen")).value.p;
    add_to_cart(db, m, 5, cart);

    CU_ASSERT_TRUE(remove_from_cart(db, m, 2, cart->id));
    CU_ASSERT_EQUAL(m->reserved, 3);
    CU_ASSERT_TRUE(remove_from_cart(db, m, 3, cart->id)); // remove completely
    CU_ASSERT_EQUAL(m->reserved, 0);
    destroy_db(db);
}

/* checkout_cart */
void test_checkout_cart(void)
{
    db_t *db = create_db();
    add_merch(db, "Laptop", "Gaming", 1000);
    replenish_stock(db, "S1", ptr_elem("Laptop"), 3);
    cart_t *cart = create_cart(db);
    merch_t *m = ioopm_hash_table_lookup(db->merch_ht, ptr_elem("Laptop")).value.p;
    add_to_cart(db, m, 2, cart);
    CU_ASSERT_TRUE(checkout_cart(db, cart->id));
    CU_ASSERT_EQUAL(m->total_stock, 1);
    CU_ASSERT_EQUAL(m->reserved, 0);
    destroy_db(db);
}

/* --- REGISTER TESTS --- */

int main()
{
    CU_pSuite suite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    suite = CU_add_suite("Database Tests", init_suite, clean_suite);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "create and destroy db", test_create_and_destroy_db);
    CU_add_test(suite, "add merch", test_add_merch);
    CU_add_test(suite, "duplicate merch", test_add_merch_duplicate);
    CU_add_test(suite, "replenish stock", test_replenish_stock);
    CU_add_test(suite, "replenish conflict", test_replenish_conflict);
    CU_add_test(suite, "change merch", test_change_merch);
    CU_add_test(suite, "remove merch", test_remove_merch);
    CU_add_test(suite, "cart create remove", test_cart_create_remove);
    CU_add_test(suite, "add to cart + cost", test_add_to_cart_and_cost);
    CU_add_test(suite, "remove from cart", test_remove_from_cart);
    CU_add_test(suite, "checkout cart", test_checkout_cart);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
