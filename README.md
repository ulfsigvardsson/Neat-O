# Neat-O, A Reference Counting Garbage Collector for the C Programming Language

This is a simple semi-automatic garbage collector for C. Instead of using `malloc()`or `calloc()`, memory is allocated on the heap with `allocate()` and `allocate_array()` respectively. The equivalent function to `strdup()`is `allocate_string()`.

References to object are managed by the programmer by calls to `retain()`and `release()` which increment and decrement the reference count of an object. When the reference count of an object reaches zero, it is eligable for garbage collection and may be freed by the GC. The GC keeps track of data eligable for garbage collecting and frees this data as a side effect when the user allocates new memory.

After allocating an object it must therefore first be *explicitly* retained, else it will be treated as a stack variable and possibly lost when popping the current stack frame.

---

## Functionality

#### `obj allocate (size_t, function1_t)`

The function takes as arguments the size in bytes to allocate and a function pointer to a destruction function to be used when freeing the object. The signature for `function1_t` is `typedef void (*function1_t) (obj)` where `obj` is a void pointer. This function is responsible for returning all allocated memory of the object, including pointers to other objects. If no explicit destructor is required, such as the case when the object is an array of integers, this argument can be `NULL`.


#### `obj allocate_array (size_t, size_t, function1_t)`

Analogous to `calloc()` but with the destructor function passed as its third argument.


#### `void retain (obj)`

Increments the reference count for an object


#### `void release (obj)`

Decrements the reference count for an object


#### `size_t get_cascade_limit()`

Sets the cascade limit, i.e. the number of objects that can be freed in a single invocation of the GC, for example: a linked list with 10 million elements will, if unchecked, be freed by the GC in one go. A high cascade limit may thus cause high latency for the user and a low limit may delay freing of memory.


#### `void deallocate (obj)`

Deallocates an object by passing it to its associated destructor function. Useful when defining recursive destructors, such as for a linked list. The parameter `obj` is the object to deallocate.


#### `rc_format rc(obj)`

Returns the current reference count for an object.


#### `void cleanup()`

Frees all objects who's reference count is 0, regardless of the cascade limit.

## Example Use

```
list_t *list = allocate (sizeof (list_t), list_destructor);
link_t *first = allocate (sizeof (link_t), link_destructor);
first->next = NULL;
list->first = list->last = first;
retain (first);
retain (list);
```

Here, a list is allocated with its destructor passed as an argument. Next an object of type `link_t` is allocated on the heap with its own destructor. This link can now be used within the object `list`. Before the function returns, both objects are retained. A destructor function for `link_t` could look something like this:

```
void
link_destructor (obj o)
{
    link_t *link = (link_t *) o;
    release (link->data);
    return release (link->next);
}
```

``
