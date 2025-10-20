#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"
#include "common.h"



/// @brief Iterator structure for the linked list
typedef struct iterator {
    ioopm_list_t *list;     // pointer to underlying list
    ioopm_link_t *current;  // current node in iteration
    ioopm_link_t *prev;     // previous node (needed for remove)
} ioopm_list_iterator_t;


/// @brief Create an iterator for a given list
ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list) {
    ioopm_list_iterator_t *iter = malloc(sizeof(ioopm_list_iterator_t));
    iter->list = list;
    iter->current = list->head;
    iter->prev = NULL;
    return iter;
}

/// @brief Checks if there are more elements to iterate over
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter) {
    if (!iter || !iter->current) return false;
    return iter->current->next != NULL;  // Check if there's a NEXT element
}

/// @brief Step the iterator forward one step
elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter) {
    if (!iter || !iter->current) return (elem_t){ .p = NULL };  // FIXED
    elem_t val = iter->current->element;
    iter->prev = iter->current;
    iter->current = iter->current->next;
    return val;
}

/// @brief Return the current element from the underlying list
elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter) {
    if (!iter || !iter->current) return (elem_t){ .p = NULL };  // FIXED
    return iter->current->element;
}

/// @brief Reposition the iterator at the start of the underlying list
void ioopm_iterator_reset(ioopm_list_iterator_t *iter) {
    if (!iter) return;  // ADDED
    iter->current = iter->list->head;
    iter->prev = NULL;
}

/// @brief Remove the current element from the underlying list
elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter){
    if (!iter || !iter->current) return (elem_t){ .p = NULL };  // FIXED

    ioopm_link_t *remove = iter->current;
    elem_t elem = remove->element;

    if (ioopm_linked_list_size(iter->list) == 1) {
        // Handle single element case directly
        iter->list->head = NULL;
        iter->list->tail = NULL;
        iter->current = NULL;
        iter->prev = NULL;
    } else if (iter->prev == NULL) {
        // Removing head
        iter->list->head = remove->next;
        iter->current = remove->next;
    } else {
        // Removing middle or tail
        iter->prev->next = remove->next;
        iter->current = remove->next;
        if (remove == iter->list->tail) {
            iter->list->tail = iter->prev;
        }
    }

    free(remove);
    iter->list->size--;
    return elem;
}

/// @brief Insert a new element into the underlying list making the current element it's next
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element) {
    if(!iter) return;
    ioopm_link_t *new_node = calloc(1, sizeof(ioopm_link_t));
    new_node->element = element;

    if (ioopm_linked_list_size(iter->list) == 0) { // empty list
        iter->list->head = new_node;
        iter->list->tail = new_node;
        iter->current = new_node;
    } else if (iter->prev == NULL) { // insert at head
        new_node->next = iter->current;
        iter->list->head = new_node;
        iter->current = new_node;
    } else if (iter->current == NULL) { // insert at end  <-- ADD THIS CASE
        iter->prev->next = new_node;
        iter->list->tail = new_node;
        iter->current = new_node;
    } else { // insert in middle
        new_node->next = iter->current;
        iter->prev->next = new_node;
        iter->current = new_node;
    }

    iter->list->size++;
}

/// @brief Destroy the iterator and return its resources
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter){
    if (!iter) return;  // ADDED
    free(iter);
}
