#include "refmem.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* =================================
 * -------------MACROS--------------
 * =================================
 */

/**
 * @def CALCULATE_SIZE(object, record)
 * Given a obj pointer @object and an object record pointer @record, calculates the total
 * amount of bytes contained by the object record.
 */
#define CALCULATE_SIZE(object, record) sizeof(object_record_t)+sizeof(*object)*(record->array_length)

/**
 * @def OBJECT_TO_RECORD(object)
 * Given a pointer @object, steps back in memory the size of the object_record struct
 * in order to point at the meta data before the actual data.
 */
#define OBJECT_TO_RECORD(object) ((object_record_t*)(object) - 1);
/**
 * @def RECORD_TO_OBJECT(record)
 * Given a pointer @record steps forward in memory the size of @see object_record_t
 * in order to point at the actual data after the meta data.
 */
#define RECORD_TO_OBJECT(record) ((obj)(record + 1));

/**
 * @def CASCADE_LIMIT
 * Global constant for the default value of the cascade limit.
 */
#define CASCADE_LIMIT 10

/* =================================
 * --------TYPE DEFINITIONS---------
 * =================================
 */

/**
 *  @typedef object_record_t
 */

typedef struct object_record object_record_t;

/* =================================
 * --------STATIC VARIABLES---------
 * =================================
 */

static size_t cascade_limit = CASCADE_LIMIT;     /*!< The cascade limit with a default value. */
static object_record_t *last_allocation   = NULL;  /*!< Pointer to the most recently allocated heap object with rc > 0*/
static object_record_t *remaining_garbage = NULL;/*!< Pointer to the 'first' remaining heap object with rc == 0 */

/* =================================
 * -------STRUCTS & PROTOTYPES------
 * =================================
 */

void cleanup_before_allocation(size_t bytes);

/**
 * @struct object_record
 * The object_record struct contains meta data consisting of the
 * current amount of references to a heap object along with a function pointer
 * to a destructor function.
 */
struct object_record
{
  function1_t destroy;        	/*!< Pointer to a destructor function */          // 8 bytes
  object_record_t *next;  		/*!< Pointer to the next allocated object */ 	  // 8 bytes
  object_record_t *previous;  	/*!< Pointer to the previusly allocated object */ // 8 bytes
  rc_format ref_count;        	/*!< Reference count for the object */            // 2 bytes
  short array_length;         	/*!< Array length for the object */               // 2 bytes
  // Total: 32 bytes
};

/* =================================
 * -----------FUNCTIONS------
 * =================================
 */

/**
 * Initializes the 'previous' pointer for a newly allocated object_record_t
 *
 * @param record The object record that was allocated
 */
void set_heap_pointers(object_record_t *record)
{
	record->next 	 = NULL;

	if (last_allocation)
		last_allocation->next = record;

	record->previous = last_allocation;
	last_allocation  = record;
}

/**
 * Initializes the 'previous' pointer for a newly allocated object_record_t
 *
 * @param record The object record that was allocated
 */
void set_garbage_pointers(object_record_t *record)
{
	if(record)
	{
		record->previous = remaining_garbage;
		record->next     = NULL;

		if (remaining_garbage)
			remaining_garbage->next = record;

		remaining_garbage  = record;
	}
}

/**
 * Redirects pointers for an garbage object to be deallocated.
 *
 * @param record The garbage object record to be deallocated
 */
void redirect_garbage_pointers(object_record_t *record)
{
  if (record)
    {
      object_record_t *previous = record->previous;
      object_record_t *next     = record->next;

      //If no other heap objects remain then the final pointer is set to NULL
      if (!previous && !next)
        remaining_garbage = NULL;

      if(next)
        next->previous = previous;

      if(previous)
        previous->next = next;

      //Corner case: if this is the most recent allocation then last_allocation is stepped back one step in the list
      if(remaining_garbage == record)
        remaining_garbage = previous;
    }

}

/**
 * Redirects pointers for an object to be deallocated.
 *
 * @param record The object record to be deallocated
 */
void redirect_heap_pointers(object_record_t *record)
{
	if (record)
	{
		object_record_t *previous = record->previous;
		object_record_t *next     = record->next;

		//If no other heap objects remain then the final pointer is set to NULL
		if (!previous && !next)
			last_allocation = NULL;

		if(next)
			next->previous = previous;

		if(previous)
			previous->next = next;

		//Corner case: if this is the most recent allocation then last_allocation is stepped back one step in the list
		if (last_allocation == record)
			last_allocation = previous;

		set_garbage_pointers(record);
	}
}

