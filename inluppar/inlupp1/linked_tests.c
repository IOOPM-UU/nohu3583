#define _POSIX_C_SOURCE 200809L
#include "CUnit/Basic.h"
#include "linked_list.h"  // replace with your header
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

void test_create_destroy() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
    ioopm_linked_list_destroy(list);
}

void test_append_prepend() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);

    ioopm_linked_list_append(list, int_elem(10)); // list is 10
    ioopm_linked_list_append(list, int_elem(20)); // list is 10, 20
    ioopm_linked_list_prepend(list, int_elem(5)); // list should be 5, 1, 20

    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 5);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 10);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 2).i, 20);

    ioopm_linked_list_destroy(list);
}

void test_insert_remove() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);

    ioopm_linked_list_append(list, int_elem(1)); // list is 1
    ioopm_linked_list_append(list, int_elem(3));  //list is 1,3
    ioopm_linked_list_insert(list, 1, int_elem(2));  // list: 1,2,3

    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);

    int removed = ioopm_linked_list_remove(list, 1).i; // remove 2
    CU_ASSERT_EQUAL(removed, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

    ioopm_linked_list_destroy(list);
}

void test_contains_and_get() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);

    ioopm_linked_list_append(list, int_elem(5));
    ioopm_linked_list_append(list, int_elem(10));

    CU_ASSERT_TRUE(ioopm_linked_list_contains(list, int_elem(5)));
    CU_ASSERT_TRUE(ioopm_linked_list_contains(list, int_elem(10)));
    CU_ASSERT_FALSE(ioopm_linked_list_contains(list, int_elem(51)));

    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 5);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 10);

    ioopm_linked_list_destroy(list);
}

bool is_positive(elem_t key, elem_t value, void *extra) {
    (void)value;
    (void)extra;
    return key.i > 0;
}

void increment(elem_t key, elem_t *value, void *extra) {
    (void)key;
    (void)extra;
    value->i += 1;
}

void test_predicate_functions() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_linked_list_append(list, int_elem(1));
    ioopm_linked_list_append(list, int_elem(2));
    ioopm_linked_list_append(list, int_elem(3));

    CU_ASSERT_TRUE(ioopm_linked_list_all(list, is_positive, NULL));
    CU_ASSERT_TRUE(ioopm_linked_list_any(list, is_positive, NULL));

    ioopm_linked_list_apply_to_all(list, increment, NULL);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 3);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 2).i, 4);

    ioopm_linked_list_destroy(list);
}

void test_clear() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_linked_list_append(list, int_elem(1));
    ioopm_linked_list_append(list, int_elem(2));
    ioopm_linked_list_clear(list);

    CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list),0);
    ioopm_linked_list_destroy(list);
}

void test_empty_list_operations() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    
    // Test operations on empty list
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, 0).p, NULL);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).p, NULL);
    CU_ASSERT_FALSE(ioopm_linked_list_contains(list, int_elem(1)));
    
    ioopm_linked_list_destroy(list);
}

void test_invalid_indices() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_linked_list_append(list, int_elem(1));
    
    // Negative indices
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, -1).p, NULL);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, -1).p, NULL);
    
    // Indices beyond size
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 5).p, NULL);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, 5).p, NULL);
    
    ioopm_linked_list_destroy(list);
}

void test_single_element_list() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_linked_list_append(list, int_elem(42));
    
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 42);
    CU_ASSERT_TRUE(ioopm_linked_list_contains(list, int_elem(42)));
    
    // Remove the only element
    elem_t removed = ioopm_linked_list_remove(list, 0);
    CU_ASSERT_EQUAL(removed.i, 42);
    CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
    
    ioopm_linked_list_destroy(list);
}


void test_insert_at_boundaries() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    
    // Insert at beginning of empty list
    ioopm_linked_list_insert(list, 0, int_elem(1));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 1);
    
    // Insert at end
    ioopm_linked_list_insert(list, 1, int_elem(3));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 3);
    
    // Insert in middle
    ioopm_linked_list_insert(list, 1, int_elem(2));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 2);
    
    ioopm_linked_list_destroy(list);
}

void test_clear_non_empty() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    
    // Add multiple elements
    for (int i = 0; i < 10; i++) {
        ioopm_linked_list_append(list, int_elem(i));
    }
    
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 10);
    ioopm_linked_list_clear(list);
    CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
    
    // Ensure list is still usable after clear
    ioopm_linked_list_append(list, int_elem(100));
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);
    
    ioopm_linked_list_destroy(list);
}

void test_destroy_non_empty() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    
    // Add elements and immediately destroy
    for (int i = 0; i < 5; i++) {
        ioopm_linked_list_append(list, int_elem(i));
    }
    
    ioopm_linked_list_destroy(list); // Should not leak memory
}

bool always_true(elem_t key, elem_t value, void *extra) {
    (void)key; (void)value; (void)extra;
    return true;
}

bool always_false(elem_t key, elem_t value, void *extra) {
    (void)key; (void)value; (void)extra;
    return false;
}

void test_predicate_edge_cases() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    
    // Test all with empty list (should return true - vacuous truth)
    CU_ASSERT_TRUE(ioopm_linked_list_all(list, always_true, NULL));
    
    // Test any with empty list (should return false)
    CU_ASSERT_FALSE(ioopm_linked_list_any(list, always_true, NULL));
    
    ioopm_linked_list_append(list, int_elem(1));
    
    // Test all with always_false
    CU_ASSERT_FALSE(ioopm_linked_list_all(list, always_false, NULL));
    
    // Test any with always_false  
    CU_ASSERT_FALSE(ioopm_linked_list_any(list, always_false, NULL));
    
    ioopm_linked_list_destroy(list);
}

bool string_eq(elem_t a, elem_t b) {
    return strcmp(a.p, b.p) == 0;
}

void test_string_data() {
    ioopm_list_t *list = ioopm_linked_list_create(string_eq);
    
    ioopm_linked_list_append(list, (elem_t){.p = "hello"});
    ioopm_linked_list_append(list, (elem_t){.p = "world"});
    
    CU_ASSERT_TRUE(ioopm_linked_list_contains(list, (elem_t){.p = "hello"}));
    CU_ASSERT_FALSE(ioopm_linked_list_contains(list, (elem_t){.p = "test"}));
    
    ioopm_linked_list_destroy(list);
}

int main() {
    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

    CU_pSuite suite = CU_add_suite("Linked List Tests", init_suite, clean_suite);
    if (!suite) { CU_cleanup_registry(); return CU_get_error(); }

    CU_add_test(suite, "Create/Destroy", test_create_destroy);
    CU_add_test(suite, "Append and Prepend", test_append_prepend);
    CU_add_test(suite, "Insert and Remove", test_insert_remove);
    CU_add_test(suite, "Contains and Get", test_contains_and_get);
    CU_add_test(suite, "Predicate and Apply", test_predicate_functions);
    CU_add_test(suite, "Clear List", test_clear);
    CU_add_test(suite, "Empty list operations", test_empty_list_operations);
    CU_add_test(suite, "Invalid indices on list", test_invalid_indices);
    CU_add_test(suite, "Tests on single element", test_single_element_list);
    CU_add_test(suite, "Insering on boundaries", test_insert_at_boundaries);
    CU_add_test(suite, "Clearing non empty list", test_clear_non_empty);
    CU_add_test(suite, "Destroy non empty list", test_clear_non_empty);
    CU_add_test(suite, "Predicate testing", test_predicate_edge_cases);
    CU_add_test(suite, "String testing with predicate", test_string_data);








    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
