all: task2c

task2c: start.o task2c.o util.o
	ld -melf_i386 start.o task2c.o util.o -o task2c
start.o:
	nasm -f elf start.s -o start.o
task2c.o:
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector task2c.c -o task2c.o

util.o:
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

.PHONY: clean
clean:
	rm -f *.o task2c