#include "refmem.h"
#include <stdio.h>

typedef struct foo{
  char *name;
} foo_t;

void foo_destructor(obj o)
{
  return; 
}

int main(int argc, char *argv[])
{
  printf("Size of size_t: %zd\n", sizeof(size_t));
  printf("Size of void*: %zd\n", sizeof(void*));
  foo_t *foo = allocate(sizeof(foo_t), foo_destructor);
  foo->name ="Foo";
  printf("%s\n", foo->name);
  printf("%zd\n", rc(foo)); 
  retain(foo);
  printf("%zd\n", rc(foo)); 
  retain(foo);
  printf("%zd\n", rc(foo)); 
  release(foo);
  printf("%zd\n", rc(foo)); 
  release(foo);
  
  return 0;
}
