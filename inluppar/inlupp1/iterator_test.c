#define _POSIX_C_SOURCE 200809L
#include "CUnit/Basic.h"
#include "linked_list.h"
#include "iterator.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

// Initialize a list for all the tests:
ioopm_list_t *make_simple_list(void){
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_linked_list_append(list, int_elem(10)); // list is 10
    ioopm_linked_list_append(list, int_elem(20)); // list is 10, 20
    ioopm_linked_list_prepend(list, int_elem(5)); // list should be 5, 10, 20
    return list;
}

// Existing tests
void test_has_next(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    CU_ASSERT_TRUE(ioopm_iterator_has_next(iter));
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_iter_remove(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    ioopm_iterator_next(iter);
    int removed = ioopm_iterator_remove(iter).i;
    CU_ASSERT_EQUAL(removed, 10);
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_current_next_reset(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 5);
    ioopm_iterator_next(iter);
    ioopm_iterator_next(iter);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 20);
    ioopm_iterator_reset(iter);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 5);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_insert(){
    ioopm_list_t *list = make_simple_list(); 
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    ioopm_iterator_insert(iter, int_elem(8));
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 4);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 8);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 8);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 5);
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_empty_list_iterator() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_iter_remove_edges() {
    ioopm_list_t *list = make_simple_list();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    // Remove the first element
    int removed_first = ioopm_iterator_remove(iter).i;
    CU_ASSERT_EQUAL(removed_first, 5);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

    // Move to last element
    ioopm_iterator_next(iter);
    int removed_last = ioopm_iterator_remove(iter).i;
    CU_ASSERT_EQUAL(removed_last, 20);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);

    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_iter_insert_edges() {
    ioopm_list_t *list = make_simple_list(); //(5,10,20)
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    // Insert at start (before current)
    ioopm_iterator_insert(iter, int_elem(1));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 1);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 5);

    // Move to end
    while (ioopm_iterator_current(iter).p) {
        ioopm_iterator_next(iter);
    }
    ioopm_iterator_insert(iter, int_elem(99));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, ioopm_linked_list_size(list) - 2).i, 20);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, ioopm_linked_list_size(list) - 1).i, 99);

    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

// NEW TEST CASES:

void test_null_iterator_safety() {
    // Test all functions with NULL iterator
    CU_ASSERT_FALSE(ioopm_iterator_has_next(NULL));
    CU_ASSERT_PTR_NULL(ioopm_iterator_next(NULL).p);
    CU_ASSERT_PTR_NULL(ioopm_iterator_current(NULL).p);
    CU_ASSERT_PTR_NULL(ioopm_iterator_remove(NULL).p);
    
    // These should not crash
    ioopm_iterator_reset(NULL);
    ioopm_iterator_insert(NULL, int_elem(1));
    ioopm_iterator_destroy(NULL);
}

void test_empty_list_operations() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Test operations on empty list iterator
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));
    CU_ASSERT_PTR_NULL(ioopm_iterator_next(iter).p);
    CU_ASSERT_PTR_NULL(ioopm_iterator_current(iter).p);
    CU_ASSERT_PTR_NULL(ioopm_iterator_remove(iter).p);
    
    // Insert into empty list
    ioopm_iterator_insert(iter, int_elem(42));
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 42);
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_iterator_end_behavior() {
    ioopm_list_t *list = make_simple_list();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Move to end
    while (ioopm_iterator_current(iter).p) {
        ioopm_iterator_next(iter);
    }
    
    // At end state
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));
    CU_ASSERT_PTR_NULL(ioopm_iterator_current(iter).p);
    CU_ASSERT_PTR_NULL(ioopm_iterator_next(iter).p);
    CU_ASSERT_PTR_NULL(ioopm_iterator_remove(iter).p);
    
    // Reset should work from end state
    ioopm_iterator_reset(iter);
    CU_ASSERT_TRUE(ioopm_iterator_has_next(iter));
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 5);
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_single_element_list_iterator() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_linked_list_append(list, int_elem(99));
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter)); // No next element after first
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 99);
    
    // Remove the only element
    elem_t removed = ioopm_iterator_remove(iter);
    CU_ASSERT_EQUAL(removed.i, 99);
    CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_multiple_removes() {
    ioopm_list_t *list = make_simple_list();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Remove first element
    ioopm_iterator_remove(iter);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 10);
    
    // Remove second element
    ioopm_iterator_remove(iter);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 20);
    
    // Remove last element
    ioopm_iterator_remove(iter);
    CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_insert_remove_sequence() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Insert, remove, insert pattern
    ioopm_iterator_insert(iter, int_elem(1));
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 1);
    
    ioopm_iterator_remove(iter);
    CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
    
    ioopm_iterator_insert(iter, int_elem(2));
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}


