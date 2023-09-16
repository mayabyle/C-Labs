all:  task3

task3: task3.o
	gcc -m32 -g -Wall -o task3 task3.o

task3.o: task3.s 
	nasm -g -f elf -w+all -o task3.o task3.s 

.PHONY: clean  

clean:
	rm -f *.o task3
