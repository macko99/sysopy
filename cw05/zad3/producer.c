#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "usage: [pipe] [file path] [char count]");
        exit(-1);
    }
    srand(time(NULL));
    char *pipePath = argv[1];
    char *filePath = argv[2];
    int charLimit = atoi(argv[3]);

    FILE *file = fopen(filePath, "r");
    if (file == NULL){
        fprintf(stderr, "cannot open or read file: %s", filePath);
        exit(-1);
    }

    int pipe = open(pipePath, O_WRONLY);
    if (pipe < 0){
        fprintf(stderr, "pipe file failed: %d", pipe);
        exit(-1);
    }

    char buffer[charLimit];
    while (fgets(buffer, charLimit, file) != NULL){
        char text[charLimit + 20];
        sprintf(text, "#%d#%s\n", getpid(), buffer);
        write(pipe, text, strlen(text));
        sleep(rand() % 2 + 1);
    }
    close(pipe);
    fclose(file);
    return 0;
}
