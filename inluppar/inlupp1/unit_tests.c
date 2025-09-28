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
      ioopm_hash_table_t *ht = ioopm_hash_table_create();
      CU_ASSERT_PTR_NOT_NULL(ht);
      ioopm_hash_table_destroy(ht);
  }

  void test_hash_insert()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create();

      ioopm_hash_table_insert(ht, 7, strdup("a"));
      CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, 7));
      CU_ASSERT_STRING_EQUAL(ioopm_hash_table_get(ht, 7), "a");

      // Insert same key → update value
      ioopm_hash_table_insert(ht, 7, strdup("b"));
      CU_ASSERT_STRING_EQUAL(ioopm_hash_table_get(ht, 7), "b");

      ioopm_hash_table_destroy(ht);
  }

  void test_hash_lookup_and_remove()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create();

      ioopm_hash_table_insert(ht, 8, strdup("b"));

      option_t res = ioopm_hash_table_lookup(ht, 8);
      assert(Successful(res));
      assert(strcmp(res.value, "b") == 0);

      ioopm_hash_table_remove(ht, 8);

      option_t res2 = ioopm_hash_table_lookup(ht, 8);
      assert(Unsuccessful(res2));

      ioopm_hash_table_destroy(ht);
  }

  void test_hash_empty()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create();
      bool result = ioopm_hash_table_is_empty(ht);
      assert(result == true);
      ioopm_hash_table_destroy(ht);
  }

  void test_hash_size()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create();
      ioopm_hash_table_insert(ht, 5, strdup("A"));
      ioopm_hash_table_insert(ht, 51, strdup("B"));
      ioopm_hash_table_insert(ht, 33, strdup("C"));

      assert(ioopm_hash_table_size(ht) == 3);
      ioopm_hash_table_destroy(ht);
  }

  void test_hash_clear()
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create();
      ioopm_hash_table_insert(ht, 5, strdup("A"));
      ioopm_hash_table_insert(ht, 51, strdup("B"));
      ioopm_hash_table_insert(ht, 33, strdup("C"));

      assert(ioopm_hash_table_size(ht) == 3);
      ioopm_hash_table_clear(ht);
      assert(ioopm_hash_table_is_empty(ht) == true);

      ioopm_hash_table_destroy(ht);
  }

  /// Test ioopm_hash_table_keys
  void test_hash_table_keys(void)
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create();

      ioopm_hash_table_insert(ht, 42, strdup("answer"));
      ioopm_hash_table_insert(ht, 17, strdup("seventeen"));
      ioopm_hash_table_insert(ht, 99, strdup("ninety-nine"));

      int n_keys = ioopm_hash_table_size(ht);
      int *keys = ioopm_hash_table_keys(ht);

      CU_ASSERT_EQUAL(n_keys, 3);

      bool found42 = false, found17 = false, found99 = false;
      for (int i = 0; i < n_keys; i++) {
          if (keys[i] == 42) found42 = true;
          if (keys[i] == 17) found17 = true;
          if (keys[i] == 99) found99 = true;
      }

      CU_ASSERT_TRUE(found42);
      CU_ASSERT_TRUE(found17);
      CU_ASSERT_TRUE(found99);

      free(keys);
      ioopm_hash_table_destroy(ht);
  }

  /// Test ioopm_hash_table_values
  void test_hash_table_values(void)
  {
      ioopm_hash_table_t *ht = ioopm_hash_table_create();

      ioopm_hash_table_insert(ht, 1, strdup("afsas"));
      ioopm_hash_table_insert(ht, 2, strdup("kekka"));
      ioopm_hash_table_insert(ht, 3, strdup("241r12"));

      int n_values = ioopm_hash_table_size(ht);
      char **values = ioopm_hash_table_values(ht);

      CU_ASSERT_EQUAL(n_values, 3);

      bool first = false, second = false, third = false;
      for (int i = 0; i < n_values; i++) {
          if (strcmp(values[i], "afsas") == 0) first = true;
          if (strcmp(values[i], "kekka") == 0) second = true;
          if (strcmp(values[i], "241r12") == 0) third = true;
      }

      CU_ASSERT_TRUE(first);
      CU_ASSERT_TRUE(second);
      CU_ASSERT_TRUE(third);

      free(values);
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

      CU_basic_set_mode(CU_BRM_VERBOSE);
      CU_basic_run_tests();
      CU_cleanup_registry();
      return CU_get_error();
  }
