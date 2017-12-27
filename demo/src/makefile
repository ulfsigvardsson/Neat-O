CC=gcc
FLAGS= -Wall -g
TESTFLAGS= -Wall -pg -fprofile-arcs -ftest-coverage -fPIC

lager: lager.c list.o tree.o item.o utils.o shelfinfo.o undo.o refmem.o
	$(CC) $(FLAGS) -O0 $^ -o $@

list.o: list.h list.c
	$(CC) $(FLAGS) list.c -c

tree.o: tree.h tree.c
	$(CC) $(FLAGS) tree.c -c

item.o: list.o item.c item.h
	$(CC) $(FLAGS) item.c list.c -c

utils.o: utils.c utils.h
	$(CC) $(FLAGS) utils.c -c

shelfinfo.o: shelfinfo.c shelfinfo.h
	$(CC) $(FLAGS) shelfinfo.c -c

undo.o: undo.c undo.h common.h tree.o item.o
	$(CC) $(FLAGS) undo.c tree.c item.c -c

refmem.o: refmem.c refmem.h
	$(CC) $(FLAGS) refmem.c -c

test:	test.c list.o tree.o item.o undo.o shelfinfo.o utils.o
	$(CC) $(FLAGS) test.c list.c tree.c item.c utils.c shelfinfo.c undo.c -o test -lcunit

analysis: analysis.c list.o tree.o
	$(CC) $(TESTFLAGS) $^ -o $@

run : lager
	./lager db.txt

runtest: test
	./test

valgrind: lager
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./lager db.txt
 
clean:
	rm -f *.o
	rm -rf *.dSYM
	rm -f lager
	rm -f *~
	rm -f *.gcno
	rm -f *.gcda
	rm -f *.gcov
	rm -f test
	rm -f gmon.out