/**
 * Increments an objects reference count by one
 *
 * @param o The object for wich to increment the reference count
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
 * @param o The object for wich to decrement the reference count
 */
void release(obj object)
{
  if (object)
    {
      object_record_t *record = OBJECT_TO_RECORD(object);
      (record->ref_count)--;

      if (rc(object) <= 0)
      	{
          redirect_heap_pointers(record);

          if(get_cascade_limit() > 0)
            {
              int size_freed = CALCULATE_SIZE(object, record);
              deallocate(object);
              set_cascade_limit(get_cascade_limit()-size_freed);
            }
    	}
    }
}

/**
 * Returns the current reference count of an object
 *
 * @param o The object for wich to return the reference count
 * @return size_t The current reference count for the object
 */
rc_format rc(obj object)
{
  assert(object);
  object_record_t *record = OBJECT_TO_RECORD(object);
  return record->ref_count;
}

/**
 * Default destructor function for use when allocate is called with NULL
 * as its destructor argument. This function is used for strings since they are simply
 * an array of chars adjacent to the object record and need no explicit free.
 *
 * @param o Data object to be freed.
 */
void no_destructor(obj object)
{
  return;
}


/**
 * Allocates space for an object on the heap together with additional meta data
 * consisting of reference count and a pointer to a destructor function.
 * Note that only the allocation of the actual data is made, no assumptions to what it
 * may contain are made.
 *
 * @param bytes The size of the object to allocate
 * @param destructor Function pointer to the destructor
 *
 * @return obj Pointer to the data allocated by the client
 */
obj allocate(size_t bytes, function1_t destructor)
{
  set_cascade_limit(CASCADE_LIMIT); //reset cascade limit to its original value.

  if (remaining_garbage)
    cleanup_before_allocation(bytes);

  object_record_t *record = (object_record_t*) malloc(sizeof(*record) + bytes);

  if (!record)
    return NULL;

  record->ref_count    = 0;
  record->array_length = 1;
  record->destroy      = destructor ? destructor : no_destructor;
  set_heap_pointers(record);

  return RECORD_TO_OBJECT(record);
}

void cleanup_before_allocation(size_t bytes)
{
  size_t released_memory = 0;

  while (remaining_garbage && (released_memory <= bytes || get_cascade_limit() > 0))
    {
      obj object = RECORD_TO_OBJECT(remaining_garbage);

      if (rc(object) <= 0)
        {
          size_t old_limit = CALCULATE_SIZE(object, remaining_garbage);
          released_memory += old_limit;
          size_t new_limit = get_cascade_limit()-old_limit;
          deallocate(object);
          set_cascade_limit(new_limit);
        }

    }
}

/**
 * Allocates space for an array an initializes each byte to 0. Equivalent to 'calloc'.
 *
 * @param elements The number of elements in the array
 * @param elem_size The size of each array element
 * @param destructor Function pointer to the destructor
 *
 * @return obj Pointer to the first element of the array
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
 * @param org The string to allocate
 * @return char* Pointer to the allocated string
 */
char *strdup2(char *org)
{
  char *result;
  int org_size = strlen(org);
  int buf_size = (org_size+1) * sizeof(char);

  result = (char*) allocate(buf_size, NULL);

  if (!result)
    return NULL;

  object_record_t *record = OBJECT_TO_RECORD(result);
  record->array_length    = buf_size;
  strcpy(result, org);
  retain(result);

  return result;
}

/**
 * Deallocates an object by passing it to its associated destructor function.
 *
 * @param o The object to deallocate
 * @warning Passing NULL to the function causes an error, so does an object with a non-zero reference count.
 */
void deallocate(obj object)
{
  assert(object);

  object_record_t *record = OBJECT_TO_RECORD(object);

  assert(rc(object) <= 0);
  redirect_garbage_pointers(record);
  record->destroy(object);
  free(record);

}

/**
 * Modifies the cascade limit
 *
 * @param limit The new cascade limit
 */
//TODO: Upper limit? -- tidsbaserad (räkna tid i bytes)
void set_cascade_limit(size_t limit)
{
  if ((int)limit < 0)
    cascade_limit = 0;
  else
    cascade_limit = limit;
}

/**
 * Returns the current cascade limit
 *
 * @return size_t The current cascade limit
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
  while (remaining_garbage)
    {
      obj object = RECORD_TO_OBJECT(remaining_garbage);

      deallocate(object);
    }
}

/**
 * Frees all remaining allocated heap objects regardless of reference count
 */
void shutdown()
  {
    object_record_t *current = last_allocation;

    cleanup();

    while (current)
      {
        object_record_t *temp = current;
        current    = current->previous;
        free(temp);
      }
  }

