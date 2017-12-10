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
  free(foo->name);
}

foo_t *bajs()
{
  foo_t *foo = allocate(sizeof(foo_t), foo_destructor);
  return foo;
}

int main(int argc, char *argv[])
{
  foo_t *foo = bajs();
  foo->name =strdup("Foo"); 
  printf("%zd\n", rc(foo)); 
  retain(foo);
  printf("%zd\n", rc(foo)); 
  retain(foo);
  printf("%zd\n", rc(foo)); 
  release(foo);
  printf("%zd\n", rc(foo)); 
  printf("Cascade limit: %zd\n", get_cascade_limit());
  set_cascade_limit(2000);
  printf("Cascade limit: %zd\n", get_cascade_limit());
  printf("Deallokerar foo...\n");
  release(foo);
  return 0;
}

