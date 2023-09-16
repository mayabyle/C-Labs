#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

extern int startup(int argc, char **argv, void (*start)());

int currFD = -1;
char* currentFilenameOpen = NULL;
char* fileName;
void* map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to the size of the file */
Elf32_Ehdr* header; /* this will point to the header structure */
int fd;

 /*
#define PF_R		0x4
#define PF_W		0x2
#define PF_X		0x1*/
char* getPrivacy(int i) {
    switch(i) {
        case 0:
            return "   ";
        case 1:
            return "  E";
        case 2:
            return " W ";
        case 3:
            return " WE";
        case 4:
            return "R  ";
        case 5:
            return "R E";
        case 6:
            return "RW ";
        case 7:
            return "RWX";
        default:
            return "   ";
    }
}
 
//TODO: change flags to be similar to moodle task
char* getFlag(int i) {
    switch(i) {
        case 0:
            return "None";
        case 1:
            return "PROTection_EXEC\t";
        case 2:
            return "PROTection_WRITE\t";
        case 3:
            return "PROTection_EXEC | PROTection_WRITE";
        case 4:
            return "PROTection_READ\t";
        case 5:
            return "PROTection_EXEC | PROTection_READ";
        case 6:
            return "PROTection_WRITE | PROTection_READ";
        case 7:
            return "PROTection_EXEC | PROTection_WRITE | PROTection_READ";
        default:
            return "UNDIFINED";
    }
}

char* getType(int d) {
    switch(d) {
        case 0:
            return "NULL\t";
        case 1: 
            return "LOAD\t";
        case 2:
            return "DYNAMIC\t";
        case 3:
            return "INTERP\t";
        case 4:
            return "NOTE\t";
        case 5:
            return "SHLIB\t";
        case 6:
            return "PHDR\t";
        case 7:
            return "TLS\t";
        case 0x60000000:
            return "PT_LOOS";
        case 1685382481:
            return "GNU_STACK";
        case 1685382482:
            return "GNU_RELRO";
        default:
            return "UNDIFINED";
    }
}

int LoadFile() {
    if((fd = open(fileName, O_RDWR)) < 0) {
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
    if(currFD!=-1)
        close(currFD);
    currFD=fd;
	strcpy((char*)&currentFilenameOpen, (char*)fileName);
    return currFD;
}

//This function will apply func to each program header.
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr*, int), int arg) {
    header = (Elf32_Ehdr*)map_start;
    if(strncmp((char*)header->e_ident, (char*)ELFMAG, 4) != 0) { 
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size); 
        close(currFD); 
        currFD = -1;
        currentFilenameOpen = NULL;
        return -1;
    }
    int headers_num = header->e_phnum;  //the number of program headers
    int i = 0;
    while(i < headers_num) {
        Elf32_Phdr* currHeader = map_start + header->e_phoff + (i * sizeof(Elf32_Phdr));
        func(currHeader, currFD);
	    i++;
    }
    return 0;
}

//TODO - understand why Align 1000
void print_header(Elf32_Phdr* phdr, int fd) {
    char* type = getType(phdr->p_type);
    char* privacy = getPrivacy(phdr->p_flags);
    printf("%s\t\t%x\t\t%x\t\t%x\t\t%x\t\t%x\t\t%s\t\t%x\t\t%s\n" , 
            type, phdr->p_offset, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz, getFlag(phdr->p_flags), phdr->p_align, privacy);
}

void print_general_header() {
    printf("Type\t\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\t\tMemSiz\t\tFlg\t\t\t\tAlign\t\tMapFlag\n");
}


int getProtection(Elf32_Phdr *phdr) {
    switch(phdr->p_flags & (PF_R | PF_W | PF_X)) {
        case PF_R:
            return PROT_READ;
        case PF_W:
            return PROT_WRITE;
        case PF_X:
            return PROT_EXEC;
        case PF_R | PF_W:
            return PROT_READ | PROT_WRITE;
        case PF_R | PF_X:
            return PROT_READ | PROT_EXEC;
        case PF_W | PF_X:
            return PROT_WRITE | PROT_EXEC;
        case PF_R | PF_W | PF_X:
            return PROT_READ | PROT_WRITE | PROT_EXEC;
        default:
            return PROT_NONE;
    }
}


void load_phdr(Elf32_Phdr *phdr, int fd) {
    if(phdr->p_type == PT_LOAD) {
        int protection = getProtection(phdr);
        int flags = MAP_PRIVATE | MAP_FIXED;
	    // vaddr: aligns the virtual address to page boundaries by '&' it with the value 0xfffff000 - to ensure that the virtual address is aligned to the start of a page.
        unsigned int vaddr = phdr->p_vaddr & 0xfffff000;
	    // offset: aligns offset to page boundaries by '&' it with the value 0xfffff000 - to ensure that the virtual address is aligned to the start of a page.
        int offset = phdr->p_offset & 0xfffff000;
	    // padding: aligns the virtual address of the segment (phdr->p_vaddr) to page boundaries.
        int padding = phdr->p_vaddr & 0xfff;
        void* map = mmap((void*)vaddr, phdr->p_memsz+padding, protection, flags, currFD, offset);
        if(map == MAP_FAILED){
            perror("mmap");
            exit(-1);
        } else {
            print_header(phdr, fd);
        }
    }
}



int main(int argc, char **argv) {
    fileName = argv[1];
    if(argc <= 1) {
        perror("File name argument is missing");
        exit(EXIT_FAILURE);
    }
    if(LoadFile() == -1)
        exit(EXIT_FAILURE);
    print_general_header();
    if(foreach_phdr(map_start, load_phdr, currFD) == -1) //mapping all the program headers with load flag
        exit(EXIT_FAILURE);
    startup(argc-1, argv+1, (void *)(((Elf32_Ehdr*)map_start)->e_entry));  //executing the loaded files
    printf("\n");
    return 0;
}
