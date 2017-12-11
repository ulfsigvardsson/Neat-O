#include "refmem.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/// \def OBJECT_TO_RECORD(object)
/// Given a pointer object, steps back in memory the size of the object_record struct in order to point at the meta data before the actual data.
#define OBJECT_TO_RECORD(object) ((struct object_record*)(object) - 1)
/// \def RECORD_TO_OBJECT(record)
/// Given a pointer record, steps forward in memory the size of the object_record struct in order to point at the actual data after the meta data.
#define RECORD_TO_OBJECT(record) ((obj)(record + 1))

static size_t cascade_limit = 1000;  /*!< The cascade limit with a default value. */

/**
 * \struct object_record
 * The object_record struct contains meta data consisting of the
 * current amount of references to a heap object along with a function pointer
 * to a destructor function.
 */
struct object_record
{
  size_t ref_count;    /*!< Reference count for the object member */    // 8 bytes
  function1_t destroy; /*!< Pointer to a destructor function */         // 8 bytes
};

/**
 * Increments an objects referene count by one
 *
 * \param o The object for wich to increment the reference count
 */
void retain(obj object)
{
  if(object)
    {
      struct object_record *record = OBJECT_TO_RECORD(object);
      record->ref_count++;
    }
};

/**
 * Decrements an objects reference count by one. If this results in
 * the reference count hitting 0 the associated object will be deallocated.
 *
 * \param o The object for wich to decrement the reference count
 */
void release(obj object)
{
  if (object)
    {
      struct object_record *record = OBJECT_TO_RECORD(object);
      record->ref_count--;
      if (record->ref_count == 0) deallocate(object); 
    }
};

/**
 * Returns the current reference count of an object
 *
 * \param o The object for wich to return the reference count
 * \return size_t The current reference count for the object
 */
size_t rc(obj object)
{
  assert(object);
  struct object_record *record = OBJECT_TO_RECORD(object);
  return record->ref_count;  
};

/**
 * Default destructor function for use when allocate is called with NULL
 * as its destructor argument.
 *
 * \param o Data object to be freed.
 */
void no_destructor(obj object)
{
  free(object);
};


/**
 * Allocates space for an object on the heap together with additional meta data
 * consisting of reference count and a pointer to a destructor function.
 * Note that only the allocation of the actual data is made, no assumptions to what it
 * may contain are made.
 *
 * \param bytes The size of the object to allocate
 * \param destructor Function pointer to the destructor
 *
 * \return obj Pointer to the data allocated by the client
 */
obj allocate(size_t bytes, function1_t destructor)
{
  
  //Har vi skit som inte är freeat sen sist, gör det så länge vi inte överskrider cascade limit.
  //Om mängden ofreead data är mindre än 'bytes' måste vi dock freea tills vi har frigjorts
  // 'bytes' antal bytes eller tills det inte finns fler gamla objekt kvar.
  
  struct object_record *record = calloc(1, sizeof(*record) + bytes); 
  record->ref_count = 0;
  record->destroy = destructor ? destructor : no_destructor;
  return RECORD_TO_OBJECT(record);
};

char *strdup2(char *c)
{
  char *result = allocate(sizeof(char*), NULL);
  retain(c);
  return result;
}

obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor);

/**
 * Deallocates an object by passing it to its associated destructor function. 
 *
 * \param o The object to deallocate
 * \warning Passing NULL to the function causes an error, so does an object with a non-zero reference count. 
 */
void deallocate(obj object)
{
  printf("Deallocating object...\n");
  assert(object);
  
  struct object_record *record = OBJECT_TO_RECORD(object); 
  assert(record->ref_count == 0); 

  record->destroy(object); //Kontrollera att vi inte överskrider cascade limit
  free(record);

};
/**
 * Modifies the cascade limit
 *
 * \param s The new cascade limit 
 */
//TODO: Upper limit? -- tidsbaserad (räkna tid i bytes)
void set_cascade_limit(size_t size)
{
  cascade_limit = abs(size);
};

/**
 * Returns the current cascade limit
 *
 * \return size_t The current cascade limit
 */
size_t get_cascade_limit()
{
  return cascade_limit;
};

void cleanup();
void shutdown();

