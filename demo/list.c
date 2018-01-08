#include "list.h"

typedef struct link link_t;

struct link
{
  elem_t elem;
  link_t *next;
};

struct list
{
  link_t *first;
  link_t *last;
  element_copy_fun copy_f;
  element_free_fun free_f;
  element_comp_fun cmp_f;
  size_t size;
};


elem_t
list_no_copy (elem_t elem)
{
  return elem;
}


/// Creates a new list
///
/// \param copy (may be NULL) a function applied to all elements when stored in the list
/// \param free (may be NULL) used to free elements in list_delete
/// \param compare (may be NULL) used to compare elements in list_contains
/// \returns: empty list
/// TODO: lägg till en standard compare-funktion för listor så vi slipper if-satserna
list_t *
list_new (element_copy_fun copy, element_free_fun free_f,
	  element_comp_fun compare)
{
  list_t *list = (list_t *) allocate_array (1, sizeof (list_t), NULL);
  retain (list);
  list->free_f = free_f;
  list->copy_f = copy ? copy : list_no_copy;
  list->cmp_f = compare;

  return list;
}

link_t *
link_new (elem_t elem, link_t * next)
{
  link_t *link = (link_t *) allocate_array (1, sizeof (link_t), NULL);
  retain (link);
  link->elem = elem;
  link->next = next;
  retain (link->next);
  return link;
}

int
adjust_index_insert (int index, int size)
{
  /// Negative indexes count from the back
  if (index < 0)
    {
      /// Call adjust_index again to handle when
      /// adjusted index is still negative
      return adjust_index_insert (size + index + 1, size);
    }

  /// Index too big, adjust to after last element
  if (index > size)
    {
      return size;
    }

  /// Index OK
  return index;
}

int
adjust_index_get (int index, int size)
{
  /// Negative indexes count from the back
  if (index < 0)
    {
      /// Call adjust_index again to handle when
      /// adjusted index is still negative
      return adjust_index_get (size + index, size);
    }

  /// Index too big, adjust to after last element
  if (index > size)
    {
      return size;
    }

  /// Index OK
  return index;
}

link_t **
list_find (list_t * list, int index)
{
  link_t **c = &(list->first);
  int counter = 0;
  while (*c && index > counter)
    {
      c = &((*c)->next);
      ++counter;
    }
  return c;			// elementet som ska komma efter indexet vi söker
}


void
make_singleton (list_t * list, elem_t elem)
{
  list->first = link_new (elem, NULL);
  list->last = list->first;
}

/// Inserts a new element at a given index.
///
/// If list's copy function is non-NULL, it will be applied to elem and its result
/// stored in the list. Otherwise, elem will be stored in the list.
///
/// All indexes are valid. 0 means first element. Negative indexes
/// count backward and too large negative indexes equal 0. Too
/// large positive indexes are same as -1.
///
/// \param list  pointer to the list
/// \param index the index for elem to be inserted at
/// \param elem  the element to be inserted
/// \returns true if succeeded, else false
void
list_insert (list_t * list, int index, elem_t elem)
{
  index = adjust_index_insert (index, list->size);
  if (list->size == 0)
    {
      make_singleton (list, elem);
    }
  else
    {
      link_t **c = list_find (list, index);
      // För insättning längst bak i listan
      if (index == (int) list->size)
	{
	  link_t *new = link_new (list->copy_f (elem), NULL);
	  (list->last)->next = new;
	  list->last = new;
	}
      // Alla andra fall
      else
	{
	  *c = link_new (list->copy_f (elem), (*c));
	}
    }
  ++(list->size);
}

bool
empty_list (list_t * list)
{

  return (!list->first);
}

/// Inserts a new element at the end of the list.
///
/// If list's copy function is non-NULL, it will be applied to elem and its result
/// stored in the list. Otherwise, elem will be stored in the list.
///
/// \param list pointer to the list
/// \param elem the element to be appended
void
list_append (list_t * list, elem_t elem)
{
  // Denna ser annorlunda ut än list_prepend eftersom index -1 just nu
  // knuffar det sista elementet framför sig och alltså hamnar det nya
  // elementet inte sist i listan.
  if (list->size == 0)
    {
      // Corner case för tom lista
      make_singleton (list, elem);
    }
  else
    {
      link_t *current_last = list->last;
      link_t *new = link_new (elem, NULL);
      current_last->next = new;
      list->last = new;
    }
  ++(list->size);
}

