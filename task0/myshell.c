#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include "line_parser.h"
#include <unistd.h>

int execute(cmd_line *line){
    int ret = execvp(line->arguments[0], line->arguments);
    printf("%d", ret);
    return ret;
}

int main(int argc, char **argv){
    while (1){
        char str[2048];
        char cwd[PATH_MAX];

        if (NULL == getcwd(cwd, PATH_MAX)){
            perror("Error in getcwd(): ");
            return 1;
        }
        printf("%s/", cwd);

        if (NULL == fgets(str, 2048, stdin)){
            perror("Error in fgets(): ");
            return 1;
        }

        cmd_line *line = parse_cmd_lines(str);
        if (0 == strcmp("quit", line->arguments[0])){
            free_cmd_lines(line);
            return 0;
        }

        int ret = execute(line);
        if (0 > ret){
            perror("Execution error: ");
            free_cmd_lines(line);
            return 1;
        }
        free_cmd_lines(line);
    }
    return 0;
}