#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

const int commandsLimit = 32;
const int argsLimit = 16;

void stringToArray(char **args, char *cmd){
    char *arg = strtok_r(cmd, " ", &cmd);
    int arg_counter = 0;
    while (arg != NULL)
    {
        args[arg_counter++] = arg;
        arg = strtok_r(NULL, " ", &cmd);
    }
}

void execute_command(char *line)
{
    char *commands[commandsLimit][argsLimit];
    for (int i = 0; i < commandsLimit; ++i){
        for (int j = 0; j < argsLimit; ++j){
            commands[i][j] = NULL;
        }
    }

    char *commandToken = strtok_r(line, "|", &line);
    int commandCounter = 0;
    while (commandToken != NULL){
        stringToArray(commands[commandCounter++], commandToken);
        commandToken = strtok_r(NULL, "|", &line);
    }

    int pipes[commandsLimit][2];
    for (int i = 0; i < commandCounter - 1; i++){
        if (pipe(pipes[i]) < 0)
        {
            fprintf(stderr, "cannot make pipe\n");
            exit(-1);
        }
    }

    for (int i = 0; i < commandCounter; i++){
        pid_t pid = fork();
        if (pid < 0){
            fprintf(stderr, "fork failed\n");
            exit(-1);
        }
        else if (pid == 0){
            if (i > 0){
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i + 1 < commandCounter){
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < commandCounter - 1; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(commands[i][0], commands[i]);
            exit(0);
        }
    }
    for (int j = 0; j < commandCounter - 1; ++j){
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
    for (int i = 0; i < commandCounter; ++i){
        wait(0);
    }
}

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "usage: [file]\n");
        exit(-1);
    }

    char *filePath = argv[1];
    FILE *f = fopen(filePath, "r");
    if (f == NULL){
        fprintf(stderr, "cannot find or open file %s\n", filePath);
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = (char*)calloc(sizeof(char), fileSize + 1);
    if (fread(buffer, 1, fileSize, f) != fileSize){
        fprintf(stderr, "cannot read from file %s\n", filePath);
        exit(-1);
    }
    fclose(f);

    char *lineToken = strtok_r(buffer, "\n", &buffer);
    while (lineToken != NULL){
        execute_command(lineToken);
        lineToken = strtok_r(NULL, "\n", &buffer);
    }
    return 0;
}
