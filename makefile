SOURCEDIR = ./src
DEMODIR = ./demo
OBJECTDIR = ./obj
BINARYDIR = ./bin
CFLAGS = -g -Werror
CC = gcc
RM = -rm -f

.PHONY: clean, demo

srcclean:
	$(MAKE) clean -C $(SOURCEDIR)

democlean:
	$(MAKE) clean -C $(DEMODIR)

demo:
	$(MAKE) valgrind -C $(DEMODIR)

