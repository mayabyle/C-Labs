
all:  task1

task1: task1.o
	gcc -m32 -g -Wall -o task1 task1.o

task1.o: task1.s 
	nasm -g -f elf -w+all -o task1.o task1.s 

.PHONY: clean  

clean:
	rm -f *.o task1
