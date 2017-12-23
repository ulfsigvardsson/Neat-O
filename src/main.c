#include "refmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct link
{
  obj data;
  struct link *next;
}link_t;

typedef struct list
{
  link_t *first;
  link_t *last;
}list_t;

typedef struct foo{
  char *name; //8 bytes
} foo_t;

void link_destructor(obj o)
{
  link_t *link = (link_t*)o;
  release(link->next);
  release(link->data);
}

void list_destructor(obj o)
{
  list_t *list = (list_t*)o;
  release(list->first);
}

void test_list()
{
  list_t *list  = allocate(sizeof(list_t), list_destructor);
  link_t *first = allocate(sizeof(link_t), link_destructor);
  list->first = list->last = first;
  
  list->first->data = allocate(sizeof(char*), NULL);
  retain(list->first->data);
  retain(first);

  link_t *current = first;

  for (size_t i = 0; i < 30000; i++) {
    link_t *link = allocate(sizeof(link_t), link_destructor);
    link->data = allocate(sizeof(char*), NULL);
    retain(link->data);
    retain(link);
    current->next = link;
    current = link;
  }
  
  retain(list);
  release(list);
}

void foo_destructor(obj o)
{
  
  foo_t *foo = (foo_t*)o;
  printf("Record count för strängen: ");
  release(foo->name);
}

  foo_t *new_foo(char *name)
  {
  foo_t *foo = allocate(sizeof(foo_t), foo_destructor);
  foo->name = name;
  retain(foo->name);
  retain(foo);
  return foo;
}

foo_t *test_foo(char *name)
{
  foo_t *foo = new_foo(name);
  return foo;
}

char** test_array()
{
  char **array = (char**)allocate_array(2, sizeof(char*), NULL);
  retain(array);
  array[0] = "Foo";
  array[1] = "Bar";
  for (int i= 0; i < 2; i++) {
    printf("Array[%d]: %s\n", i, array[i]);
  } 
  return array;
}

int main(int argc, char *argv[])
{
  printf("Innan test_list\n");
  test_list();
  printf("Efter test_list\n");
  
  for (int i = 0; i < 100; i++) {
    printf("Iteration nr: %d\n", i+1);
    char * temp = strdup2("foo");
    release(temp);
  }

  char *bajs = strdup2("bajs");
  foo_t *foo = test_foo(bajs);
  char *bajs3 = bajs;
  retain(bajs3);
  release(foo);
  release(bajs);
  char *bajs2 = strdup2("bajs");
  release(bajs2);
  
  printf("Innan cleanup.\n");
  cleanup();
  shutdown();

  return 0;
}

