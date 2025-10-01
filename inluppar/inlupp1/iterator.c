#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"
#include "common.h"


struct link {
    elem_t element;
    ioopm_link_t *next;
};


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
    return iter->current != NULL;
}

/// @brief Step the iterator forward one step
elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter) {
    if (!iter->current) return (elem_t){ .p = NULL };
    elem_t val = iter->current->element;
    iter->prev = iter->current;
    iter->current = iter->current->next;
    return val;
}

/// @brief Return the current element from the underlying list
elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter) {
    if (!iter->current) return (elem_t){ .p = NULL };
    return iter->current->element;
}

/// @brief Reposition the iterator at the start of the underlying list
void ioopm_iterator_reset(ioopm_list_iterator_t *iter) {
    iter->current = iter->list->head;
    iter->prev = NULL;
}

/// @brief Remove the current element from the underlying list
elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter){
    if (!iter->current) return (elem_t){ .p = NULL };

    ioopm_link_t *remove = iter->current;
    elem_t elem = remove->element;

    if (ioopm_linked_list_size(iter->list) == 1) {
        ioopm_linked_list_clear(iter->list);
        iter->current = NULL;
        iter->prev = NULL;
        return elem;
    }

    ioopm_link_t *next_node = remove->next;

    if (iter->prev == NULL) { // removing head
        iter->list->head = next_node;
        if (remove == iter->list->tail) {
            iter->list->tail = NULL;
        }
    } else { // removing middle or tail
        iter->prev->next = next_node;
        if (remove == iter->list->tail) {
            iter->list->tail = iter->prev;
        }
    }

    free(remove);
    iter->current = next_node;
    iter->list->size--;
    return elem;
}

/// @brief Insert a new element into the underlying list making the current element it's next
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element) {
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
    } else { // insert in middle or before tail
        new_node->next = iter->current;
        iter->prev->next = new_node;
        if (iter->prev == iter->list->tail) { // update tail if needed
            iter->list->tail = new_node;
        }
        iter->current = new_node;
    }

    iter->list->size++;
}

/// @brief Destroy the iterator and return its resources
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter){
    free(iter);
}
