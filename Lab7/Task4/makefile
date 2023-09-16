all: task4

task4: LineParser.o task4.o
	gcc -Wall 	-g LineParser.o task4.o -o task4

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c

task4.o: task4.c
	gcc -g -Wall -c -o task4.o task4.c
	
.PHONY: clean
clean:
	rm -f *.o task4
