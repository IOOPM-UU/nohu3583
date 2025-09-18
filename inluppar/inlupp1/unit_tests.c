#include "CUnit/Basic.h"
#include "hash_table.h"

int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create();
   CU_ASSERT_PTR_NOT_NULL(ht);
   ioopm_hash_table_destroy(ht);
}

int main() {
  // Initialize the test registry
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // Add a test suite
  CU_pSuite my_test_suite = CU_add_suite("Hash table tests", init_suite, clean_suite);
  if (my_test_suite == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Add the create/destroy test
  if (CU_add_test(my_test_suite, "Create and Destroy", test_create_destroy) == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Run the tests in verbose mode
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  // Clean up
  CU_cleanup_registry();
  return CU_get_error();
}
