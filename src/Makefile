default: prog.exe
	./prog.exe

prog.exe: Practica1.o red-black-tree.o
	gcc Practica1.o red-black-tree.o -o prog.exe -lm
	
Practica1.o: Practica1.c
	gcc -c Practica1.c 
	
red-black-tree.o: red-black-tree.c
	gcc -c red-black-tree.c
	
forDebug: Practica1d.o red-black-treed.o
	gcc Practica1d.o red-black-treed.o -o progDebug.exe -lm
	
Practica1d.o: Practica1.c
	gcc -c Practica1.c -o Practica1d.o
	
red-black-treed.o: red-black-tree.c
	gcc -c red-black-tree.c -o red-black-treed.o 
