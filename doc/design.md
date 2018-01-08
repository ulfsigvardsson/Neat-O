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

## Release <a name = "release-exp"></a>

## Rc <a name = "rc-exp"></a>

## Allocate <a name = "allocate-exp"></a>

## Allocate_array <a name = "allocate_array-exp"></a>

## Deallocate <a name = "deallocate-exp"></a>

## Set_cascade_limit <a name = "set_casc-exp"></a>

## Get_cascade_limit <a name = "get_casc-exp"></a>

## Cleanup <a name = "cleanup-exp"></a>

## shutdown <a name = "shutdown-exp"></a>

## strdup2 <a name = "strdup2-exp"></a>