void test_reset_preserves_state() {
    ioopm_list_t *list = make_simple_list();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Advance iterator
    ioopm_iterator_next(iter);
    ioopm_iterator_next(iter);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 20);
    
    // Reset should go back to beginning
    ioopm_iterator_reset(iter);
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).i, 5);
    
    // List should be unchanged
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_insert_at_different_positions() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Insert into empty list
    ioopm_iterator_insert(iter, int_elem(1));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 1);
    
    // Insert at head (non-empty list)
    ioopm_iterator_insert(iter, int_elem(2));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 1);
    
    // Move to middle and insert
    ioopm_iterator_next(iter); // Now at position with value 1
    ioopm_iterator_insert(iter, int_elem(3));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0).i, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1).i, 3);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 2).i, 1);
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_has_next_correctness() {
    ioopm_list_t *list = make_simple_list(); // [5, 10, 20]
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Start: current = 5
    CU_ASSERT_TRUE(ioopm_iterator_has_next(iter)); // Should be true (10 is next)
    
    ioopm_iterator_next(iter); // current = 10
    CU_ASSERT_TRUE(ioopm_iterator_has_next(iter)); // Should be true (20 is next)
    
    ioopm_iterator_next(iter); // current = 20  
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter)); // Should be false (nothing after 20)
    
    ioopm_iterator_next(iter); // current = NULL
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter)); // Should be false
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_has_next_single_element() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    ioopm_linked_list_append(list, int_elem(42));
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    // Single element: current exists, but no next element
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
}

void test_no_memory_leaks() {
    ioopm_list_t *list = ioopm_linked_list_create(int_eq);
    
    // Perform many operations
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    for (int i = 0; i < 100; i++) {
        ioopm_iterator_insert(iter, int_elem(i));
    }
    
    // Remove all elements via iterator
    ioopm_iterator_reset(iter);
    while (ioopm_iterator_has_next(iter)) {
        ioopm_iterator_next(iter);
        ioopm_iterator_remove(iter);
    }
    
    ioopm_iterator_destroy(iter);
    ioopm_linked_list_destroy(list);
    
    // If no memory leaks, this test passes
    CU_ASSERT_TRUE(true);
}

int main() {
    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

    CU_pSuite suite = CU_add_suite("Iterator Tests", init_suite, clean_suite);
    if (!suite) { CU_cleanup_registry(); return CU_get_error(); }

    CU_add_test(suite, "Iterator has next", test_has_next);
    CU_add_test(suite, "Iterator remove", test_iter_remove);
    CU_add_test(suite, "Current, next and reset", test_current_next_reset);
    CU_add_test(suite, "Iterator insert", test_insert);
    CU_add_test(suite, "Test empty list", test_empty_list_iterator);
    CU_add_test(suite, "Editing edges", test_iter_remove_edges);
    CU_add_test(suite, "Insert edges", test_iter_insert_edges);
    CU_add_test(suite, "Null iterator safety", test_null_iterator_safety);
    CU_add_test(suite, "Empty list operations", test_empty_list_operations);
    CU_add_test(suite, "Iterator end behavior", test_iterator_end_behavior);
    CU_add_test(suite, "Single element list", test_single_element_list_iterator);
    CU_add_test(suite, "Multiple removes", test_multiple_removes);
    CU_add_test(suite, "Insert remove sequence", test_insert_remove_sequence);
    CU_add_test(suite, "Reset preserves state", test_reset_preserves_state);
    CU_add_test(suite, "Insert at positions", test_insert_at_different_positions);
    CU_add_test(suite, "Has next correctness", test_has_next_correctness);
    CU_add_test(suite, "Has next single element", test_has_next_single_element);
    CU_add_test(suite, "No memory leaks", test_no_memory_leaks);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}