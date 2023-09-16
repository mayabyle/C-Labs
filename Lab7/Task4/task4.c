#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/fcntl.h>

#define STDIN 0
#define STDOUT 1
#define HISTLEN  20
#define BUFFER_SIZE 2048

typedef struct circularQueue{
    char* arr[HISTLEN];
    int newest;
    int start;
    int full;
}circularQueue;

circularQueue* my_history;

char name[PATH_MAX];
int debug = 0;
void execute(cmdLine* pCmdLine, int debag);


char* getNewest(circularQueue* history){
    return history->arr[history->newest];
}

char* getstart(circularQueue* history){
    return history->arr[history->start];
}

char* getHistoryAt(circularQueue* history, int n) {
    if (n <= history->newest) 
        return history->arr[n];
    return NULL;
}

void addHistory(circularQueue* history, char* new) {
    int index = (history->newest + 1) % HISTLEN;
    if(index == history->start) {
        history->start = (index + 1) % HISTLEN;
        history->full = 1;
    }
    history->arr[index] = (char*)malloc(strlen(new));
    strcpy(history->arr[index], new);
    history->newest = index;
}

void printHistory(circularQueue* history,int n) {
    int max, curr = 1;
    if(history->full)
        max = HISTLEN-1;
    else
        max = history->newest;
    while(max >= curr){
        printf("       %d. %s",curr, history->arr[curr]);
        curr++;
    }
}

int specialCommand(cmdLine* command) {
    int isSpecial = 0;
    if(strcmp(command->arguments[0],"cd")==0 && command->argCount > 1){
        isSpecial=1;
        if(chdir(command->arguments[1]) == 0)
            getcwd(name,PATH_MAX);
        else
            perror("");
        
    }
    if(strcmp(command->arguments[0],"quit")==0){
        isSpecial=1;
        _exit(EXIT_SUCCESS);
    }
    if(strcmp(command->arguments[0], "history") == 0){
        isSpecial = 1;
        int n = my_history->newest - my_history->start;
        if(command->arguments[1] != NULL)
            n = atoi(command->arguments[1]);
        printHistory(my_history, n);
    }
    if(strcmp(command->arguments[0], "!!") == 0){
        isSpecial = 1;
        char* cmdStr = getNewest(my_history);
        printf("%s\n", cmdStr);
        struct cmdLine* cmd = parseCmdLines(cmdStr);
        execute(cmd, debug);
    }
    else if(command->arguments[0][0] == '!' && command->arguments[0][1] > 48 && command->arguments[0][1] <57){
        isSpecial = 1;
        int n;
        sscanf(command->arguments[0], "!%d", &n);
        char* cmdStr = getHistoryAt(my_history, n);
        if (cmdStr != NULL){
            printf("%s\n", cmdStr);
            struct cmdLine* cmd = parseCmdLines(cmdStr);
            execute(cmd, debug);
        }
    }
    return isSpecial;
}

void Error(){
    perror(strerror(errno));
    _exit(EXIT_FAILURE);
}

void pipeCommands(cmdLine* input){
    int p[2];
    pid_t childPid1, childPid2;
    if (pipe(p)==-1)
        Error("");
    childPid1 = fork();
    if (childPid1==-1)
        Error();
    if (!childPid1){
        close(STDOUT);
        dup2(p[1],STDOUT);
        close(p[1]);
        if(execvp(input->arguments[0] ,input->arguments)<0)
            perror(strerror(errno));
    }
    else {
        close(p[1]);
        childPid2 = fork();
        if (childPid2==-1)
            Error();

        if(!childPid2){
            close(STDIN);
            dup2(p[0],STDIN);
            close(p[0]);
            if (execvp(input->next->arguments[0] ,input->next->arguments)<0)
                perror(strerror(errno));
        }
        else {
            close(p[0]);
            waitpid(childPid1,NULL,0);
            waitpid(childPid2,NULL,0);
        }
    }
}

void execute(cmdLine* pCmdLine, int debug){
    int outputFile = STDOUT;
    int inputFile = STDIN;
    if(!specialCommand(pCmdLine)){
        int child;
        if(!(child=fork())){
            if(pCmdLine != NULL && pCmdLine->outputRedirect != NULL) {
                close(STDOUT);
                outputFile = open(pCmdLine->outputRedirect, O_CREAT | O_WRONLY,0777);
                if(outputFile < 0)
                    Error("");
            }
            if(pCmdLine->next != NULL && pCmdLine->next->outputRedirect != NULL){
                close(STDOUT);
                outputFile = open(pCmdLine->next->outputRedirect,O_CREAT | O_RDWR,0777);
                if(outputFile < 0)
                    Error("");
            }
            if(pCmdLine->inputRedirect != NULL) {
                close(STDIN);
                inputFile = open(pCmdLine->inputRedirect, O_CREAT | O_RDWR, 0777);
                if(inputFile < 0)
                    Error("");
            }
            if(pCmdLine->next != NULL)
                pipeCommands(pCmdLine);

            else if(execvp(pCmdLine->arguments[0], pCmdLine->arguments) < 0)
                Error("can not execute");

            _exit(0);
        }
        if(pCmdLine->blocking)
            waitpid(child,NULL,0);
        if(debug)
            fprintf(stderr, "PID: %d\nexecuting the command: %s\n",child,pCmdLine->arguments[0]);
    }
}

int main(int argc, char const *argv[]) {
    FILE* inputFile = stdin;
    char buf[BUFFER_SIZE];
    my_history = calloc(1, sizeof(circularQueue));
    for(int i=1 ; i<argc ; i++){
        if((strcmp("-d",argv[i])==0))
            debug=1;
    }
    my_history->full=0;
    getcwd(name,PATH_MAX);
    fprintf(stdout,"%s\n",name);
    while(fgets(buf,BUFFER_SIZE,inputFile)){
        if(buf != NULL && buf[0] != '!' && strlen(buf) > 1)
            addHistory(my_history, buf);
        if(strlen(buf) > 1) {
            cmdLine* newLine = parseCmdLines(buf);
            execute(newLine,debug);
            freeCmdLines(newLine);
        }
        fprintf(stdout,"%s\n",name);
    }
    return 0;
}
