SRCDIR = src
DEMODIR = demo/src
OBJDIR = src/obj
CFLAGS = -g -Werror
CC = gcc
RM = -rm -f

.PHONY: clean, demo

srcclean:
	$(MAKE) clean -C $(SRCDIR)

democlean:
	$(MAKE) clean -C $(DEMODIR)

demo:
	$(MAKE) valgrind -C $(DEMODIR)

