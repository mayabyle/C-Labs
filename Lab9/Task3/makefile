all: myELF

myELF: task3.o 
	gcc -g -Wall -o myELF task3.o 

task3.o: task3.c
	gcc -g -Wall -c -o task3.o task3.c

.PHONY: clean

clean: 
	rm -f *.o myELF