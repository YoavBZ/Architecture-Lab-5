#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include "line_parser.h"

void execute(cmd_line *line){
    int fd[2];
    if (NULL != line->next){
        pipe(fd);
    }
    int child1 = fork();

    if (0 == child1){
        // Child
        if (NULL != line->input_redirect){
            fclose(stdin);
            fopen(line->input_redirect, "r");
        }
        if (NULL != line->output_redirect){
            fclose(stdout);
            fopen(line->output_redirect, "w");
        }
        if (NULL != line->next){
            fclose(stdout);
            dup(fd[1]);
            close(fd[1]);
        }
        int ret = execvp(line->arguments[0], line->arguments);
        if (0 > ret){
            perror("Execution error: ");
            free_cmd_lines(line);
            _exit(child1);
        }
    } else if(0 < child1){
        // Parent
        if (1 == line->blocking) {
            int status;
            waitpid(child1, &status, 0);
        }
        if (NULL != line->next){
            close(fd[1]);
            int child2 = fork();
            if (0 == child2){
                fclose(stdin);
                dup(fd[0]);
                close(fd[0]);
                if (NULL != line->next->output_redirect){
                    fclose(stdout);
                    fopen(line->next->output_redirect, "w");
                }
                int ret = execvp(line->next->arguments[0], line->next->arguments);
                if (0 > ret){
                    perror("Execution error: ");
                    free_cmd_lines(line);
                    _exit(child1);
                }
            } else if (0 < child2){
                //Parent
                close(fd[0]);
                int status1, status2;
                waitpid(child1, &status1, 0);
                waitpid(child2, &status2, 0);
            }
        }
    } else {
        // Handle fork() error
        free_cmd_lines(line);
        _exit(-child1);
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
        printf("%s$: ", cwd);

        if (NULL == fgets(str, 2048, stdin)){
            perror("Error in fgets(): ");
            return 1;
        }
        if ('\n' == *str){
            continue;
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