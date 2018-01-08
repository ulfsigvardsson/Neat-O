# Design Documentation
Here follows an introductionary description of the design and functionality of the reference counting garbage collector implemented
by the project group Gulhämplingarna.

# Table of Content
1. [Useful Info](#Uinfo)
    - [Format explanations](#format)
    - [Type Definitions](#typedef)
    - [Function Prototypes](#func-prot)
2. [Usage of Functions](#use-func")
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
3. [Indepth explanation of Functions](#exp-func)
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

## Allocate_array <a name = "allocate_array"></a>
```c
obj allocate_array(size_t, size_t, function1_t);
```
### Usage of allocate_array


## Deallocate <a name = "deallocate"></a>
```c
void deallocate(obj);
```
### Usage of deallocate

## Set_cascade_limit <a name = "set_casc"></a>
```c
void set_cascade_limit(size_t);
```
### Usage of set_cascade_limit

## Get_cascade_limit <a name = "get_casc"></a>
```c
size_t get_cascade_limit();
```
### Usage of get_cascade_limit

## Cleanup <a name = "cleanup"></a>
```c
void cleanup();
```
### Usage of cleanup

## shutdown <a name = "shutdown"></a>
```c
void shutdown();
```
### Usage of shutdown

## strdup2 <a name = "strdup2"></a>
```c
char* strdup2(char*);
```
### Usage of strdup2

# Indept explanation of Functions <a name = "exp-func"></a>

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

