#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "usage: [file]\n");
        exit(-1);
    }

    char *filePath = argv[1];
    char line[200];
    sprintf(line, "cat %s | sort", filePath);
    FILE *file = popen(line, "w");
    pclose(file);

    return 0;
}
