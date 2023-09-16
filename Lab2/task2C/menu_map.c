#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//task 2c
/* Gets a char c, and if the char is 'q' , ends the program with exit code 0. Otherwise returns c. */
char quit(char c) {
	if(c == 'q')
		exit(0);
	return c;
}

//task2b                     
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


//task 2a

char censor(char c) {
	if(c == '!')
		return '.';
	else
		return c;
}

char* map(char *array, int array_length, char (*f) (char)){
	char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
	for(int i=0; i<array_length; i++)
		mapped_array[i] = f(array[i]);
	return mapped_array;
}

int main(int argc, char **argv){
	int base_len = 5;
	char arr1[base_len];
	char* arr2 = map(arr1, base_len, my_get);
	char* arr3 = map(arr2, base_len, cprt);
	char* arr4 = map(arr3, base_len, xprt);
	char* arr5 = map(arr4, base_len, encrypt);
	char* arr6 = map(arr5, base_len, decrypt);
	free(arr2);
	free(arr3);
	free(arr4);
	free(arr5);
	free(arr6); 
}

