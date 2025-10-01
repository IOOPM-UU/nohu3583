#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"


typedef struct link ioopm_link_t;

struct link {
    elem_t element;
    ioopm_link_t *next;
};

typedef struct list {
    ioopm_link_t *head;   // first node
    ioopm_link_t *tail;   // last node;
    size_t size;    // number of elements
    ioopm_eq_function *func; //boolean  euq function
} ioopm_list_t;

/// @brief Creates a new empty list
/// @return an empty linked list
ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function *eq_func){
    ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
    list->func = eq_func;
    return list;
}

/// @brief Tear down the linked list and return all its memory (but not the memory of the elements)
/// @param list the list to be destroyed
void ioopm_linked_list_destroy(ioopm_list_t *list) {
    if (!list) return;
    ioopm_link_t *current = list->head;
    while (current != NULL) {
        ioopm_link_t *tmp = current;
        current = current->next;
        free(tmp);
    }
    free(list);  // finally free the container itself
}

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ioopm_linked_list_append(ioopm_list_t *list, elem_t value){
    ioopm_link_t *new_node = calloc(1, sizeof(ioopm_link_t));
    new_node->element = value;
    new_node->next = NULL; // since we add it in the last place in the list there will be nothing after

    if (list->tail == NULL) {
        // list is empty
        list->head = new_node;
        list->tail = new_node;
    } else {
        // attach after old tail
        list->tail->next = new_node;
        list->tail = new_node;
    }
    list->size++; // updates the size to correct value
}

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended to
/// @param value the value to be prepended
void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value) {
    ioopm_link_t *new_node = calloc(1, sizeof(ioopm_link_t));
    new_node->element = value;
    new_node->next = list->head;

    // Update head
    list->head = new_node;

    // If list was empty, also update tail
    if (list->size == 0) {
        list->tail = new_node;
    }

    list->size++;
}

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ioopm_linked_list_size(ioopm_list_t *list){
    if (!list) return 0;
    return (int) list->size;
}

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be inserted 
void ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t value) {
    int size = ioopm_linked_list_size(list);

    // Validate input
    if (index < 0 || index > size) {
        return; // invalid index
    }

    // Case 1: insert at the beginning
    if (index == 0) {
        ioopm_linked_list_prepend(list, value);
        return;
    }

    // Case 2: insert at the end
    if (index == size) {
        ioopm_linked_list_append(list, value);
        return;
    }

    // Case 3: insert in the middle
    ioopm_link_t *new_node = calloc(1, sizeof(ioopm_link_t));
    new_node->element = value;

    // Walk to node just before the insertion point
    ioopm_link_t *current = list->head;
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }

    // Insert new node
    new_node->next = current->next;
    current->next = new_node;

    list->size++;
}

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list
/// @param index the position in the list
/// @return the value removed
elem_t ioopm_linked_list_remove(ioopm_list_t *list, int index) {
    int size = ioopm_linked_list_size(list);

    if (index < 0 || index >= size) {
        return (elem_t){ .p = NULL };
    }

    elem_t value;

    if (index == 0) {
        ioopm_link_t *tmp = list->head;
        value = tmp->element;
        list->head = tmp->next;
        if (size == 1) {
            list->tail = NULL; 
        }
        free(tmp);
    }
    else {
        ioopm_link_t *current = list->head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        ioopm_link_t *tmp = current->next;
        value = tmp->element;
        current->next = tmp->next;
        if (index == size - 1) {
            list->tail = current;
        }
        free(tmp);
    }

    list->size--;
    return value;
}

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @return the value at the given position
elem_t ioopm_linked_list_get(ioopm_list_t *list, int index){
    int size = ioopm_linked_list_size(list);
    
    if (index < 0 || index >= size) {
        return (elem_t){ .p = NULL };
    }

    ioopm_link_t *current = list->head;
    for(int i = 0; i < index; i++){
        current = current->next;
    }
    return current->element;
}

/// @brief Test if an element is in the list
/// @param list the linked list
/// @param element the element sought
/// @return true if element is in the list, else false
bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element){
    ioopm_link_t *current = list->head;

    while (current != NULL){
        if(list->func(current->element, element)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ioopm_linked_list_is_empty(ioopm_list_t *list){
    return (list->size == 0);
}

/// @brief Remove all elements from a linked list
/// @param list the linked list
void ioopm_linked_list_clear(ioopm_list_t *list) {
    ioopm_link_t *current = list->head;
    while (current != NULL) {
        ioopm_link_t *tmp = current;
        current = current->next;
        free(tmp);
    }

    // Reset container fields
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

typedef bool ioopm_int_predicate(int value, void *extra);
typedef void ioopm_apply_int_function(int *value, void *extra);

/// @brief Test if a supplied property holds for all elements in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested (function pointer)
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, void *extra) {
    ioopm_link_t *current = list->head;

    while (current != NULL) {
        if (!prop(current->element ,current->element, extra)) {
            return false; // property failed, bail early
        }
        current = current->next;
    }
    return true; // all elements satisfied
}

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate *prop, void *extra){
    ioopm_link_t *current = list->head;

    while (current != NULL)
    {
        if(prop(current->element, current->element, extra)){
            return true;
        }
        current = current->next;
    }
    return false; 
}

/// @brief Apply a supplied function to all elements in a list.
/// @param list the linked list
/// @param fun the function to be applied
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of fun
void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_function *fun, void *extra){
    ioopm_link_t *current = list->head;

    while (current != NULL)
    {
        fun(current->element, &current->element, extra);
        current = current->next;
    }
    return;
}

