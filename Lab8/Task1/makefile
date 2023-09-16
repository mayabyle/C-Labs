all: task1

task1: LineParser.o task1.o
	gcc -Wall	-g LineParser.o task1.o -o task1

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c

task1.o: task1.c
	gcc -g -Wall -c -o task1.o task1.c
.PHONY: clean
clean:
	rm -f *.o task1
