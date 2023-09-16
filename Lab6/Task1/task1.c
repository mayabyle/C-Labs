
#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define STDIN 0
#define STDOUT 1
#define BUFFER_SIZE 2048

int debug;

void execute(cmdLine* pCmdLine);
void pipeProc(cmdLine*);
void freeCmdLines(cmdLine*);
cmdLine *parseCmdLines(const char *strLine);

void printPath(){
  char path_name[PATH_MAX];
  getcwd(path_name, PATH_MAX);  //get current working directory into path_name
  fprintf(stdout, "%s\n",path_name);
}

int specialCommand(cmdLine* command){
  int isSpecial = 0;
  if(strcmp(command->arguments[0], "quit") == 0){
    isSpecial = 1;
    exit(EXIT_SUCCESS);
  }
  if(strcmp(command->arguments[0], "cd") == 0){
    isSpecial = 1;
    if(chdir(command->arguments[1]) < 0)
      perror("bad cd command");
  }
  return isSpecial;
}

void execute(cmdLine* pCmdLine){
  int input = STDIN;
  int ouput = STDOUT;
  if(!specialCommand(pCmdLine)) {
    int childPid;
    if(!(childPid = fork())) {
      if(pCmdLine->inputRedirect) {
        close(input);
        if(!fopen(pCmdLine->inputRedirect, "r")) {
          perror(strerror(errno));
          exit(EXIT_FAILURE);
        }
      }
      if(pCmdLine->outputRedirect){
        close(ouput);
        if(!fopen(pCmdLine-> outputRedirect, "w+")){
          perror(strerror(errno));
          exit(EXIT_FAILURE);
        }
      }
      if(pCmdLine->next!=NULL)
          pipeProc(pCmdLine);
      else if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
          perror(strerror(errno));
          _exit(EXIT_FAILURE);
      }
    }
    if(debug)
      fprintf(stderr, "PID: %d\nExecuting command: %s\n", childPid,pCmdLine->arguments[0]);
    if(pCmdLine->blocking)
      waitpid(childPid, NULL, 0);
  }
}

/*handling single piped command*/
void pipeProc(cmdLine* input_command){
    pid_t child1_pid, child2_pid;
    int p[2];
    if (pipe(p)==-1){
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
    child1_pid = fork();
    if (child1_pid==-1){
        int error_code = errno;
        perror(strerror(error_code));
        exit(EXIT_FAILURE);
    }
    if (!child1_pid){
        close(STDOUT);
        dup2(p[1],STDOUT);
        close(p[1]);
        if(execvp(input_command->arguments[0], input_command->arguments) < 0)
          perror(strerror(errno));
    }
    else {
        close(p[1]);
        child2_pid = fork();
        if (child2_pid==-1){
            perror(strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(!child2_pid) {
            close(STDIN);
            dup2(p[0],STDIN);
            close(p[0]);
            if(execvp(input_command->next->arguments[0] ,input_command->next->arguments) < 0)
              perror(strerror(errno));
        }
        else {
            close(p[0]);
            waitpid(child1_pid, NULL, 0);
            waitpid(child2_pid, NULL, 0);
        }
    }
}

int main(int argc, char const *argv[]) {
  FILE* input = stdin;
  char buffer[BUFFER_SIZE];
  debug = 0;
  for(int i=1; i<argc; i++) {
    if((strcmp("-D", argv[i]) == 0))
      debug = 1;
  }
  while(1){
    printPath();
    fgets(buffer, BUFFER_SIZE, input);
    cmdLine* line = parseCmdLines(buffer);
    execute(line);
    freeCmdLines(line);
  }
  return 0;
}