/// Inserts a new element at the beginning of the list
///
/// If list's copy function is non-NULL, it will be applied to elem and its result
/// stored in the list. Otherwise, elem will be stored in the list.
///
/// \param list pointer to the list
/// \param elem the element to be prepended
void
list_prepend (list_t * list, elem_t elem)
{

  list_insert (list, 0, elem);
}

/// Returns the element at a given index
/// \param list  pointer to the list
/// \param index the index to be returned
/// \param result pointer to where the element at index index will be stored if true is returned
/// \returns true if index was a valid index
bool
list_get (list_t * list, int index, elem_t * result)
{
  index = adjust_index_get (index, list->size);
  link_t **c = list_find (list, index);

  if (*c)
    {
      if (index < 0 && abs (index) <= (int) list->size)	// Negativa tal?
	{
	  *result = (*c)->elem;
	  return true;
	}
      else if (abs (index) < (int) list->size)	// Detta ger fel när index är -1 och size är 1
	{
	  *result = (*c)->elem;
	  return true;
	}
    }
  return false;
}

/// A convenience for list_get(list, 0, result)
bool
list_first (list_t * list, elem_t * result)
{
  return list_get (list, 0, result);
}

/// A convenience for list_get(list, -1, result)
bool
list_last (list_t * list, elem_t * result)
{
  if (list->size > 0)
    {
      return list_get (list, -1, result);
    }
  return false;
}

/// Returns the length of the list. It is undefined
/// whether the length is calculated in O(n) time or
/// whether there is a size counter in the list object
/// that is manipulated by insert, remove, etc.
/// \param list the list
/// \returns the length of list
int
list_length (list_t * list)
{
  return list->size;
}

/// Removes an element from a list.
///
/// All indexes are valid. 0 means first element. Negative indexes
/// count backward and too large negative indexes equal 0. Too
/// large positive indexes are same as -1.
///
/// \param list  pointer to the list
/// \param index the index to be removed
/// \param delete if true, run list's free function on all elements
/// \returns true if succeeded, else false
void
list_remove (list_t * list, int index, bool delete)
{
  link_t **c = list_find (list, index);
  link_t *tmp = *c;

  if (*c)			// Kollar att pekare inte är NULL, vi kan köra på tomma listor
    {
      *c = (*c)->next;
      if (delete)
	{
	  list->free_f (tmp->elem);
	}
      release (tmp);
      --(list->size);
    }

}

/// Deletes a list.
///
/// \param list pointer to the list
/// \param delete if true, use list's free function to free elements
void
list_delete (list_t * list, bool delete)
{
  int size = list_length (list);

  for (int i = 0; i < size; ++i)
    {
      list_remove (list, 0, delete);
    }
  release (list);
}

/// Applies a function to all elements in a list in list order
///
/// \param list the list
/// \param fun the function to apply to all elements
/// \param data an extra argument passed to each call to fun (may be NULL)
/// \returns the result of all fun calls, combined with OR (||)
bool
list_apply (list_t * list, elem_apply_fun fun, void *data)
{
  bool result = false;
  link_t **c = &(list->first);
  while (*c)
    {
      if (fun ((*c)->elem, data))
	{
	  result = true;
	}
      c = &((*c)->next);
    }
  return result;
}

/// Searches for an element in a list
///
/// Uses list's compare if non-NULL, otherwise uses == for comparison.
///
/// \param list the list
/// \param elem the element to search for
/// \returns the index of elem in list, or -1 if not found
/// TODO: lägg till en standard compare-funktion för listor så vi slipper if-satserna
int
list_contains (list_t * list, elem_t elem)
{
  int index = -1;
  int counter = 0;
  link_t **c = &(list->first);

  while (*c)
    {
      //Om listan har en cmp-funktion
      if (list->cmp_f)
	{
	  if (list->cmp_f ((*c)->elem, elem) == 0)
	    {
	      return counter;
	    }
	}
      // Om listan inte har en cmp-funktion
      else
	{
	  if ((*c)->elem.i == elem.i)	// Ful-lösning, hur jämföra en union med '=='?
	    {
	      return counter;
	    }
	}

      c = &((*c)->next);
      ++counter;
    }
  return index;
}
