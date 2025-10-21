  #define _POSIX_C_SOURCE 200809L
  #include "CUnit/Basic.h"
  #include "hash_table.h"
  #include <assert.h>
  #include <string.h>
  #include <stdlib.h>
  

  int init_suite(void) { return 0; }
  int clean_suite(void) { return 0; }

  void test_create_destroy()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);
      CU_ASSERT_PTR_NOT_NULL(ht);
      ioopm_hash_table_destroy(ht);
  }

void test_hash_insert()
{
    // Integer keys, string values
    ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);

    // Insert key=7, value="a"
    ioopm_hash_table_insert(ht, int_elem(7), ptr_elem("a"));
    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, int_elem(7)));

    elem_t val = ioopm_hash_table_get(ht, int_elem(7));
    CU_ASSERT_STRING_EQUAL(val.p, "a");

    // Insert same key → update value
    ioopm_hash_table_insert(ht, int_elem(7), ptr_elem("b"));
    val = ioopm_hash_table_get(ht, int_elem(7));
    CU_ASSERT_STRING_EQUAL(val.p, "b");

    ioopm_hash_table_destroy(ht);
}

  void test_hash_lookup_and_remove()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);

      ioopm_hash_table_insert(ht, int_elem(8), ptr_elem("b"));

      option_t res = ioopm_hash_table_lookup(ht, int_elem(8));
      assert(Successful(res));
      assert(strcmp(res.value.p, "b") == 0);

      ioopm_hash_table_remove(ht, int_elem(8));

      option_t res2 = ioopm_hash_table_lookup(ht, int_elem(8));
      assert(Unsuccessful(res2));

      ioopm_hash_table_destroy(ht);
  }

  void test_hash_empty()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);
      bool result = ioopm_hash_table_is_empty(ht);
      assert(result == true);
      ioopm_hash_table_destroy(ht);
  }

  void test_hash_size()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);
      ioopm_hash_table_insert(ht, int_elem(5), ptr_elem("A"));
      ioopm_hash_table_insert(ht, int_elem(51), ptr_elem("B"));
      ioopm_hash_table_insert(ht, int_elem(33), ptr_elem("C"));

      assert(ioopm_hash_table_size(ht) == 3);
      ioopm_hash_table_destroy(ht);
  }

  void test_hash_clear()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);
      ioopm_hash_table_insert(ht, int_elem(5), ptr_elem("A"));
      ioopm_hash_table_insert(ht, int_elem(51), ptr_elem("B"));
      ioopm_hash_table_insert(ht, int_elem(33), ptr_elem("C"));

      assert(ioopm_hash_table_size(ht) == 3);
      ioopm_hash_table_clear(ht);
      assert(ioopm_hash_table_is_empty(ht) == true);

      ioopm_hash_table_destroy(ht);
  }

  /// Test ioopm_hash_table_keys
  void test_hash_table_keys(void)
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);

      ioopm_hash_table_insert(ht, int_elem(42), ptr_elem("answer"));
      ioopm_hash_table_insert(ht, int_elem(17), ptr_elem("seventeen"));
      ioopm_hash_table_insert(ht, int_elem(99), ptr_elem("ninety-nine"));

      int n_keys = ioopm_hash_table_size(ht);
      ioopm_list_t *keys = ioopm_hash_table_keys(ht);

      CU_ASSERT_EQUAL(n_keys, 3);

      bool found42 = false, found17 = false, found99 = false;
      for (int i = 0; i < n_keys; i++) {
          if (ioopm_linked_list_get(keys, i).i == 42) found42 = true;
          if (ioopm_linked_list_get(keys, i).i == 17) found17 = true;
          if (ioopm_linked_list_get(keys, i).i == 99) found99 = true;
      }

      CU_ASSERT_TRUE(found42);
      CU_ASSERT_TRUE(found17);
      CU_ASSERT_TRUE(found99);

      ioopm_hash_table_destroy(ht);
      ioopm_linked_list_destroy(keys);
}

  /// Test ioopm_hash_table_values
  void test_hash_table_values(void)
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);

      ioopm_hash_table_insert(ht, int_elem(1), ptr_elem("afsas"));
      ioopm_hash_table_insert(ht, int_elem(2), ptr_elem("kekka"));
      ioopm_hash_table_insert(ht, int_elem(3), ptr_elem("241r12"));

      int n_values = ioopm_hash_table_size(ht);
      ioopm_list_t *values = ioopm_hash_table_values(ht);

      CU_ASSERT_EQUAL(n_values, 3);

      bool first = false, second = false, third = false;
      for (int i = 0; i < n_values; i++) {
          if (strcmp(ioopm_linked_list_get(values, i).p, "afsas") == 0) first = true;
          if (strcmp(ioopm_linked_list_get(values, i).p, "kekka") == 0) second = true;
          if (strcmp(ioopm_linked_list_get(values, i).p, "241r12") == 0) third = true;
      }

      CU_ASSERT_TRUE(first);
      CU_ASSERT_TRUE(second);
      CU_ASSERT_TRUE(third);

      ioopm_linked_list_destroy(values);
      ioopm_hash_table_destroy(ht);
  }


  void test_remove_nonexistent_key()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);
    ioopm_hash_table_insert(ht, int_elem(1), ptr_elem("one"));
    ioopm_hash_table_remove(ht, int_elem(999)); // Should not crash or affect table
    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, int_elem(1)));
    ioopm_hash_table_destroy(ht);
}

