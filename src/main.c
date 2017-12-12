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

foo_t *new_foo(char *id)
{
  foo_t *foo = allocate(sizeof(foo_t), foo_destructor);
  foo->name = strdup2(id); 
  retain(foo);
  return foo;
}

int main(int argc, char *argv[])
{
  foo_t *foo = new_foo("Foo");
  retain(foo);
  printf("Namn på foo: %s\n", foo->name);
  retain(foo);
  release(foo);
  release(foo);
  release(foo);
  char *test = strdup2("test");
  printf("Namn på test: %s\n", test);
  release(test);

  return 0;
}

