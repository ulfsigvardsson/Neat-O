#include "refmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct link
{
  char *data;
  struct link *next;
} link_t;

typedef struct list
{
  link_t *first;
  link_t *last;
} list_t;

typedef struct foo
{
  char *name;			//8 bytes
} foo_t;

void
link_destructor (obj o)
{
  link_t *link = (link_t *) o;
  release (link->data);
  return release (link->next);
}

void
list_destructor (obj o)
{
  list_t *list = (list_t *) o;
  return release (list->first);
}

void
test_list ()
{
  list_t *list = allocate (sizeof (list_t), list_destructor);
  link_t *first = allocate (sizeof (link_t), link_destructor);
  first->next = NULL;
  list->first = list->last = first;
  retain (first);
  list->first->data = allocate_string ("Test"); 

  link_t *current = first;

  for (size_t i = 0; i < 10; i++)
    {
      link_t *link = allocate (sizeof (link_t), link_destructor);
      retain (link);
      link->data = allocate_string ("Test"); 
      link->next = NULL;
      current->next = link;
      current = link;
      list->last = link;
    }
  retain (list);
  release (list);

}


int *
test_array ()
{
  int *array = (int *) allocate_array (70000, sizeof (int *), NULL);
  retain (array);
  for (int i = 0; i < 70000; i++)
    {
      array[i] = i;
    }
  return array;
}

int
main (int argc, char *argv[])
{
  printf ("innan test_list\n");
  test_list ();
  printf ("efter test_list\n");


  int *arr = test_array();

  for (int i = 0; i < 1; i++)
    {
      printf ("iteration nr: %d\n", i + 1);
      char *temp = allocate_string ("foo");
      release (temp);
    }
   release(arr);
   printf ("Innan cleanup.\n");
  cleanup ();

  shutdown ();


  return 0;
}
