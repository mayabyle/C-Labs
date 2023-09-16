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
            header->e_ident[EI_MAG0],header->e_ident[EI_MAG1],header->e_ident[EI_MAG2],header->e_ident[EI_MAG3]);;    //to make sure with Michael
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

/*return pointer to the relevent table entry(by name)*/
Elf32_Shdr* getTable(char* _name){
    Elf32_Shdr* string_table_entry = map_start+ header->e_shoff+(header->e_shstrndx*header->e_shentsize);
    for (size_t i = 0; i < header->e_shnum; i++){
        Elf32_Shdr* entry = map_start+header->e_shoff+(i* header->e_shentsize);
        char* name = map_start + string_table_entry->sh_offset + entry->sh_name;
        if(strcmp(_name, name)==0){
            return entry;
        }
    }
    return NULL;
}

void printSymbols(){
    if(currFD!=-1) {
        Elf32_Shdr *symbol_table_entry = getTable(".symtab");
        Elf32_Shdr *strtab = getTable(".strtab");       //get the entry of "name" --> symbol name
        Elf32_Shdr *shstrtab = getTable(".shstrtab");   // --> section name
        if (symbol_table_entry != NULL){
            int num_of_entries = symbol_table_entry->sh_size / sizeof(Elf32_Sym);
            if(debug){
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\t\tsize\n");
            }
            else{
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\n");
            }
            for (size_t i = 0; i < num_of_entries; i++) {
                Elf32_Sym *entry = map_start + symbol_table_entry->sh_offset + (i * sizeof(Elf32_Sym));
                char *section_name;
                if (entry->st_shndx == 0xFFF1) { section_name = "ABS";}
                else if (entry->st_shndx == 0x0) { section_name = "UND";}
                else {
                    Elf32_Shdr *section_entry = map_start + header->e_shoff + (entry->st_shndx * header->e_shentsize);
                    section_name = map_start + shstrtab->sh_offset + section_entry->sh_name;
                }
                char *symbol_name = map_start + strtab->sh_offset + entry->st_name;
                char *symbol_size = map_start + strtab->sh_offset + entry->st_size;
                if (debug){
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t\%-20.30s\t\t%-20.30s\n",
                    i, entry->st_value, entry->st_shndx, section_name,symbol_name,symbol_size);
                }
                else {
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t\%-20.30s\n",
                        i, entry->st_value, entry->st_shndx, section_name,symbol_name);
                }
            }
        }
        else{
            perror("Can't find symbol table in the file");
        }
    }
    else{perror("No file is currently open\n");}
}

void printRelocationTable(char* table_name, Elf32_Shdr* table_entry, Elf32_Shdr* dynsym, Elf32_Shdr* dynstr){
    printf("Relocation section '%s' at offset %#x contains %d entries:\n",
            table_name,table_entry->sh_offset,table_entry->sh_size/sizeof(Elf32_Rel));
    printf(" Offset\t\t  Info\t   Type\t   Sym.Value\tSym.Name\n");
    for(int i=0; i<table_entry->sh_size/sizeof(Elf32_Rel);i++){
        Elf32_Rel* entry = map_start+table_entry->sh_offset+(i*sizeof(Elf32_Rel));
        int type = ELF32_R_TYPE(entry->r_info);
        int symbol_table_index = ELF32_R_SYM(entry->r_info);
        Elf32_Sym* symbol_entry = map_start+dynsym->sh_offset+(symbol_table_index*sizeof(Elf32_Sym));
        int value = symbol_entry->st_value;
        char* symbol_name = map_start+dynstr->sh_offset+symbol_entry->st_name;
        printf("%08x\t%08x    %d\t   %08x\t%s\n",
            entry->r_offset, entry->r_info,type,value,symbol_name);
    }
    printf("\n");
}

void relocationTables(){
    if(currFD!=-1) {
        Elf32_Shdr *rel_dyn_entry = getTable(".rel.dyn");
        Elf32_Shdr *rel_plt_entry = getTable(".rel.plt");
        Elf32_Shdr *dynsym = getTable(".dynsym");
        Elf32_Shdr *dynstr = getTable(".dynstr");
        if (rel_dyn_entry != NULL) { printRelocationTable(".rel.dyn", rel_dyn_entry, dynsym, dynstr); }
        if (rel_plt_entry != NULL) { printRelocationTable(".rel.plt", rel_plt_entry, dynsym, dynstr); }
    } else{perror("No file is currently open\n");}
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