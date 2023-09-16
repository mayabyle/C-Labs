#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define buffer_length 100

/*Global variables*/
int debug = 0;
int currFD=-1;
void* map_start;        /* will point to the start of the memory mapped file */
struct stat fd_stat;    /* this is needed to  the size of the file */
Elf32_Ehdr *header;     /* this will point to the header structure */

typedef struct {
  char *name;
  void (*fun)();
}fun_desc;

void toggleDebugMode () {
  if (debug == 0) {
    printf("Debug flag now on\n");
    debug = 1;
  }
  else {
    printf("Debug flag now off\n");
    debug = 0;
  }
}

int LoadFile(){     //reading material
    char filename[buffer_length];
    int fd;
    fscanf(stdin,"%s",filename);
    if((fd = open(filename, O_RDWR)) < 0) {
      perror("error in open");
      exit(-1);
   }
    if(fstat(fd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
   }
   if(currFD!=-1){close(currFD);}
    currFD=fd;
    return currFD;
}

char* dataType(Elf32_Ehdr* header){
    switch (header->e_ident[5]){
    case ELFDATANONE: return "invalid data encoding";
    case ELFDATA2LSB: return "2's complement, little endian";
    case ELFDATA2MSB: return "2's complement, big endian";
    default: return "NO DATA";
    }
}

void examineELFFile(){
    printf("Enter file name: ");
    if(LoadFile()==-1){exit(EXIT_FAILURE);}
    header = (Elf32_Ehdr *) map_start;
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0){    //chech if ELF file
        printf("Magic:\t\t\t\t %x %x %x %x\n",
            header->e_ident[EI_MAG0],header->e_ident[EI_MAG1],header->e_ident[EI_MAG2],header->e_ident[EI_MAG3]);    //to make sure with Michael
        printf("Data:\t\t\t\t %s\n",dataType(header));
        printf("Enty point address:\t\t 0x%x\n",header->e_entry);
        printf("Start of section headers:\t %d (bytes into file)\n",header->e_shoff);
        printf("Number of section headers:\t %d\n",header->e_shnum);
        printf("Size of section headers:\t %d (bytes)\n",header->e_shentsize);
        printf("Start of program headers:\t %d (bytes into file)\n",header->e_phoff);
        printf("Number of program headers:\t %d\n",header->e_phnum);
        printf("Size of program headers:\t %d (bytes)\n",header->e_phentsize);
   }
    else{
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size); 
        close(currFD); 
        currFD=-1;
    }
}

char *sectionType(int type) {
    switch (type) {
        case SHT_NULL:return "NULL";
        case SHT_PROGBITS:return "PROGBITS";
        case SHT_SYMTAB:return "SYMTAB";
        case SHT_STRTAB:return "STRTAB";
        case SHT_RELA:return "RELA";
        case SHT_HASH:return "HASH";
        case SHT_DYNAMIC:return "DYNAMIC";
        case SHT_NOTE:return "NOTE";
        case SHT_NOBITS:return "NOBITS";
        case SHT_REL:return "REL";
        case SHT_SHLIB:return "SHLIB";
        case SHT_DYNSYM:return "DYNSYM";
        default:return "Unknown";
    }
}

/*prints one entry of section*/
void printSectionEntry(int i,char* name ,Elf32_Shdr* section,int offset){
    if(debug){
        printf("[%2d] %-18.18s\t%#09x\t%06d\t%06d\t%-13.10s\t%d\n",
        i,name,section->sh_addr,section->sh_offset,section->sh_size,sectionType(section->sh_type),offset );
    }
    else{
        printf("[%2d] %-18.18s\t%#09x\t%06d\t%06d\t%-13.10s\n",
        i, name ,section->sh_addr,section->sh_offset, section->sh_size, sectionType(section->sh_type) );}
}

void printSectionNames(){
    if(currFD!=-1){
    	Elf32_Shdr* sections_table = map_start+header->e_shoff;
    	Elf32_Shdr* string_table_entry = map_start+header->e_shoff+(header->e_shstrndx*header->e_shentsize); //to get the names
		if(debug){
			fprintf(stderr,"section table address: %p\n",sections_table);
			fprintf(stderr,"string table entry: %p\n",string_table_entry);
            printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\t\toffset(bytes)\n");
        }
		else{printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\n");}
    	for (size_t i = 0; i < header->e_shnum; i++){       
    		Elf32_Shdr* entry = map_start+header->e_shoff+(i* header->e_shentsize);     //header->e_shoff+(i* header->e_shentsize) ==> section
        	char* name = map_start + string_table_entry->sh_offset + entry->sh_name;
        	printSectionEntry(i,name,entry,header->e_shoff+(i* header->e_shentsize));
    	}
  	}
  	else{perror("No file is currently open\n");}
}

void printSymbols(){
    fprintf(stdout, "not implemented yet\n");
}

void relocationTables(){
    fprintf(stdout, "not implemented yet\n");
}

void quit(){
    if (debug) { printf("quitting\n");}
    if (currFD > -1){
        munmap(map_start, fd_stat.st_size); 
        close(currFD); 
    }
    currFD=-1;
    exit(0);
}

void displayMenu (fun_desc menu[]){
    fprintf(stdout, "Choose action:\n");
    int i=0 ;
    while(menu[i].name != NULL) {
      fprintf(stdout,"%d) %s\n", i, menu[i].name);
      i++;
    }
    fprintf(stdout,"Option: ");
}

int getUserInput (int bounds){
  int option;
  scanf("%d", &option);
  if (option>=0 && option<bounds){
    fprintf(stdout,"\n");
    return option;
  }
  else{
    fprintf(stdout, "Not within bounds\n" );
    return -1;
  }
}

int main(int argc, char **argv){
    fun_desc menu[] = {{"Toggle Debug Mode",toggleDebugMode},{"Examine ELF File",examineELFFile},
                        {"Print Section Names",printSectionNames},{"Print Symbols",printSymbols},
                        {"Relocation Tables",relocationTables},{"Quit",quit},{NULL,NULL}};
    size_t i=0;
    while(menu[i].name != NULL){
        i++;
    }
    while(1) {
        displayMenu(menu);
        int option = getUserInput(i);
        if(option != -1) { menu[option].fun();}
        printf("\n");
    }
    return 0;
}