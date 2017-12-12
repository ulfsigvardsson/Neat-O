SRCDIR = src
OBJDIR = src/obj
CFLAGS = -ggdb -Werror
CC = gcc
RM = rm -f

run: main
	./main

main: main.o refmem.o
	$(CC) $(CFLAGS) $? -o $@

main.o: refmem.h

refmem.o: refmem.h

clean:
	$(RM) main
	$(RM) *.o
	$(RM) *.*~
	$(RM) *.*#

.PHONY: clean
