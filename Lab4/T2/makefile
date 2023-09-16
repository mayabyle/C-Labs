
all:  task2

task2: task2.o
	gcc -m32 -g -Wall -o task2 task2.o

task2.o: task2.s 
	nasm -g -f elf -w+all -o task2.o task2.s 

.PHONY: clean  

clean:
	rm -f *.o task2
