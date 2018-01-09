# Design Documentation
Here follows an introductionary description of the design and functionality of the reference counting garbage collector implemented
by the project group Gulhämplingarna.

# Table of Content
* [Useful Info](#Uinfo)
    - [Format explanations](#format)
    - [Type Definitions](#typedef)
    - [Function Prototypes](#func-prot)
* [Usage of Functions](#use-func")
	- [Retain](#retain)
	- [Release](#release)
	- [Rc](#rc)
	- [Allocate](#allocate)
	- [Allocate_array](#allocate_array)
	- [Deallocate](#deallocate)
	- [Set_cascade_limit](#set_casc)
	- [Get_cascade_limit](#get_casc)
	- [Cleanup](#cleanup)
	- [Shurdown](#shutdown)
	- [Strdup2](#strdup2)
* [Indepth explanation of Functions](#exp-func)
	- [Retain](#retain-exp)
	- [Release](#release-exp)
	- [Rc](#rc-exp)
	- [Allocate](#allocate-exp)
	- [Allocate_array](#allocate_array-exp)
	- [Deallocate](#deallocate-exp)
	- [Set_cascade_limit](#set_casc-exp)
	- [Get_cascade_limit](#get_casc-exp)
	- [Cleanup](#cleanup-exp)
	- [Shurdown](#shutdown-exp)
	- [Strdup2](#strdup2-exp)

# Useful Info <a name = "Uinfo"></a>
## Format explanations <a name = "format"></a>
### obj
### function1_t
### rc_format
### size_format

## Type Definitions <a name = "typedef"></a>
```c
 typedef void *obj;
 typedef void(*function1_t)(obj);
 typedef unsigned short rc_format;
 typedef unsigned short size_format;
```
## Function Prototypes <a name = "func-prot"></a>
```c
 void retain(obj);
 void release(obj);
 rc_format rc(obj);
 obj allocate(size_t, function1_t);
 obj allocate_array(size_t, size_t, function1_t);
 void deallocate(obj);
 void set_cascade_limit(size_t);
 size_t get_cascade_limit();
 void cleanup();
 void shutdown();
 char *strdup2(char*);
```

# Usage of Functions <a name = "use-func"></a>
## Retain <a name = "retain"></a>
```c
void retain(obj);
```
### Usage of retain
Retain given an object is used in order to increase the objects reference count by one.
Be aware of Overflow of the reference count if this happens it will cause the program to terminate.

## Release <a name = "release"></a>
```c
void release(obj);
```
### Usage of release
Release given an object is used to decrease the objects reference count by one.
If this results in the objects reference count hitting 0 the object will depending on the cascade limit either be deallocated or be put in garbage and wait until the program has resources to deallocate the object.

## Rc <a name = "rc"></a>
```c
rc_format rc(obj);
```
### Usage of rc
Reference count is used to keep track of references to a specific object.
The use of rc on an object returns the amount of references to that object in the format rc_format.

## Allocate <a name = "allocate"></a>
```c
obj allocate(size_t, function1_t );
```
### Usage of allocate
Allocate given a size of the object and an function pointer to a destructor function, allocates space on the heap together with additional metadata consisting of reference count and a pointer to an destructor function.
And after this is done returns the obj pointer to the allocated space.

## Allocate_array <a name = "allocate_array"></a>
```c
obj allocate_array(size_t, size_t, function1_t);
```
### Usage of allocate_array
Allocate_array given an amount of elements, the size of each array element and a destructor pointer to the destructor.
Will allocate the desired amount and initialize each byte with 0, which is equivilant to 'calloc'.

returns a obj pointer to the first element of the array.

## Deallocate <a name = "deallocate"></a>
```c
void deallocate(obj);
```
### Usage of deallocate
Deallocate given an object will deallocate the object by passing it through it's associated destructor function.
Calling Deallocate with a null reference will cause an error, so will calling it with a object with a non-zero reference count.

## Set_cascade_limit <a name = "set_casc"></a>
```c
void set_cascade_limit(size_t);
```
### Usage of set_cascade_limit
Set_cascade_limit given a size will uppdate the current cascade_limit to the given size.

## Get_cascade_limit <a name = "get_casc"></a>
```c
size_t get_cascade_limit();
```
### Usage of get_cascade_limit
Get_cascade_limit returns the current cascade_limit

## Cleanup <a name = "cleanup"></a>
```c
void cleanup();
```
### Usage of cleanup
A call to cleanup will deallocate the remaining objects with a reference count equal to zero and doing so while ignoring the current cascade_limit.

## shutdown <a name = "shutdown"></a>
```c
void shutdown();
```
### Usage of shutdown
A call to shutdown will cleanup the remaining garbage and then continue to free the allocations made by refmem. Shutting down the program without leaks from refmems side.

## strdup2 <a name = "strdup2"></a>
```c
char* strdup2(char*);
```
### Usage of strdup2
Same functions as the original strdup but uses our memory handler in order to allocate the desired obj.

# Indept explanation of Functions <a name = "exp-func"></a>

## Retain <a name = "retain-exp"></a>
The idea with retain is to let our program know the user is using this specific object x amount of times. It increases the object_records reference count by one for each call.
## Release <a name = "release-exp"></a>
Releaseing an object is depending on the object_records reference count & the cascade limit. If the object_record reference count is non-zero after decreasing the reference count by one release will not deallocate the object.
However if the object reference count hits zero and still have bytes left to free / ignore_cascade_limit has the value true, our released_memory will increase by the object_records size and our object will be deallocated.
However if the cascade_limit is reached our object will be moved to our list of garbage!
## Rc <a name = "rc-exp"></a>
Rc returns a objects reference count, the idea by using a reference count is for us hold the information of how many times the client are in use of the object in our spectrum.
Beware that our reference count can reach it's upper limit or lower limit and cause a overflow and throw out and terminate + give exception!

## Allocate <a name = "allocate-exp"></a>
Allocate are in charge of allocating a specific size of bytes on our heap.

The allocation process starts by initializing a garbage list if there currently is none. If there already is one allocated with a size larger than 0 meaning we have some pointers to objects whoms reference count is zero, we are deallocating these objects before continuing with our current allocation.

Later the initialization of our destructors are done and lastly the record is initialized.

## Allocate_array <a name = "allocate_array-exp"></a>
Does the same as Allocate - see Allocate.

The addition to allocate it initializes each byte with a value of 0, the equivilant to 'Calloc'

## Deallocate <a name = "deallocate-exp"></a>
Deallocation is the process of telling the system our program no longer need this allocation the function assures the object to be deallocated has a reference count equal to zero and is not a null pointer.

Since the deallocation is done to garbage it redirects the garbage_pointers and later on gets the objects destructor function and destroys the object using the destructor given during the allocation.

## Set_cascade_limit <a name = "set_casc-exp"></a>
Setting the cascade limit is done by the client in order to optimize the effectivness of our memory handling. Cascade limit is initilized at the value of XXX

## Get_cascade_limit <a name = "get_casc-exp"></a>

## Cleanup <a name = "cleanup-exp"></a>

## shutdown <a name = "shutdown-exp"></a>

## strdup2 <a name = "strdup2-exp"></a>

