FLAGS= -Wall -g
SOURCEDIR = ../src/
CC = gcc


test: 	test.c refmem.o
	$(CC) $(FLAGS) $^ -o $@ -lcunit 

refmem.o: $(SOURCEDIR)refmem.c $(SOURCEDIR)refmem.h
	$(CC) $(FLAGS) $(SOURCEDIR)refmem.c -c
