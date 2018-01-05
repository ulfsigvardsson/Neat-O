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
#define CASCADE_LIMIT 500000

/* =================================
 * --------TYPE DEFINITIONS---------
 * =================================
 */

/**
 *  @typedef object_record_t
 */

typedef struct object_record object_record_t;
typedef struct link link_t;
typedef struct list list_t;

/* =================================
 * --------STATIC VARIABLES---------
 * =================================
 */

static size_t cascade_limit = CASCADE_LIMIT;      	/*!< The cascade limit with a default value. */
static size_t released_memory = 0;                      /*!< Global variable tracking number of released bytes of a given cycle */
static list_t *garbage     = NULL;
static list_t *destructors = NULL;
static bool ignore_cascade_limit = false;

/* =================================
 * -------STRUCTS & PROTOTYPES------
 * =================================
 */

void cleanup_before_allocation(size_t bytes);
void no_destructor(obj object);

struct link
{
  obj data;
  link_t *next;
};

struct list
{
  link_t *first;
  link_t *last;
  size_t size;
};

/**
 * @struct object_record
 * The object_record struct contains meta data consisting of the
 * current amount of references to a heap object along with a function pointer
 * to a destructor function.
 */
struct object_record
{
  unsigned long size;           /*!< Number of bytes occupied by the object including meta data, max 4.294.967.295 */  // 4 bytes
  rc_format ref_count;        	/*!< Reference count for the object, max 4.294.967.295 */                              // 4 bytes
  unsigned char destr_index;    /*!< Pointer to a destructor function, max 255 */                                      // 1 bytes
  // Total: 9 bytes
};

/* =================================
 * -----------FUNCTIONS------
 * =================================
 */

void free_link(link_t *link)
{
  if (link)
    {
      free_link(link->next);
      free(link);
    }
}

void free_list(list_t *list)
{
  if (list)
    {
      free_link(list->first); 
      free(list);
    }

}

function1_t get_destructor(unsigned char index)
{
  link_t *result = destructors->first;

  for (unsigned short i = 0; i < index; i++)
    result = result->next;

  return (function1_t)result->data;
}

link_t **find_link(list_t *list, obj data)
{
  link_t **current = &(list->first);

  while (*current)
    {
      if ((*current)->data == data)
        return current;

      current = &(*current)->next;
    }

  return current;
}

unsigned char add_to_destructors(function1_t destructor)
{
  link_t **current = &(destructors->first);
  int index = 0;

  if (!destructor)
    return index;

  while (*current)
    {
      if ((*current)->data == destructor)
        return index;

      ++index;
      current = &(*current)->next;
    }

  *current          = (link_t*)calloc(1, sizeof(link_t));
  (*current)->data  = (function1_t*)destructor;
  destructors->last = *current;
  ++(destructors->size);

  return index;
}

void initialize_destructors()
{
  destructors        = (list_t*)calloc(1, sizeof(list_t));
  link_t *first      = (link_t*)calloc(1, sizeof(link_t));
  first->data = (obj)no_destructor;
  destructors->first = first;
  destructors->last  = first;
  destructors->size = 1;
}

bool rc_overflow(obj object)
{
  return rc(object) == ULONG_MAX;
}

/**
 * Initializes the 'previous' pointer for a newly allocated object_record_t
 *
 * @param record The object record that was allocated
 */
void set_garbage_pointers(object_record_t *record)
{
  link_t *new_point = (link_t*)calloc(1, sizeof(link_t));
  new_point->data = record;

  if (garbage->size == 0)
    {
      garbage->first = new_point;
      garbage->last = new_point;
    }
  else
    {
      garbage->last->next = new_point;
      garbage->last = new_point;
    }

  ++(garbage->size);
}

/**
 * Redirects pointers for an garbage object to be deallocated.
 *
 * @param record The garbage object record to be deallocated
 */
void redirect_garbage_pointers(object_record_t *record)
{
  link_t **c = find_link(garbage, record);

  if (*c)
    {
      link_t *temp = *c;
      *c = (*c)->next;
      
      if (garbage->size == 1)
        {
          garbage->last = NULL;
          garbage->first = NULL;
        }
      
      --(garbage->size);
      free(temp);
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
      assert(!rc_overflow(object) && "Too many references to the same object. Shutting down application.");
      object_record_t *record = OBJECT_TO_RECORD(object);
      ++(record->ref_count);
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

      if (rc(object) > 0)
        --(record->ref_count);

      if (rc(object) == 0)
        {
          
          if(get_cascade_limit() > released_memory || (ignore_cascade_limit))
            {
              released_memory += record->size; 
              deallocate(object);
            }
          else
            {
              set_garbage_pointers(record);
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

void initialize_garbage()
{
  garbage            = (list_t*)calloc(1, sizeof(list_t));
  link_t *first      = NULL;
  garbage->first = first;
  garbage->last  = first;
  garbage->size = 0;
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
  ignore_cascade_limit = false;
  
  if (!garbage) 
    initialize_garbage(); 

  if (garbage->size > 0)
    {
      cleanup_before_allocation(bytes);
    }

  if (!destructors)
    initialize_destructors();

  object_record_t *record = (object_record_t*) malloc(sizeof(*record) + bytes);

  if (!record)
    return NULL;

  record->ref_count = 0;
  record->destr_index = add_to_destructors(destructor);
  record->size      = sizeof(object_record_t) + bytes;
  return RECORD_TO_OBJECT(record);
}

//TODO: Pekar om fel här om vi inte hinner städa upp allt.
void cleanup_before_allocation(size_t bytes)
{
  assert(garbage);
  released_memory = 0;
  link_t *current = garbage->first;

  while (current && (released_memory <= bytes || get_cascade_limit() > released_memory))
    {
      object_record_t *record = (object_record_t*)current->data;
      current = current->next;
      obj object = RECORD_TO_OBJECT(record);
      released_memory += record->size;
      deallocate(object);
    }

  //  garbage->first = current; 
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

  assert(rc(object) == 0);
  redirect_garbage_pointers(record);
  function1_t destructor = get_destructor(record->destr_index);
  destructor(object);
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
  ignore_cascade_limit = true;
  assert(garbage);

  link_t *remaining_garbage = garbage->first;
  link_t *temp;

  while (remaining_garbage)
    {
      temp = remaining_garbage;
      remaining_garbage = remaining_garbage->next;
      object_record_t *to_cleanup = (object_record_t*)temp->data;
      obj object = RECORD_TO_OBJECT(to_cleanup);
      function1_t fun = get_destructor(to_cleanup->destr_index);
      redirect_garbage_pointers(to_cleanup);
      fun(object);
      free(to_cleanup); 
    }
}

/**
 * Frees all remaining allocated heap objects regardless of reference count
 */
void shutdown()
{
  cleanup(); 
  free(garbage);
  free_list(destructors);
}

