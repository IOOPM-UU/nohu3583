#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef union elem elem_t;

union elem
{
  int i;
  unsigned int u;
  bool b;
  float f;
  void *p;
};

typedef bool ioopm_predicate(elem_t key, elem_t value, void *extra);
typedef void ioopm_apply_function(elem_t key, elem_t *value, void *extra);

#define int_elem(x) (elem_t) { .i=(x) }
#define ptr_elem(x) (elem_t) { .p=(x) }

#define ioopm_int_str_ht_insert(ht, i, s) \
   ioopm_hash_table_insert(ht, int_elem(i), ptr_elem(s))

typedef bool ioopm_eq_function(elem_t a, elem_t b);
typedef int ioopm_hash_func(elem_t key);

// === Equality function prototypes ===
bool int_eq(elem_t a, elem_t b);
bool str_eq(elem_t a, elem_t b);
bool ptr_eq(elem_t a, elem_t b);

// === Hash function prototypes ===
int hash_int(elem_t key);
int hash_str(elem_t key);

#endif // COMMON_H
