all: task3

task3: LineParser.o task3.o
	gcc -Wall -g LineParser.o task3.o -o task3

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c

task3.o: task3.c
	gcc -g -Wall -c -o task3.o task3.c
.PHONY: clean
clean:
	rm -f *.o task3
