# Design Documentation
Here follows an introductionary description of the design and functionality of the reference counting garbage collector implemented
by the project group Gulhämplingarna.

## Overview


***

# Useful info
## Format explanations
### obj
### function1_t
### rc_format
### size_format

## Type definitions
```c
 typedef void *obj;
 typedef void(*function1_t)(obj);
 typedef unsigned short rc_format;
 typedef unsigned short size_format;
```
## Function prototypes
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

***

# Usage of Functions
## Retain
```c
void retain(obj);
```
### Usage of retain
Retain given an object is used in order to increase the objects reference count by one.
Be aware of Overflow of the reference count if this happens it will cause the program to terminate.

## Release
```c
void release(obj);
```
### Usage of release
Release given an object is used to decrease the objects reference count by one.
If this results in the objects reference count hitting 0 the object will depending on the cascade limit either be deallocated or be put in garbage and wait until the program has resources to deallocate the object.

## Rc
```c
rc_format rc(obj);
```
### Usage of rc
Reference count is used to keep track of references to a specific object.
The use of rc on an object returns the amount of references to that object in the format rc_format.

## Allocate
```c
obj allocate(size_t, function1_t );
```
### Usage of allocate

## Allocate_array
```c
obj allocate_array(size_t, size_t, function1_t);
```
### Usage of allocate_array


## Deallocate
```c
void deallocate(obj);
```
### Usage of deallocate

## Set_cascade_limit
```c
void set_cascade_limit(size_t);
```
### Usage of set_cascade_limit

## Get_cascade_limit
```c
size_t get_cascade_limit();
```
### Usage of get_cascade_limit

## Cleanup
```c
void cleanup();
```
### Usage of cleanup

## shutdown
```c
void shutdown();
```
### Usage of shutdown

## strdup2
```c
char* strdup2(char*);
```
### Usage of strdup2

