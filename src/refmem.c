#include "refmem.h"
#include <stdio.h>

/* =================================
 * -------------MACROS--------------
 * =================================
 */

/**
 * @def ALLOCATION_SPACE_REQUIRED
 * Determines if the amount of bytes for a pending allocation is greater that the amount of deallocated space.
 */
#define ALLOCATION_SPACE_REQUIRED released_memory < bytes

/**
 * @def BELOW_CASCADE_LIMIT
 * Determines if the cascade limit has been reached.
 */
#define BELOW_CASCADE_LIMIT get_cascade_limit() > released_memory

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
 * @def INITAIL_CASCADE_LIMIT
 * Global constant for the default value of the cascade limit.
 */
#define INITIAL_CASCADE_LIMIT 150000

/* =================================
 * --------TYPE DEFINITIONS---------
 * =================================
 */

typedef struct object_record object_record_t;	/*!< @typedef object_record_t  */
typedef struct link link_t;	/*!< @typedef link__t  */
typedef struct list list_t;	/*!< @typedef list_t  */

/* =================================
 * --------STATIC VARIABLES---------
 * =================================
 */

static size_t cascade_limit =
    INITIAL_CASCADE_LIMIT;	/*!< The cascade limit with a default value.  */
static size_t released_memory =
    0;	/*!< Global variable tracking number of released bytes of a given cycle.  */
static list_t *garbage =
    NULL;	/*!< List containing all dangling garbage objects. */
static list_t *destruct_list =
    NULL;	/*!< List containing all destructor functions.  */
static bool ignore_cascade_limit =
    false;	/*!< Global bool variable for bypassing the cascade limit in @see cleanup.  */

/* =================================
 * -------STRUCTS & PROTOTYPES------
 * =================================
 */

void no_destructor (obj object);
void cleanup_before_allocation (size_t bytes);

/**
 * @struct link
 * The link struct represents a link in a linked list.
 */
struct link
{
    obj data;
    link_t *next;
};

/**
 * @struct list
 * The list struct is a standard linked list composed of @see link_t links.
 */
struct list
{
    link_t *first;
    link_t *last;
    size_t size;
};

/**
 * @struct object_record
 * The object_record struct contains meta data consisting of the current amount of references
 * to a heap object along with an index number corresponding to a destructor function and the amount
 * of bytes occupied by the data object.
 * in the @see destruct_list list.
 */
struct object_record
{
    size_format
    size;		/*!< Number of bytes occupied by the object including meta data.  */
    rc_format ref_count;		/*!< Reference count for the object.  */
    unsigned char destr_index;	/*!< Index to a destructor function.  */
};

/* =================================
 * -----------FUNCTIONS------
 * =================================
 */

/**
 * Frees a links next pointer and then the link itself.
 * @warning this function does not free the @see data struct member.
 *
 * @param link The link to free.
 */
void
free_link (link_t * link)
{
    if (link)
        {
            free_link (link->next);
            free (link);
        }
}

/**
 * Frees a lists first pointer and then the list itself.
 * @warning this function does not free the @see data struct member.
 *
 * @param link The link to free.
 */
void
free_list (list_t * list)
{
    if (list)
        {
            free_link (list->first);
            free (list);
        }
}

/**
 * Finds the destructor function positioned at a given index in @see destruct_list.
 *
 * @param index Index to retrieve.
 * @return function1_t The destructor function at the given index.
 */
function1_t
get_destructor (unsigned char index)
{
    link_t *result = destruct_list->first;

    for (unsigned char i = 0; i < index; i++)
        result = result->next;

    return (function1_t) result->data;
}

/**
 * Finds and returns a double pointer to the link containing a given element.
 *
 * @param list A list to search.
 * @param data A pointer to locate.
 * @return link_t** A double pointer to link containing data, if present, otherwise NULL.
 */
link_t **
find_link (list_t * list, obj data)
{
    link_t **current = & (list->first);

    while (*current)
        {
            if ((*current)->data == data)
                return current;

            current = & (*current)->next;
        }

    return current;
}

/**
 * Adds a destructor function to @see destruct_list if not already present
 * and returns its index in the list.
 *
 * @param destructor A destructor function.
 * @return unsigned char Index of destructor in list.
 */
