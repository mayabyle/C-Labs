#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	FILE *in= stdin;
	int debug = 0;
	int mathCond = 2;
	int keyLen = 0;
	char *keyString;
	int count = 0;

	if(argc == 1) {
		char ch = fgetc(in);
		while(ch != EOF) {
			if(ch>='a' && ch<='z') 
				printf("%c/n", ch-32);
			else 
				printf("%c/n", ch);
			ch=fgetc(in);
		}
	}
	else {
		for(size_t i=1; i<argc; i++) {
			if (strncmp(argv[i], "-D", 2) == 0)
        		debug = 1; 
			if (strncmp(argv[i], "+e", 2) == 0) {
				mathCond = 1; //for +
				keyLen = strlen(argv[i]);
				keyString = argv[i];
				count++;
			}
			if (strncmp(argv[i], "-e", 2) == 0) { 
				mathCond = -1; //for -
		    	keyLen = strlen(argv[i]);
				keyString = argv[i];
				count++;
			}
		}

		char ch = fgetc(in);
		int currIndex = 2;

		while(ch != EOF) {	
			if(ch != '\n') {
				if(currIndex == keyLen)
					currIndex = 2;
				if(debug == 1)
					fprintf(stderr, "%X\t", ch);
				if(count != 0)
					ch = (ch + mathCond*(keyString[currIndex]-'0')%128);
				else if(ch>='a' && ch<='z')
					ch = ch -32;
				if(debug == 0) 
					printf("%c", ch);
				else //debug == true
					fprintf(stderr, "%X\n", ch);
				currIndex++;
			}
			else {
				printf("%c", '\n');
				currIndex = 2;
			}
			ch=fgetc(in);
		}
	}
	if(in!=NULL || in!=stdin)
		fclose(in);
	return 0;
}


