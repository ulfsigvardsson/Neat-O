#include "refmem.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
//#include <stdbool.h>

/// \def OBJECT_TO_RECORD(object)
/// Given a pointer 'object', steps back in memory the size of the object_record struct in order to point at the meta data before the actual data.
#define OBJECT_TO_RECORD(object) ((object_record_t*)(object) - 1)

#define IS_LAST_ALLOCATION(record) (!(record->previous) && !(record->next))
/// \def RECORD_TO_OBJECT(record)
/// Given a pointer 'record', steps forward in memory the size of the object_record struct in order to point at the actual data after the meta data.
#define RECORD_TO_OBJECT(record) ((obj)(record + 1))
#define CASCADE_LIMIT 1000

typedef struct object_record object_record_t;

static size_t cascade_limit = CASCADE_LIMIT;  /*!< The cascade limit with a default value. */
static object_record_t *last_allocation = NULL; /*!< Pointer to the 'first' heap object */

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
  object_record_t *previous;
  object_record_t *next; /*!< Pointer to next allocated object */       // 8 bytes
  size_t arr_len;  /*!< Length of array, if an array. */                // 8 bytes 
  //TODO: Implement ref_count, arr_len and if and object is an array or not, to a bitstring.
};

/**
 * Increments an objects reference count by one
 *
 * \param o The object for wich to increment the reference count
 */
void retain(obj object)
{
  if(object)
    {
      object_record_t *record = OBJECT_TO_RECORD(object);
      (record->ref_count)++;
    }
}

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
      if (rc(object) == 0)
        deallocate(object, true);

      else
        {
          (record->ref_count)--;
          if (rc(object) == 0)
            deallocate(object, true);           
        } 

    }
}

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

  //Check if anything has been allocated yet
  if (last_allocation) 
    last_allocation->next = record; 

  //Sets this object as the most recent allocation, points its previous pointer to the previous one
  //and its next pointer to NULL
  record->previous = last_allocation;
  record->next     = NULL;
  last_allocation  = record;
  
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
  object_record_t *record = OBJECT_TO_RECORD(object); 
  
  if(!object)
    return NULL;

  record->arr_len = elements;
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

  result = allocate(buf_size, NULL);

  if (!result)
    return NULL;
  
  strcpy(result, org);
  retain(result); 

  return result;
}

/**
 * Redirects pointers for an object to be deallocated.
 *
 * \param record The object record to be deallocated
 */

void redirect_heap_pointers(object_record_t *record)
{
  object_record_t *previous = record->previous;
  object_record_t *next     = record->next;

  //If no other heap objects remain then the final pointer is set to NULL
  if (IS_LAST_ALLOCATION(record))
    last_allocation = NULL;
  
  if(previous)
    previous->next = next;
  
  if(next)
    next->previous = previous;

  //Corner case: if this is the most recent allocation then last_allocation is stepped back one step in the list
  if (last_allocation == record)
    last_allocation = previous;
}
 
/**
 * Deallocates an object by passing it to its associated destructor function. 
 *
 * \param o The object to deallocate
 * \param destroy True if called through the retain function, if so the object's destructor is called.
 * \warning Passing NULL to the function causes an error, so does an object with a non-zero reference count. 
 */
void deallocate(obj object, bool destroy)
{
  assert(object); 
   
  object_record_t *record   = OBJECT_TO_RECORD(object); 
  redirect_heap_pointers(record);
  
  if (destroy)
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

/**
 * Frees all object whos reference count is 0, regardless of cascade limit
 */
void cleanup()
{
  object_record_t *current = last_allocation;
  object_record_t *previous;

  while (current)
    {
      previous = current->previous;
      obj object = RECORD_TO_OBJECT(current);
      
      if (rc(object) == 0) 
        deallocate(object, true); 
      
      current = previous;
    }
}

/**
 * Frees all remaining allocated heap objects regardless of reference count
 */
void shutdown()
{
  object_record_t *current = last_allocation;
  object_record_t *previous;

  while (current)
    {
      previous = current->previous; 
      obj object = RECORD_TO_OBJECT(current); 
      deallocate(object, false);
      current    = previous;
    }
}

