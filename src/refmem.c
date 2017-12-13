#include "refmem.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

/// \def OBJECT_TO_RECORD(object)
/// Given a pointer 'object', steps back in memory the size of the object_record struct in order to point at the meta data before the actual data.
#define OBJECT_TO_RECORD(object) ((struct object_record*)(object) - 1)

/// \def RECORD_TO_OBJECT(record)
/// Given a pointer 'record', steps forward in memory the size of the object_record struct in order to point at the actual data after the meta data.
#define RECORD_TO_OBJECT(record) ((obj)(record + 1))
#define CASCADE_LIMIT 1000

typedef struct object_record object_record_t;

static size_t cascade_limit = CASCADE_LIMIT;  /*!< The cascade limit with a default value. */
static object_record_t *garbage_start; /*!< Pointer to the 'first' dangling garbage object */

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
  object_record_t *next; /*!< Pointer to next dangling garbage object */// 8 bytes
  size_t arr_len;  /*!< Length of array, if an array. */                // 8 bytes
                                                                        // Totalt 32 bytes
  //TODO: Implement ref_count, arr_len and if an object is ana rray or not to a bitstring.
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
      (record->ref_count)++;
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
      object_record_t *record = OBJECT_TO_RECORD(object);
      (record->ref_count)--; 

      if (rc(object) == 0) deallocate(object); 
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
  object_record_t *record = OBJECT_TO_RECORD(object);
  return record->ref_count;  
};

/**
 * Default destructor function for use when allocate is called with NULL
 * as its destructor argument. This function is used for strings since they are simply
 * an array of chars adjacent to the object record and need no explicit free.
 *
 * \param o Data object to be freed.
 */
void no_destructor(obj object)
{
  return;
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
  
  set_cascade_limit(CASCADE_LIMIT); //reset cascade limit to its original value.
  object_record_t *record = malloc(sizeof(*record) + bytes);
 
  record->ref_count = 0;
  record->destroy   = destructor ? destructor : no_destructor;
  record->next      = NULL;
  
  return RECORD_TO_OBJECT(record);
}

/**
 * Allocates space for an array an initializes each byte to 0. Equivalent to 'calloc'.
 *
 * \param elements The number of elements in the array
 * \param elem_size The size of each array element 
 * \param destructor Function pointer to the destructor
 *
 * \return obj  Pointer to the first element of the array
 */
obj allocate_array(size_t elements, size_t elem_size, function1_t destructor)
{
  size_t total = elements * elem_size;
  obj object   = allocate(total, destructor);
  
  if(!object)
    return NULL;

  return memset(object, 0, total); // Detta nollar alla bytes, från 'object' och 'total' bytes framåt
}

/**
 * Own implementation of 'strdup'. Allocates space for a string and stores a string there.
 *
 * \param org The string to allocate
 * \return char* Pointer to the allocated string
 */
char *strdup2(char *org)
{
  char *result;
  int org_size = strlen(org);
  int buf_size = (org_size+1)*sizeof(char);

  result = (char*)allocate(buf_size, NULL);

  if (!result)
    return NULL;
  
  strcpy(result, org);
  retain(result);

  return result;
}

/**
 * Deallocates an object by passing it to its associated destructor function. 
 *
 * \param o The object to deallocate
 * \warning Passing NULL to the function causes an error, so does an object with a non-zero reference count. 
 */
void deallocate(obj object)
{
  assert(object);
  
  object_record_t *record = OBJECT_TO_RECORD(object); 
  record->destroy(object); //TODO: Kontrollera att vi inte överskrider cascade limit
  free(record);
}

/**
 * Modifies the cascade limit
 *
 * \param s The new cascade limit 
 */
//TODO: Upper limit? -- tidsbaserad (räkna tid i bytes)
void set_cascade_limit(size_t size)
{
  cascade_limit = abs(size);
}

/**
 * Returns the current cascade limit
 *
 * \return size_t The current cascade limit
 */
size_t get_cascade_limit()
{
  return cascade_limit;
}

void cleanup();
void shutdown();

