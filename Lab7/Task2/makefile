all: task2

task2: LineParser.o task2.o
	gcc -Wall -g LineParser.o task2.o -o task2

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c

task2.o: task2.c
	gcc -g -Wall -c -o task2.o task2.c
.PHONY: clean
clean:
	rm -f *.o task2
