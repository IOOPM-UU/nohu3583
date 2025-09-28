#define _POSIX_C_SOURCE 200809L
#include "CUnit/Basic.h"
#include "linked_list.h"  // replace with your header
#include "iterator.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

//initalize a list for all the tests:

ioopm_list_t *make_simple_list(void){
    ioopm_list_t *list = ioopm_linked_list_create();
    ioopm_linked_list_append(list, 10); // list is 10
    ioopm_linked_list_append(list, 20); // list is 10, 20
    ioopm_linked_list_prepend(list, 5); // list should be 5, 10, 20
    return list;
}

void test_has_next(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    CU_ASSERT_TRUE(ioopm_iterator_has_next(iter));
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list); //to not get any leaks in memory
}

void test_iter_remove(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    ioopm_iterator_next(iter);
    int removed = ioopm_iterator_remove(iter);
    CU_ASSERT_EQUAL(removed, 10);
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list); //to not get any leaks in memory
}

void test_current_next_reset(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    CU_ASSERT_EQUAL(ioopm_iterator_current(iter), 5);
    ioopm_iterator_next(iter);
    ioopm_iterator_next(iter);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter), 20);
    ioopm_iterator_reset(iter);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter), 5);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_insert(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    ioopm_iterator_insert(iter, 8);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 4);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter), 8);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0), 8);  // original head changed to 8
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1), 5);  // new element which makes this the second elem
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);

}

void test_empty_list_iterator() {
    ioopm_list_t *list = ioopm_linked_list_create();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));

    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_iter_remove_edges() {
    ioopm_list_t *list = make_simple_list();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    // Remove the first element
    int removed_first = ioopm_iterator_remove(iter);
    CU_ASSERT_EQUAL(removed_first, 5);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

    // Move to last element
    ioopm_iterator_next(iter);
    int removed_last = ioopm_iterator_remove(iter);
    CU_ASSERT_EQUAL(removed_last, 20);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);

    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_iter_insert_edges() {
    ioopm_list_t *list = make_simple_list();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    // Insert at start (before current)
    ioopm_iterator_insert(iter, 1);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0), 1); // head changes to 1
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1), 5); // new node before head

    // Move to end
    while (ioopm_iterator_has_next(iter)) {
        ioopm_iterator_next(iter);
    }
    ioopm_iterator_insert(iter, 99);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, ioopm_linked_list_size(list) - 2), 20);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, ioopm_linked_list_size(list) - 1), 99);

    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}


int main() {
    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

    CU_pSuite suite = CU_add_suite("Linked List Tests", init_suite, clean_suite);
    if (!suite) { CU_cleanup_registry(); return CU_get_error(); }

    CU_add_test(suite, "Iterator has next", test_has_next);
    CU_add_test(suite, "Iterator remove", test_iter_remove);
    CU_add_test(suite, "Current, next and reset", test_current_next_reset);
    CU_add_test(suite, "Iterator insert", test_insert);
    CU_add_test(suite,"Test empty list", test_empty_list_iterator);
    CU_add_test(suite,"Editing edges", test_iter_remove_edges);
    CU_add_test(suite,"Insert edges", test_iter_insert_edges);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
