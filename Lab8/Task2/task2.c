#include <stdio.h>
#include "LineParser.h"
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define STDOUT 1
#define STDIN 0

void Error(){
    perror(strerror(errno));
    _exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    pid_t childPid1, childPid2;
    char* const tail_Args[4] = {"tail", "-n", "2", 0};
    char* const ls_Args[3] = {"ls", "-l", 0};
    int des[2];
    int debug = 0;
    for(int i=0; i<argc; i++){
        if(strncmp(argv[i], "-d", 2) == 0)
            debug = 1;
    }
    if(debug)
        fprintf(stderr,"(%s)\n","parent_process > forking...");
    if(pipe(des)==-1)
        Error();

    childPid1 = fork();
    if(childPid1 == -1)
        Error();
    if(debug) {
        int pID = getpid();
        if(pID == -1)
            fprintf(stderr,"(%s)\n","parent_process > created process with id");   //לא בטוחה
        else
            fprintf(stderr, "(%s: %d)\n", "parent_process > created process with id", pID);
    }

    if(!childPid1) {   //in child1 process
        if(debug)
            fprintf(stderr,"(%s)\n","child1 > redirecting stdout to the write end of the pipe...");
        close(STDOUT);  //Close the standard output.
        dup2(des[1], STDOUT);  //Duplicate the write-end of the pipe.
        close(des[1]);  //Close the file descriptor that was duplicated.
        if(debug)
            fprintf(stderr,"(%s)\n","child1 > going to execute cmd: ...");
        execvp(ls_Args[0] , ls_Args);
        Error(); 
    } 
    else {   //in parent process
        if(debug)
            fprintf(stderr,"(%s)\n","parent_process > closing the write end of the pipe...\nparent_process > forking...");
        close(des[1]);  //Close the write end of the pipe.
        // if(debug)
        //     fprintf(stderr,"(%s)\n","parent_process > forking...");
        childPid2 = fork();  //Fork a second child process (child2)
        if (childPid2 == -1)
            Error();

        if (debug) {
            if (getpid() == -1)
                fprintf(stderr, "(%s)\n", "parent_process > created process with id");
            else
                fprintf(stderr, "(%s: %d)\n", "parent_process > created process with id", getpid());
        }
        if(!childPid2) {  //in child2 process
            if(debug)
                fprintf(stderr, "(%s)\n", "child2 > redirecting stdin to the read end of the pipe...");
            close(STDIN);  //Close the standard input.
            dup2(des[0],STDIN);  //Duplicate the read-end of the pipe using dup.
            close(des[0]);  //Close the file descriptor that was duplicated.
            if(debug)
                fprintf(stderr, "(%s)\n","child2 > going to execute cmd: ...");
            execvp(tail_Args[0], tail_Args);  //Execute "tail -n 2".
            Error();  
        }
        else {  //in parent process
            if(debug)
                fprintf(stderr, "(%s)\n", "parent_process > closing the read end of the pipe...");
            close(des[0]);  //Close the read end of the pipe.
            if(debug)
                fprintf(stderr, "(%s)\n","parent_process > waiting for child processes to terminate...");
            waitpid(childPid1, NULL, 0);  //wait for child1 processes to terminate
            if(debug)
                fprintf(stderr, "(%s)\n", "parent_process > waiting for child processes to terminate...");
            waitpid(childPid2, NULL, 0);  //wait for child2 processes to terminate
        }
    }
    if(debug) {
        fprintf(stderr, "(%s)\n","parent_process>closing the read end of the pipe");
        fprintf(stderr, "(%s)\n","parent_process>exiting...");
    }
    return 0;
}