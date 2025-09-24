#include "CUnit/Basic.h"
#include "hash_table.h"

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

    ioopm_hash_table_insert(ht, 7, "a");

    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, 7));
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_get(ht, 7), "a");

    // Insert same key → update value
    ioopm_hash_table_insert(ht, 7, "b");
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_get(ht, 7), "b");

    ioopm_hash_table_destroy(ht);
}

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

    CU_pSuite suite = CU_add_suite("Hash table tests", init_suite, clean_suite);
    if (!suite) { CU_cleanup_registry(); return CU_get_error(); }

    if (!CU_add_test(suite, "Create/Destroy", test_create_destroy)) { CU_cleanup_registry(); return CU_get_error(); }
    if (!CU_add_test(suite, "Insert and Access", test_hash_insert)) { CU_cleanup_registry(); return CU_get_error(); }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
