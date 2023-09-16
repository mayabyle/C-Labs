all: task1

task1: start.o
	ld -m elf_i386 start.o -o task1 
 
start.o: start.s
	nasm -f elf start.s -o start.o


.PHONY: clean

clean: 
	rm -f *.o task1

