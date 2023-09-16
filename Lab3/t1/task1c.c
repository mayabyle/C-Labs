
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define nop (0x90)

typedef struct virus {
	unsigned short SigSize;		//2
	char virusName[16];			//16
	unsigned char* sig;			//1
} virus;


typedef struct link {
	struct link *nextVirus;
	virus *vir;
} link;

// Task 1a

//this function receives a file pointer and returns a virus* that represents the next virus in the file.
//read the file and create virus object
virus* readVirus(FILE* in) {
	virus* virus  = malloc(sizeof(struct virus));  //memory allocation for the virus from the input
  	if(fread(virus, 1, 18, in) != 0){
    	virus->sig=malloc(virus->SigSize);
    	fread(virus->sig, 1, virus->SigSize, in);
  	}
  	return virus;
}

/*The function prints the virus to the given output. It prints the virus name (in ASCII), 
	the virus signature length (in decimal), and the virus signature (in hexadecimal representation). */
void printVirus(virus* virus, FILE* output) {
	fprintf(output, "virus name: %s\n", virus->virusName);  //sends formatted output to output
	fprintf(output, "virus signature length: %d\n", virus->SigSize);
	fprintf(output, "virus signature:\n");
	for(int i=0; i<(virus->SigSize); i++) 
		fprintf(output, "%02hhX ", virus->sig[i]);
	fprintf(output,"\n");
}

// Task 1b

// Print the data of every link in list to the given stream. Each item followed by a newline character.
void list_print(link *virus_list, FILE* in) {
	link* curr = virus_list;
	while (curr != NULL){
		printVirus(curr->vir, in);
		curr = curr->nextVirus;
    }
}

/* Add a new link with the given data to the list (either at the end or the beginning, depending on what your TA tells you), 
and return a pointer to the list (i.e., the first link in the list). If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data) {
	if (virus_list == NULL) {
		link* newVirLink = malloc(sizeof(struct link));
		newVirLink->vir = data;
		newVirLink->nextVirus = NULL;
		virus_list = newVirLink;
	}
	else
		virus_list->nextVirus = list_append(virus_list->nextVirus,data);
	return virus_list;
}

//Free the memory allocated by the list.
void list_free(link *virus_list) {
	link* curr = virus_list;  //the address of the first link
	if(curr != NULL){
		list_free(curr->nextVirus);  //recursion for keeping the access to next link all the time
		free(curr->vir->sig);
		free(curr->vir);
		free(curr);
	}
	return;
}

link* load_list(FILE* file) {
	link* firstLink = NULL;
	int byteSum = 4;
	char buf[4];
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	rewind(file);     //return to the beginning
	fread(&buf, 1, 4, file);
	while(byteSum<size){
		virus* nextVirus = readVirus(file);
		firstLink = list_append(firstLink,nextVirus);
		byteSum += 18 + nextVirus->SigSize;
	}
	return firstLink;
}

link* load_signatures(link* link, FILE* x) {
	char buf[BUFSIZ]; 
	char* fileName = NULL;
	printf("Enter file name: ");
	fgets(buf, sizeof(buf), stdin); //Reading stops after a newline.
	sscanf(buf,"%ms",&fileName);
	FILE* file = fopen(fileName,"rb");
	free(fileName);
	if(file == NULL) {
		fprintf(stderr,"Error\n");
		exit(EXIT_FAILURE);
	}
	struct link* firstLink = load_list(file);
	return firstLink;
}

link* print_signatures(link *list, FILE* file){
	list_print(list, stdout);
	return list;
}

//Task 1c

//find specific virus
link* find_virus(link* node, int i){
  if(i == 0)
    return node;
  return find_virus(node->nextVirus, i-1);
}

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
	link* tmp = virus_list;  //list size
    int l_size = 0;
    while(tmp != NULL) {
    	l_size++;
        tmp = tmp->nextVirus;
    }
	for (int i=0; i<size; i++) {
		for (int j = 0; j < l_size; j++) {
			virus *virus = find_virus(virus_list, j)->vir;
			int cmp = -1;
			if(size-i >= virus->SigSize)
				cmp = memcmp(buffer+i, virus->sig, virus->SigSize);
			if (cmp == 0) {
				fprintf(stdout, "The starting byte location: %d\n", i);
				fprintf(stdout, "The virus name: %s\n", virus->virusName);
				fprintf(stdout, "The size of the virus signature: %d\n", virus->SigSize);
			}
		}
	}
	free(tmp);
}

link* detect_viruses(link* list, FILE* file) {
    if(file == NULL) {
		fprintf(stderr, "Error with reading the file\n");
		exit(EXIT_FAILURE);
    }
	else { 
		fseek(file, 0, SEEK_END);  //file size
		unsigned file_size = ftell(file);
		rewind(file);
		char* buf = malloc(file_size * sizeof(unsigned char));
		fread(buf, 1, file_size, file);
		detect_virus(buf, file_size, list);
		free(buf);
		return list;
	}
}

link* quit(link* list,  FILE* file) {
    list_free(list);
    exit(0);
	return NULL;
}

link* fix_file(link* list, FILE* file, char *fileName) {
	printf("Not implemented\n");
	return list;
}

void print_menu() {
	printf("0. Load signatures\n");
	printf("1. Print signatures\n");
	printf("2. Detect virus\n");
	printf("3. Fix file\n");
	printf("4. Quit\n");
	printf("Option: ");
}

link* act_op(int op, link* list, FILE* file, char *fileName) {
	if(op==0)
		list = load_signatures(list, file);
	else if(op==1)
		list = print_signatures(list, file);
	else if(op==2)
		list = detect_viruses(list, file);
	else if(op==3)
		list = fix_file(list, file, fileName);
	else
		list = quit(list, file);
	printf("\n");
	return list;
}


int main(int argc, char **argv) {
	FILE* file = NULL;
	link* list = NULL;
	char* fileName;
	if (argc>1) {
		fileName = argv[1];
		file = fopen(fileName,"r+");
		printf("name of file: %s\n", fileName);
		if(file == NULL) {
			fprintf(stderr,"Error with reading the file\n");
			exit(EXIT_FAILURE);
		}
	}

	while (1) {
		printf("Please choose a function:\n");
		int op;
		print_menu();
		scanf(" %d", &op);
		fgetc(stdin);
		printf("You entered:  %d\n\n", op);
		if (op>=0 && op<5) 
			list = act_op(op, list, file, fileName);
		else {
			printf("out of bounds\n");
			list_free(list);
			break;
		}
	}
	free(file);
	fclose(file);
	free(stdout);
	return 0;
}