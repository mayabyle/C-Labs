#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
    FILE *in= stdin;
    FILE *out = stdout;
    int debug = 0;
    int mathCond = 0;
    int keyLen = 0;
    char *keyString;
    int count = 0;

    for(size_t i=1; i<argc; i++) {
        if(strcmp(argv[i], "-D") == 0)
            debug = 1;
        if(strncmp(argv[i],"+e",2) == 0){
			mathCond = 1; //for +
			keyLen = strlen(argv[i]);
			keyString = argv[i];
            count++;
        }    
        if(strncmp(argv[i], "-e", 2) == 0){
			mathCond = -1; //for -
		    keyLen = strlen(argv[i]);
			keyString = argv[i];
            count++;
        }
        if(strncmp("-i", argv[i], 2) == 0) {
            in = fopen(argv[i]+2 , "r");
            if(in == NULL){
                fprintf(stderr ,"error there is no file with this name\n");
                exit(1);
            }
        }
        if(strncmp("-o", argv[i], 2) == 0) {
            char* fileName = &argv[i][2];
            out = fopen(fileName, "w");
        }
    }

    int currIndex = 2;
    char ch = fgetc(in);

    while(ch != EOF){ 
	    if(mathCond != 0 && ch == '\n') {  //for +/-e
            fputc((char) ch, out);  //writes a character to the specified stream and advances the position indicator for the stream
            currIndex = 2;
        }
        // if (mathCond == 0 && ch == '\n') //no encryption key argument is supplied
        //     break;
        if(ch!='\n') {
            if (currIndex == keyLen)
                currIndex = 2;

            if(debug == 1)
                fprintf(stderr, "%X\t", ch);
            if(count != 0) 
                ch = (ch + mathCond*(keyString[currIndex]-'0')%128);
            else if(ch>='a' && ch<='z')
                ch = ch -32;
            
            if((debug == 1) & (ch != '\n')) 
                fprintf(stderr, "%X\n", ch);
            fputc((char) ch, out);
            currIndex++;
        }
        ch=fgetc(in);    
        
    }

    fclose(out);
    fclose(in);
    return 0;
}
