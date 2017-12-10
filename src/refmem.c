#include "refmem.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define OBJECT_TO_RECORD(object) ((struct object_record*)(object) - 1)
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
void retain(obj o)
{
  if(o)
    {
      struct object_record *record = OBJECT_TO_RECORD(o);
      record->ref_count++;
    }
};

/**
 * Decrements an objects reference count by one. If this results in
 * the reference count hitting 0 the associated object will be deallocated.
 *
 * \param o The object for wich to decrement the reference count
 */
void release(obj o)
{
  if (o)
    {
      struct object_record *record = OBJECT_TO_RECORD(o);
      record->ref_count--;
      if (record->ref_count == 0) deallocate(o); 
    }
};

/**
 * Returns the current reference count of an object
 *
 * \param o The object for wich to return the reference count
 * \return size_t The current reference count for the object
 */
size_t rc(obj o)
{
  assert(o);
  struct object_record *record = OBJECT_TO_RECORD(o);
  return record->ref_count;  
};

/**
 * Default destructor function for use when allocate is called with NULL
 * as its destructor argument.
 *
 * \param o Data object to (not) be freed.
 */
void no_destructor(obj o)
{
  return;
};


/**
 * Allocates an object on the heap together with additional meta data
 * consisting of reference count and a pointer to a destructor function.
 *
 * The 'object' member is malloc'd since in this context, it's just a pointer, and therefore
 * the previous calloc only guarantees the survival of the pointer and no the data itself..
 *
 * \param bytes The size of the object to allocate
 * \param destructor Function pointer to the destructor
 *
 * \return obj Pointer to the data allocated by the client
 */
obj allocate(size_t bytes, function1_t destructor)
{
  struct object_record *record = malloc(bytes + sizeof(record));
  record->ref_count = 0;
  record->destroy = destructor ? destructor : no_destructor;
  return RECORD_TO_OBJECT(record);
};


obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor);

/**
 * Deallocates an object by passing it to its associated destructor function. 
 *
 * \param o The object to deallocate
 * \warning Passing NULL to the function causes an error, so does an object with a non-zero reference count.
 //FIXME: Causes SIGSEGV 
 */
void deallocate(obj o)
{
  assert(o);
  struct object_record *record = OBJECT_TO_RECORD(o);
  assert(record->ref_count == 0); 
  record->destroy(o);
  free(record);
};
/**
 * Modifies the cascade limit
 *
 * \param s The new cascade limit
 */
void set_cascade_limit(size_t s)
{
  assert(s > 0);
  cascade_limit = s;
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
