# Design Documentation
Here follows an introductionary description of the design and functionality of the reference counting garbage collector implemented
by the project group Gulhämplingarna.

## Overview
The header file consists of the following functions.
```c
 typedef void *obj;
 typedef void(*function1_t)(obj);
 typedef unsigned short rc_format;
 typedef unsigned short size_format;

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
![Header](header.png)

## retain
### header of retain
```c
void retain(obj);
```
### usage of retain
Retain given an object is used in order to increase the objects reference count by one.
Be aware of Overflow of the reference count if this happens it will cause the program to terminate.

## release
### header of retain
```c
void release(obj);
```
### usage of release
Release given an object is used to decrease the objects reference count by one.
If this results in the objects reference count hitting 0 the object will depending on the cascade limit either be deallocated or be put in garbage and wait until the program has resources to deallocate the object.

## rc
### header of rc
```c
rc_format rc(obj);
```
### usage of rc


## allocate
```c
obj allocate(size_t, function1_t );
```

## allocate_array
```c
obj allocate_array(size_t, size_t, function1_t);
```

## deallocate
```c
void deallocate(obj);
```

## set_cascade_limit
```c
void set_cascade_limit(size_t);
```

## get_cascade_limit
```c
size_t get_cascade_limit();
```

## cleanup
```c
void cleanup();
```

## shutdown
```c
void shutdown();
```

## strdup2
```c
char* strdup2(char*);
```
