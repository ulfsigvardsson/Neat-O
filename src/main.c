#include "refmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct foo{
    char *name; //8 bytes
} foo_t;

void list_destructor(obj o)
{
    if (get_cascade_limit() > 0)
    {
        //free next
    }

}

void foo_destructor(obj o)
{
    foo_t *foo = (foo_t*)o;
    release(foo->name);
}

foo_t *new_foo(char *name)
{
    foo_t *foo = (foo_t*) allocate(sizeof(foo_t), foo_destructor);
    foo->name = strdup2(name);
    retain(foo);
    return foo;
}

void test_foo(char *name)
{
    foo_t *foo = new_foo(name);
    //release(foo);
}

void test_array()
{
    char *array = (char*) allocate_array(2, sizeof(char), NULL);
    retain(array);
    array[0] = 'a';
    release(array);
}

int main(int argc, char *argv[])
{
    char *test2 = strdup2("bajs");
    char *test =strdup2("test");
    test_foo("Foo");
    char *hej = strdup2("hejsan!");
    shutdown();
    return 0;
}

