#pragma once

#include <stddef.h>


typedef void *obj;
typedef void(*function1_t)(obj);

void retain(obj);
void release(obj);
size_t rc(obj);
obj allocate(size_t, function1_t);
obj allocate_array(size_t, size_t, function1_t);
void deallocate(obj);
void set_cascade_limit(size_t);
size_t get_cascade_limit();
void cleanup();
void shutdown();
char *strdup2(char*);
