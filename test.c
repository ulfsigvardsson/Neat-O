#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "CUnit/Basic.h"
//#include "memref.h"       // Add when memref is part of the directory

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
}

/**
 * Test for the release function
 */
void release_test(void)
{
}

/**
 * Test for the re function
 */
void re_test(void)
{
}

/**
 * Test for the allocate function
 */
void allocate_test(void)
{
}

/**
 * Test for the allocate_array function
 */
void allocate_array_test(void)
{
}

/**
 * Test for the deallocate function
 */
void deallocate_test(void)
{
}

/**
 * Test for the set_cascade_limit function
 */
void set_cascade_limit_test(void)
{
}

/**
 * Test for the get_cascade_limit function
 */
void get_cascade_limit_test(void)
{
}

/**
 * Test for the cleanup function
 */
void cleanup_test(void)
{
}

/**
 * Test for the shutdown function
 */
void shutdown_test(void)
{
}





//--------------------------------------------------------------------------------------------
// main

int main(void){
  
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
      (NULL == CU_add_test(MARC_suite, "re", re_test)) ||
      (NULL == CU_add_test(MARC_suite, "allocate", allocate_test)) ||
      (NULL == CU_add_test(MARC_suite, "allocate_array", allocate_array_test)) ||
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
