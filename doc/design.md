# Design Documentation
Here follows an introductionary description of the design and functionality of the reference counting garbage collector implemented
by the project group Gulhämplingarna.

# Table of Content
* [Useful Info](#Uinfo)
    - [Typedef & Format Explanations](#format)
    - [Object_record](#obj-rec)
* [Explanation of Functions](#exp-func)
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

## Type Definitions <a name = "typedef"></a>

## Global variables
### cascade_limit
Holds the amount of allowed bytes to be released per cycle in a format of size_t.
### released_memory
Keep track of the released bytes per given cycle.
### garbage
Garbage is a list containing all remaining garbage objects in need of deallocation.
### destruct_list
Destruct_list is a list containing all the destructor functions in use by the program.
### ignore_cascade_limit
Boolean value in order to allow cleanup to ignore the cascade_limit and free every remaining bit of garbage in one single cycle.

## Typedef & Format Explanations <a name = "format"></a>
```c
 typedef void(*function1_t)(obj);
 typedef unsigned short rc_format;
 typedef unsigned short size_format;
```
__function1_t__ is the way we define pointers to functions and in our case it's used for destructors.  
__rc_format__ is the format of which we keep track of all references to a specific object.
This format is the same as saying Unsigned Int however it's easier to read the code when given a typedef.  
__size_format__ is the format of which we keep track of the amount of bytes. The format is the same as saying Unsigned Int however the readability of our code increases with this typedef.  

## Object_record <a name="obj-rec"></a>
Our object_record holds the valuble information needed for our program to function correctly.
It holds the meta data consisting of the current amount of references to a heap object along with an index number corresponding to a destructor function and the amount of bytes occupied by the allocated object.
```c
struct object_record
{
    size_format size;
    rc_format ref_count;
    unsigned char destr_index;
};
```
The structure object_record will be refered to in the code using the alias object_record_t.

# Explanation of Functions <a name = "exp-func"></a>

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