void test_duplicate_keys(void) {
    ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);
    elem_t key = int_elem(1);
    elem_t value1 = int_elem(10);
    elem_t value2 = int_elem(20);

    ioopm_hash_table_insert(ht, key, value1);
    ioopm_hash_table_insert(ht, key, value2);

    option_t result = ioopm_hash_table_lookup(ht, key);
    CU_ASSERT_TRUE(result.success);
    CU_ASSERT_EQUAL(result.value.i, 20);

    ioopm_hash_table_destroy(ht);
}

void test_has_key_and_value(void) {
    ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);
    elem_t key = int_elem(1);
    elem_t value = int_elem(10);

    ioopm_hash_table_insert(ht, key, value);

    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, key));
    CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value));

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, int_elem(2)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, int_elem(20)));

    ioopm_hash_table_destroy(ht);
}

bool is_greater_than(elem_t key, elem_t value, void *arg) {
    int threshold = *(int*)arg;
    return value.i > threshold;
}

void test_ioopm_hash_table_any(void) {
    ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int, int_eq);

    // Insert some test data
    ioopm_hash_table_insert(ht, int_elem(1), int_elem(10));
    ioopm_hash_table_insert(ht, int_elem(2), int_elem(20));
    ioopm_hash_table_insert(ht, int_elem(3), int_elem(30));

    int threshold = 25;
    bool result = ioopm_hash_table_any(ht, is_greater_than, &threshold);

    assert(result == true); // Because 30 > 25

    threshold = 35;
    result = ioopm_hash_table_any(ht, is_greater_than, &threshold);

    assert(result == false); // No value > 35

    ioopm_hash_table_destroy(ht);
}

  int main()
  {
      if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

      CU_pSuite suite = CU_add_suite("Hash table tests", init_suite, clean_suite);
      if (!suite) { CU_cleanup_registry(); return CU_get_error(); }

      CU_add_test(suite, "Create/Destroy", test_create_destroy);
      CU_add_test(suite, "Insert and Access", test_hash_insert);
      CU_add_test(suite, "Insert, then lookup and after destroy", test_hash_lookup_and_remove);
      CU_add_test(suite, "Hash table empty", test_hash_empty);
      CU_add_test(suite, "Test size function", test_hash_size);
      CU_add_test(suite, "Test hash clear", test_hash_clear);
      CU_add_test(suite, "Get hash values", test_hash_table_keys);
      CU_add_test(suite, "Get hash keys", test_hash_table_values);
      CU_add_test(suite, "Remove non existant key", test_remove_nonexistent_key); 
      CU_add_test(suite, "Test adding on same key", test_duplicate_keys); 
      CU_add_test(suite, "Has key and value test", test_has_key_and_value); 
      CU_add_test(suite, "Test any using greater than", test_ioopm_hash_table_any); 



      CU_basic_set_mode(CU_BRM_VERBOSE);
      CU_basic_run_tests();
      CU_cleanup_registry();
      return CU_get_error();
  }
