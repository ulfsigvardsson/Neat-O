#include "CUnit/Basic.h"
#include "../src/refmem.c"

size_t different_sizes[] = {-999, -2, -1, 0, 1, 2, 999, 999999, 99999999, 999999999}; /* !< The smallest and largest numbers allowed before an overflow */
size_t upper_sizes[] = {-998, -1, 0, 1, 2, 3, 1000};
size_t lower_sizes[] = {-100, -3, -2, -1, 0, 1, 998};

rc_format different_rc[] = {0, 1, 2, 65534 }; /* !< The smallest and largest numbers allowed before an overflow */
rc_format upper_rc[] = {1, 2, 3, 65535};
rc_format lower_rc[] = { 0, 1, 65533};

int different_cl[] = {-999, -2, -1, 0, 1, 2, 999 }; /* !< The smallest and largest numbers allowed before an overflow */
int upper_cl[] = {-998, -1, 0, 1, 2, 3, 1000};
int lower_cl[] = {-1000, -3, -2, -1, 0, 1, 998};

char *different_strings[] = {"Hej", "Albin", "Ulf", "Sebastian", "Marcus", "Oskar", "Odd" };


  
int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

//--------------------------------------------------------------------------------------------
// Manual Active Reference Collector (MARC)



/**
 * Test for the retain function
 */
void retain_test(void)
{
  for (int i = 0; i < 4; i++)
    {
      obj tmp_object = constructor_allocate_tester((size_t) 32, NULL, different_rc[i]);
      retain(tmp_object);
      object_record_t *tmp_record = OBJECT_TO_RECORD(tmp_object);
      CU_ASSERT_TRUE((tmp_record->ref_count) == upper_rc[i]);

    }
}

/**
 * Test for the release function
 */
void release_test(void)
{
  for (int i = 0; i < 3; i++)
    {
      obj tmp_object = constructor_allocate_tester((size_t) 32, NULL, different_rc[i+1]);
      release(tmp_object);
      object_record_t *tmp_record = OBJECT_TO_RECORD(tmp_object);
      CU_ASSERT_TRUE((tmp_record->ref_count) == lower_rc[i]);
    }
}

/**
 * Test for the rc function
 */
void rc_test(void)
{
  for (int i = 0; i < 4; i++)
    {
      obj tmp_object = constructor_allocate_tester((size_t) 32, NULL, different_rc[i]);
      retain(tmp_object);
      CU_ASSERT_TRUE(rc(tmp_object) == upper_rc[i]);
      release(tmp_object);
      CU_ASSERT_TRUE(rc(tmp_object) == different_rc[i]);
    }
}

/**
 * Test for the allocate function
 */
void allocate_test(void)
{
  for (int i = 0; i < 7; i++)
    {
      obj tmp = allocate(different_sizes[i], NULL);
      CU_ASSERT_TRUE(sizeof(tmp) == 8);
    }
}

/**
 * Test for the allocate_array function
 */
void allocate_array_test(void)
{
  size_t array_size = 10;
  int *tmp = allocate_array(array_size, sizeof(int), NULL);
  
  for (int i = 0; i < (int) array_size; i++)
    {
      tmp[i] = i;
      CU_ASSERT_TRUE(tmp[i] == i);  
    }
}

/**
 * Test for the strdup2 function
 */

void allocate_string_test(void)
{
  for (int i = 0; i < 7; i++)
    {
      char *tmp_string = strdup(different_strings[i]);
      char *test_string = allocate_string(different_strings[i]);
      CU_ASSERT_TRUE(strcmp(tmp_string, test_string) == 0);
    }
  
}

/**
 * Test for the deallocate function
 */
void deallocate_test(void)
{
  //The test for deallocate not really deemed possible to test!
  printf("Not a possible test!...Technically ");
}

/**
 * Test for the set_cascade_limit function
 */
void set_cascade_limit_test(void)
{
  for(int i = 0; i < 7; i++)
    {
      set_cascade_limit(different_sizes[i+3]);
      CU_ASSERT_TRUE(cascade_limit == different_sizes[i+3]);
    }
}

/**
 * Test for the get_cascade_limit function
 */
void get_cascade_limit_test(void)
{  
  for(int i = 0; i < 7; i++)
    {
      set_cascade_limit(different_sizes[i+3]);
      size_t tmp_cascade_limit = get_cascade_limit();
      CU_ASSERT_TRUE(tmp_cascade_limit == different_sizes[i+3]);
    }
}

/**
 * Test for the cleanup function
 */
void cleanup_test(void)
{
  obj tmp1 = allocate((size_t) 32, NULL);
  obj tmp2 = allocate((size_t) 32, NULL);
  obj tmp3 = allocate((size_t) 32, NULL);
  object_record_t *test1 = OBJECT_TO_RECORD(tmp1);
  object_record_t *test2 = OBJECT_TO_RECORD(tmp2);
  object_record_t *test3 = OBJECT_TO_RECORD(tmp3);
  retain(tmp2);
  cleanup();
  CU_ASSERT_TRUE(garbage->size == 0 && rc(tmp2) == 1);
}

/**
 * Test for the shutdown function
 */
void shutdown_test(void)
{
  printf("Not a possible test!... Technically ");
}





//--------------------------------------------------------------------------------------------
// main

int main(int argc, char *argv[])
{
  
  CU_pSuite MARC_suite = NULL;
  
  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* adds the MARC_suite to the registry */   
  MARC_suite = CU_add_suite( "MARC_test_suite", init_suite, clean_suite );
  if ( NULL == MARC_suite ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
      (NULL == CU_add_test(MARC_suite, "retain", retain_test)) ||
      (NULL == CU_add_test(MARC_suite, "release", release_test)) ||
      (NULL == CU_add_test(MARC_suite, "rc", rc_test)) ||
      (NULL == CU_add_test(MARC_suite, "allocate", allocate_test)) ||
      (NULL == CU_add_test(MARC_suite, "allocate_array", allocate_array_test)) ||
      (NULL == CU_add_test(MARC_suite, "allocate_string", allocate_string_test)) ||
      (NULL == CU_add_test(MARC_suite, "deallocate", deallocate_test)) ||
      (NULL == CU_add_test(MARC_suite, "set_cascade_limit", set_cascade_limit_test)) ||
      (NULL == CU_add_test(MARC_suite, "get_cascade_limit", get_cascade_limit_test)) ||
      (NULL == CU_add_test(MARC_suite, "cleanup", cleanup_test)) ||
      (NULL == CU_add_test(MARC_suite, "shutdown", shutdown_test))
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();

  return CU_get_error();
}
