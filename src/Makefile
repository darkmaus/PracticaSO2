default: prog.exe
	./prog.exe

prog.exe: Practica1.c
	gcc Practica1.o red-black-tree.o -o prog.exe -lm
	
Practica1.o: Practica1.c
	gcc Practica1.c -c Practica1.o
	
red-black-tree.o: red-black-tree.c
	gcc red-black-tree.c -c red-black-tree.o