SOURCEDIR = ./src
DEMODIR = ./demo
TESTDIR = ./test
OBJECTDIR = ./obj
BINARYDIR = ./bin

CFLAGS = -g -Werror
TESTFLAGS= -Wall -pg -fprofile-arcs -ftest-coverage -fPIC

CC = gcc
RM = -rm -f

.PHONY: democlean, srcclean, demo, test

srcclean:
	$(MAKE) clean -C $(SOURCEDIR)

democlean:
	$(MAKE) clean -C $(DEMODIR)

demo:
	$(MAKE) valgrind -C $(DEMODIR)

test:
	$(MAKE) test -C $(TESTDIR)
