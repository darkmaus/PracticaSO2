CC?=gcc
CFLAGS+= -g -Wall
LDFLAGS=
LIBS=-lpthread

test: Practica1.o red-black-tree.o linked-list.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm *.o test


#
#
#OLD MAKEFILE
#
#

#CFLAGS=-g

#default: prog.exe
#	./prog.exe

#prog.exe: Practica1.o red-black-tree.o
#	gcc Practica1.o red-black-tree.o -o prog.exe -lm
	
#Practica1.o: Practica1.c
#	gcc -c Practica1.c 
	
#red-black-tree.o: red-black-tree.c
#	gcc -c red-black-tree.c
	
#forDebug: Practica1d.o red-black-treed.o
#	gcc -g Practica1d.o red-black-treed.o -o Practica1d.exe -lm
	
#Practica1d.o: Practica1.c
#	gcc -c -g Practica1.c -o Practica1d.o
	
#red-black-treed.o: red-black-tree.c
#	gcc -c -g red-black-tree.c -o red-black-treed.o 



