SRCDIR = src
OBJDIR = src/obj
CFLAGS = -g -Werror
CC = gcc
RM = -rm -f

run: main
	./src/main

main: 
	$(MAKE) main -C $(SRCDIR)

.PHONY: clean

srcclean:
	$(MAKE) clean -C $(SRCDIR) 

valgrind:
	valgrind --leak-check=full ./src/main
