#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include "line_parser.h"

void execute(cmd_line *line){
    int pid = fork();
    if (0 == pid){
        int ret = execvp(line->arguments[0], line->arguments);
        if (0 > ret){
            perror("Execution error: ");
            _exit(pid);
        }
    }
}

int main(int argc, char **argv){
    while (1){
        char str[2048];
        char cwd[PATH_MAX];

        if (NULL == getcwd(cwd, PATH_MAX)){
            perror("Error in getcwd(): ");
            return 1;
        }
        printf("%s$:", cwd);

        if (NULL == fgets(str, 2048, stdin)){
            perror("Error in fgets(): ");
            return 1;
        }

        cmd_line *line = parse_cmd_lines(str);
        if (0 == strcmp("quit", line->arguments[0])){
            free_cmd_lines(line);
            return 0;
        }

        execute(line);
        free_cmd_lines(line);
    }
    return 0;
}