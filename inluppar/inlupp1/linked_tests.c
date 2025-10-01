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

    int removed = ioopm_linked_list_remove(list, int_elem(1)).i; // remove 2
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

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
