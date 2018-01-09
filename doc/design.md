# Design Documentation
Here follows an introductionary description of the design and functionality of the reference counting garbage collector implemented
by the project group Gulhämplingarna.

# Table of Content
* [Overview](#overview)
    - [Global variables](#global)
    - [Object_record](#obj-meta)
	- [Allocating](#retain-exp)
	- [Release & retain](#retain)
	- [Deallocate](#deallocate)
	- [Cascade limit](#cascade)
	- [Cleanup](#cleanup)
	- [Shutdown](#shutdown)

## Overview <a name = "overview"></a>
This text describes the usage and implementation of a rudimentary memory management tool using an reference counting method similar to the one implemented for the language of Java.

## Global variables <a name = "global"></a>
The library depend on two globally declared linked lists, `garbage` and `destruct_list`. One for holding our pointers to objects remaining to be deallocated due to cascade limit been reached while deallocated been called on the object, and the other holding destructor functions for allocated objects. Destructors are added to this list when a object is allocated if not present already.

## Objects and Metadata <a name = "obj-meta"></a>
Due to the method we decided upon we are in need to keep track of references to our objects, this is not something you can do by default in C. Therefore we have decided to create a structure containing metadata consisting of the objects size, reference count and an index to the object's destructor function in the global list.

## Allocating <a name = "allocate"></a>
To allocate an object one calls `allocate()` with the amount of bytes needed to be allocated along with a pointer to a destructor function as arguments.
`allocate()` will initialize the metadata for the object and return a pointer to the data with metadata hidden behind it in memory.

By default the reference count has a value of zero when initialized. The amount of metadata allocated is 6 bytes per allocation. The library allocates some space for its internal structures but assuming that no allocations below 8 bytes are made, the library carries a maximal overhead less than twice the actual data allocated as the number of allocations grow.

## Release & retain <a name = "retain"></a>
`release` and `retain` are used to decrease and increase an object's reference counter.
If an object’s reference counter hits zero by calling release, the object is automatically freed and removed from memory. If the cascade limit has already been reached, the object is added to the list of outstanding frees.

Since the data type holding an objects reference count is an unsigned short, the maximum number of references to an object is 65.535. Overstepping this limit will crash the program since it is not reasonable to continue as the overflow will result in a zero reference count.

## Deallocate <a name = "deallocate"></a>
If an object qualify for deallocation it is, if needed, removed from the list of dangling garbage, its destructor function is retrieved from the global list and called upon the object. A call to `deallocate()` with an object with a non-zero reference count results in a crash.

## Cascade limit <a name = "cascade"></a>
Usually, an object is removed as soon as it’s reference counter hits zero, but to not halt the entire program if a very large number of objects where to be deallocated at once, e.g. a long linked list, a cascade limit is used to restrict how many objects that can be deallocated at once.

The cascade limit is measured in bytes in order to get a more precise approximation of time spent freeing an object and additionally to ensure a new allocation can be made by freeing any outstanding garbage before an allocation.

The default value of cascade limit is initialized with 800 000 bytes, this results in a reasonable latency for an interactive program. This can be modified by calling set_cascade_limit(). Note that stack overflow may occur with too high or low cascade limits, as `deallocate()` will recurse over the same destructor function for a linked list or similar. A low enough cascade limit may result in a stack overflow at `cleanup()` when all remaining garbage is to be freed.

## Cleanup before allocation <a name = "clean_before"></a>
To ensure that every object gets deleted, unfreed garbage is deallocated with every new allocation as long as garbage remains and either the cascade limit has not been overstepped or the amount of freed memory is less than what is needed for the allocation.

## Garbage and destruct_list <a name = "garbage-dest"></a>
In order to deal with garbage remaining after a deallocation, all garbage objects (objects with a rc of zero) is inserted into a list. Thus, when `deallocate()` is called upon it keeps deallocating garbage from the list until the cascade limit is reached or the list is empty.

To decrease the size of the metadata overhead, the object’s destructor function is not stored in the struct itself but rather as an index to a link in the list of destructors.


## Cleanup <a name ="cleanup"></a>
The `cleanup()` function runs the corresponding destructor function on all outstandning freed regardless of the cascade limit and in turn any objects becoming garbage as a result of the deallocation. 

## Shutdown <a name ="shutdown"></a>
`shutdown()` runs `cleanup()` and releases any library specific allocation, i.e. `garbage` and `destruct_list`. It does not, however, clean up any unreleased allocation by the client.
