SRCDIR = src
OBJDIR = src/obj
CFLAGS = -ggdb -Werror
CC = gcc
RM = rm -f

run: main
	./main

main: main.o refmem.o
	$(CC) $(CFLAGS) $? -o $@

main.o: refmem.h refmem.o

refmem.o: refmem.h

clean:
	$(RM) *.o
	$(RM) *.*~
	$(RM) *^#*
	$(RM) main
	$(RM) *.orig

valgrind: main
	valgrind --leak-check=full ./main

.PHONY: clean