unsigned char
add_to_destructors (function1_t destructor)
{
    link_t **current = & (destruct_list->first);
    int index = 0;

    if (!destructor)
        return index;

    while (*current)
        {
            if ((*current)->data == destructor)
                return index;

            ++index;
            current = & (*current)->next;
        }

    *current = (link_t *) calloc (1, sizeof (link_t));
    (*current)->data = (function1_t *) destructor;
    destruct_list->last = *current;
    ++ (destruct_list->size);

    return index;
}

/**
 * Initializes @see destruct_list to contain @see no_destructor as its only element.
 */
void
initialize_destructors ()
{
    destruct_list = (list_t *) calloc (1, sizeof (list_t));
    link_t *first = (link_t *) calloc (1, sizeof (link_t));

    first->data = (obj) no_destructor;
    destruct_list->first = first;
    destruct_list->last = first;
    destruct_list->size = 1;
}

/**
 * Determines if the reference count for an object has reached its maximum value.
 *
 * @param object The object to control the reference count for.
 * @return bool true if rc(object) is at its maximum, otherwise false.
 */
bool
rc_overflow (obj object)
{
    return rc (object) == USHRT_MAX;
}

/**
 * Adds a new object to @see garbage and sets the 'first' and 'last'
 * list pointers as needed.
 *
 * @param record An object record to add.
 */
void
set_garbage_pointers (object_record_t * record)
{
    link_t *new_point = (link_t *) calloc (1, sizeof (link_t));
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

    ++ (garbage->size);
}

/**
 * Redirects pointers for an garbage object to be deallocated.
 *
 * @param record The garbage object record to be deallocated.
 */
void
redirect_garbage_pointers (object_record_t * record)
{
    link_t **current = find_link (garbage, record);

    if (*current)
        {
            link_t *temp = *current;
            *current = (*current)->next;

            if (garbage->size == 1)
                {
                    garbage->last = NULL;
                    garbage->first = NULL;
                }

            -- (garbage->size);
            free (temp);
        }
}

/**
 * Increments an objects reference count by one.
 * @warning Overflow of the reference count causes the program to terminate.
 *
 * @param o The object for wich to increment the reference count.
 */
void
retain (obj object)
{
    if (object)
        {
            assert (!rc_overflow (object)
                    &&
                    "Too many references to the same object. Shutting down application.");
            object_record_t *record = OBJECT_TO_RECORD (object);
            ++ (record->ref_count);
        }
}

/**
 * Decrements an objects reference count by one. If this results in
 * the reference count hitting 0 the object will be deallocated
 * or added to garbage.
 *
 * @param o The object for wich to decrement the reference count.
 */
void
release (obj object)
{
    if (object)
        {
            object_record_t *record = OBJECT_TO_RECORD (object);

            if (rc (object) > 0)
                -- (record->ref_count);

            if (rc (object) == 0)
                {

                    if (get_cascade_limit () > released_memory || ignore_cascade_limit)
                        {
                            released_memory += record->size;
                            deallocate (object);
                        }
                    else
                        {
                            set_garbage_pointers (record);
                        }
                }
        }
}

/**
 * Returns the current reference count of an object.
 *
 * @param o The object for wich to return the reference count.
 * @return rc_format The current reference count for the object.
 */
rc_format
rc (obj object)
{
    assert (object);
    object_record_t *record = OBJECT_TO_RECORD (object);
    return record->ref_count;
}

/**
 * Default destructor function for use when allocate is called with NULL
 * as its destructor argument. This function is used for strings since they are simply
 * an array of chars adjacent to the object record and need no explicit free.
 *
 * @param o Data object to (not) be freed.
 */
void
no_destructor (obj object)
{
    return;
}

/**
 * Initializes @see garbage to to an empty list.
 */
void
initialize_garbage ()
{
    garbage = (list_t *) calloc (1, sizeof (list_t));
    garbage->first = garbage->last = NULL;
    garbage->size = 0;
}

/**
 * Allocates space for an object on the heap together with additional meta data
 * consisting of reference count and a pointer to a destructor function.
 * Note that only the allocation of the actual data is made, no assumptions to what it
 * may contain are made.
 *
 * @param bytes The size of the object to allocate.
 * @param destructor Function pointer to the destructor.
 *
 * @return obj Pointer to the data allocated by the client.
 */
obj
allocate (size_t bytes, function1_t destructor)
{
    ignore_cascade_limit = false;
    bytes = abs (bytes);

    if (!garbage)
        initialize_garbage ();

    if (garbage->size > 0)
        {
            cleanup_before_allocation (bytes);
        }

    if (!destruct_list)
        initialize_destructors ();

    object_record_t *record =
        (object_record_t *) malloc (sizeof (object_record_t) + bytes);

    if (!record)
        return NULL;

    record->ref_count = 0;
    record->destr_index = add_to_destructors (destructor);
    record->size = sizeof (object_record_t) + bytes;

    return RECORD_TO_OBJECT (record);
}

