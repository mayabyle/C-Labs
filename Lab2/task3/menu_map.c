#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char quit(char c) {
	if(c == 'q')
		exit(0);
	return c;
}
                     
char my_get(char c) {
	char ch = fgetc(stdin);
	return ch;
}

char cprt(char c) {
	if(c>=0x20 && c<=0x7E)
		printf("%c\n", c);
	else
		printf("%c\n", '.');
	return c;
}

char xprt(char c) {
	if(c>=0x20 && c<=0x7E)
		printf("%x\n", c);
	else
		printf("%c\n", '.');
	return c;
}

char encrypt(char c) {
	if(c>=0x20 && c<=0x7E)
		return c+3;
	return c;
}

char decrypt(char c) {
	if(c>=0x20 && c<=0x7E)
		return c-3;
	return c;
}

char censor(char c) {
	if(c == '!')
		return '.';
	else
		return c;
}


struct fun_desc {
	char *name;
	char (*fun)(char);
};

char* map(char *array, int array_length, char (*f) (char)){
	char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
	for(int i=0; i<array_length; i++)
		mapped_array[i] = f(array[i]);
	return mapped_array;
}




int main(int argc, char **argv){
	char* carray = (char*)(malloc(5*sizeof(char)));
	struct fun_desc menu[] = { { "my_get", &my_get }, { "cprt", &cprt },  { "xprt", &xprt }, { "censor", &censor },
								{ "encrypt", &encrypt }, { "decrypt", &decrypt }, { "quit", &quit }, { NULL, NULL } }; 
	while(1) {
		printf("Please choose a function:\n");
		size_t i = 0;
		while(menu[i].name != NULL) {
			printf("%d) %s\n", i, menu[i].name); ////&d == digit , %s==string
			i++;
		}
		printf("Option: ");
		char buf[BUFSIZ];
		int opNum;
		fgets(buf, sizeof(buf), stdin); //reads the option from the specified stream and stores it into buf
    	sscanf(buf, "%d", &opNum); //reads formatted input from a string

		if (opNum>=0 && opNum<i) {  //good option
			printf("\nWithin bounds\n");
			char* tmp = map(carray,5,menu[opNum].fun); //use map function
			free(carray);
			carray = tmp;
			printf("DONE.\n\n");
		}
		else {
			printf("Not within bounds\n");
			exit(1);
		}
	}
	free(carray); //free allocated memory
}

