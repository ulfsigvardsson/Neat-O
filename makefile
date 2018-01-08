SOURCEDIR = ./src
DEMODIR = ./demo
TESTDIR = ./test
OBJECTDIR = ./obj
BINARYDIR = ./bin

CFLAGS = -g -Werror
TESTFLAGS= -Wall -pg -fprofile-arcs -ftest-coverage -fPIC

CC = gcc
RM = -rm -f

.PHONY: democlean srcclean demo test clean format bajs

clean:
	make srcclean && make democlean

srcclean:
	$(MAKE) clean -C $(SOURCEDIR)

democlean:
	$(MAKE) clean -C $(DEMODIR)

demo:
	$(MAKE) valgrind -C $(DEMODIR)

test:
	$(MAKE) test -C $(TESTDIR)

format:
	$(shell find . -name "*.c" -o -name "*.h" | xargs astyle --style=gnu -xd -p -xC79)