/**
 * Only made for unit testing purposes.
 * Basically an allocate where you can decide the starting amount of references.
 */
obj
constructor_allocate_tester(size_t bytes, function1_t destructor, rc_format refc)
{
    ignore_cascade_limit = false;

    if (!garbage)
        initialize_garbage ();

    if (garbage->size > 0)
        {
            cleanup_before_allocation (bytes);
        }

    if (!destruct_list)
        initialize_destructors ();

    object_record_t *record =
        (object_record_t *) malloc (sizeof (object_record_t) + bytes);

    if (!record)
        return NULL;

    record->ref_count = refc;
    record->destr_index = add_to_destructors (destructor);
    record->size = sizeof (object_record_t) + bytes;

    return RECORD_TO_OBJECT (record);
}

/**
 * Deallocates remaining garbage prior to an allocation while below the cascade limit
 * or the number of bytes freed is lesser than the number of bytes to be allocated.
 *
 * @param bytes The size of the pending allocation.
 */
void
cleanup_before_allocation (size_t bytes)
{
    assert (garbage);
    released_memory = 0;
    link_t *current = garbage->first;

    while (current && (ALLOCATION_SPACE_REQUIRED || BELOW_CASCADE_LIMIT))
        {
            object_record_t *record = (object_record_t *) current->data;
            current = current->next;
            obj object = RECORD_TO_OBJECT (record);
            released_memory += record->size;
            deallocate (object);
        }
}

/**
 * Allocates space for an array an initializes each byte to 0. Equivalent to 'calloc'.
 *
 * @param elements The number of elements in the array.
 * @param elem_size The size of each array element.
 * @param destructor Function pointer to the destructor.
 *
 * @return obj Pointer to the first element of the array.
 */
obj
allocate_array (size_t elements, size_t elem_size, function1_t destructor)
{
    size_t total = elements * elem_size;
    obj object = allocate (total, destructor);

    if (!object)
        return NULL;

    return memset (object, 0, total);
}

/**
 * Own implementation of 'strdup'. Allocates space for a string and stores a string there.
 *
 * @param org The string to allocate.
 * @return char* Pointer to the allocated string.
 */
char *
allocate_string (char *org)
{
    char *result;
    int org_size = strlen (org);
    int buf_size = (org_size + 1) * sizeof (char);

    result = (char *) allocate (buf_size, NULL);

    if (!result)
        return NULL;

    strcpy (result, org);
    retain (result);

    return result;
}

/**
 * Deallocates an object by passing it to its associated destructor function.
 *
 * @param o The object to deallocate.
 * @warning Passing NULL to the function causes an error, so does an object with a non-zero reference count.
 */
void
deallocate (obj object)
{
    assert (object);

    object_record_t *record = OBJECT_TO_RECORD (object);

    assert (rc (object) == 0);
    redirect_garbage_pointers (record);	//
    function1_t destructor = get_destructor (record->destr_index);
    destructor (object);
    free (record);

}

/**
 * Modifies the cascade limit.
 *
 * @param limit The new cascade limit.
 */
void
set_cascade_limit (size_t limit)
{
    if ((int) limit < 0)
        cascade_limit = 0;
    else
        cascade_limit = limit;
}

/**
 * Returns the current cascade limit.
 *
 * @return size_t The current cascade limit.
 */
size_t
get_cascade_limit ()
{
    return cascade_limit;
}

/**
 * Frees all object whos reference count is 0, regardless of cascade limit.
 */
void
cleanup ()
{
    ignore_cascade_limit = true;
    assert (garbage);

    link_t *remaining_garbage = garbage->first;

    while (remaining_garbage)
        {
            object_record_t *to_cleanup =
                (object_record_t *) remaining_garbage->data;
            remaining_garbage = remaining_garbage->next;
            obj object = RECORD_TO_OBJECT (to_cleanup);
            function1_t destructor = get_destructor (to_cleanup->destr_index);

            redirect_garbage_pointers (to_cleanup);
            destructor (object);
            free (to_cleanup);
        }
}

/**
 * Frees all remaining garbage objects and library specifik allocations.
 */
void
shutdown ()
{
    cleanup ();
    free (garbage);
    free_list (destruct_list);
}

