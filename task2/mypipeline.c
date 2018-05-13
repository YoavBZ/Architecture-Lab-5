#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include "line_parser.h"

int main(int argc, char **argv){

    int fd[2];
    if (-1 == pipe(fd)){
        perror("Pipe error: ");
        return 1;
    }

    int child1 = fork();
    if (0 == child1){
        // Child 1
        fclose(stdout);
        dup(fd[1]);
        close(fd[1]);
        char *args[] = {"ls", "-l", 0};
        execvp(args[0], args);
    } else if (0 < child1){
        // Parent
        close(fd[1]);
    }
    
    int child2 = fork();
    if (0 == child2){
        // Child 2
        fclose(stdin);
        dup(fd[0]);
        close(fd[0]);
        char *args[] = {"tail", "-n", "2", 0};
        execvp(args[0], args);
    } else if (0 < child1){
        // Parent
        close(fd[0]);
    }
    int status1, status2;
    waitpid(child1, &status1, 0);
    waitpid(child2, &status2, 0);

    return 0;
}