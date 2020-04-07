#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "usage: [pipe] [file path] [char count]");
        exit(-1);
    }

    char *pipePath = argv[1];
    char *filePath = argv[2];
    int charLimit = atoi(argv[3]);

    FILE *file = fopen(filePath, "w");
    if (file == NULL){
        fprintf(stderr, "cannot open or read file: %s", filePath);
        exit(-1);
    }

    FILE *pipe = fopen(pipePath, "r");
    if (pipe == NULL){
        fprintf(stderr, "pipe file failed: %s", pipePath);
        exit(-1);
    }

    char buffer[charLimit];
    while (fgets(buffer, charLimit, pipe) != NULL){
        fprintf(file, buffer, strlen(buffer));
    }
    fclose(pipe);
    fclose(file);
    return 0;
}
