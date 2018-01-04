CC=gcc
FLAGS= -Wall -g
SRCDIR = ../src/

lager: lager.c list.o tree.o item.o utils.o shelfinfo.o undo.o refmem.o
	$(CC) $(FLAGS) $^ -o $@

list.o: list.h list.c $(SRCDIR)refmem.o
	$(CC) $(FLAGS) list.c -c

tree.o: tree.h tree.c $(SRCDIR)refmem.o
	$(CC) $(FLAGS) tree.c -c

item.o: list.o item.c item.h $(SRCDIR)refmem.o
	$(CC) $(FLAGS) item.c list.c -c

utils.o: utils.c utils.h $(SRCDIR)refmem.o
	$(CC) $(FLAGS) utils.c -c

shelfinfo.o: shelfinfo.c shelfinfo.h $(SRCDIR)refmem.o
	$(CC) $(FLAGS) shelfinfo.c -c

undo.o: undo.c undo.h common.h tree.o item.o $(SRCDIR)refmem.o
	$(CC) $(FLAGS) undo.c tree.c item.c -c

refmem.o: $(SRCDIR)refmem.c $(SRCDIR)refmem.h
	$(CC) $(FLAGS) $(SRCDIR)refmem.c -c

run : lager
	./lager db.txt

valgrind: lager
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./lager db.txt
 
clean:
	rm -f *.o
	rm -f lager
	rm -f *~
	rm -f *.*~
	rm -f *.*#
