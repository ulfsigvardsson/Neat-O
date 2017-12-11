#include "refmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct foo{
  char *name;
} foo_t;

void foo_destructor(obj o)
{
  foo_t *foo = (foo_t*)o;
  release(foo->name); 
}

foo_t *new_foo(char* id)
{
  foo_t *foo = allocate(sizeof(foo_t), foo_destructor);
  foo->name = strdup2(id); 
  retain(foo);
  return foo;
}

int main(int argc, char *argv[])
{
  foo_t *foo = new_foo("Foo");
  printf("Adress of foo: %p\n", foo);
  printf("Adress of foo->name: %p\n", &foo->name);
  release(foo);

  return 0;
}

