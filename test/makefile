FLAGS= -Wall -g
SOURCEDIR = ../src/
CC = gcc
RM = -rm -f

.PHONY: clean

test: compile
	./test

compile: test.c
	$(CC) $(FLAGS) $^ -o test -lcunit 

clean:
	$(RM) test
	$(RM) refmem.o
