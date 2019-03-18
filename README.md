# Lumos, A Reference Counting Garbage Collector for the C Programming Language

This is a simple semi-automatic garbage collector for C. Instead of using `malloc()`or `calloc()`, memory is allocated on the heap with `allocate` and `allocate_array` respectively. The equivalent function to `strdup()`is `allocate_string()`.

References to object are managed by the programmer by calls to `retain`and `release` which increment and decrement the reference count of an object. When the reference count of an object reaches zero, it is eligable for garbage collection and may be freed by the GC. The GC keeps track of data eligable for garbage collecting and frees this data as a side effect when allocating new memory.

After allocating an object it must therefore first be explicitly retained else it will be treated as a stack variable and possibly lost when popping the current stack frame.

---

## Usage

#### `obj allocate (size_t, function1_t)`

The function takes as arguments the size in bytes to allocate and a function pointer to a destruction function to be used when freeing the object. The signature for `function1_t` is `typedef void (*function1_t) (obj)` where `obj` is a void pointer. This function is responsible for returning all allocated memory of the object, including pointers to other objects.


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
