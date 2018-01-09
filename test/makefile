FLAGS= -Wall -g
SRCDIR = ../src/
CC = gcc
RM = -rm -f

.PHONY: clean

run: main
	./main

integration: memtest.c refmem.o
	$(CC) $(CFLAGS) $? -o $@

refmem.o: $(SRCDIR)refmem.c $(SRCDIR)refmem.h
	$(CC) $(FLAGS) $(SRCDIR)refmem.c -c

test: compile
	./test

compile: test.c
	$(CC) $(FLAGS) $^ -o test -lcunit 

clean:
	$(RM) test
	$(RM) refmem.o
	$(RM) integration
	$(RM) *.orig


memtest: integration
	valgrind --leak-check=full ./integration
