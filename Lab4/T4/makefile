all:  task4

task4: task4.o
	gcc -m32 -g -Wall -o task4 task4.o

task4.o: task4.s 
	nasm -g -f elf -w+all -o task4.o task4.s 

.PHONY: clean  

clean:
	rm -f *.o task4
