
all: task1c

task1c: looper.o
	gcc -m32 -Wall -g looper.o -o looper

looper.o: looper.c
	gcc -g -Wall -m32 -c -o looper.o looper.c

clean:
	rm *.o looper
