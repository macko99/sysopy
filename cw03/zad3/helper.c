#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <linux/limits.h>
#include <stdbool.h>
#include "matrixUtils.c"

bool check_multiply_correctness(char *aFileName, char *bFileName, char *cFileName){
    matrix a = matrixFromFile(aFileName);
    matrix b = matrixFromFile(bFileName);
    matrix c = matrixFromFile(cFileName);
    matrix modelMatrix = multiplyModelMatrices(a, b);

    if(modelMatrix.colsCount != c.colsCount || modelMatrix.rowsCount != c.rowsCount) {
        return false;
    }
    for(int i=0; i < modelMatrix.rowsCount; i++){
        for(int j=0; j < modelMatrix.colsCount; j++){
            if(modelMatrix.values[i][j] != c.values[i][j]){
                return false;
            }
        }
    }
    freeMatrix(&a);
    freeMatrix(&b);
    freeMatrix(&c);
    freeMatrix(&modelMatrix);
    return true;
}

int main(int argc, char** argv){
    char *mode = argv[1];
    if(strcmp(mode, "create") == 0){
        srand(time(NULL));
        int leftHandLimit = atoi(argv[2]);
        int rightHandLimit = atoi(argv[3]);
        int howMany = atoi(argv[4]);
        system("mkdir files");

        for(int i=0; i < howMany; i++){
            int aRowsNumber = rand() % (rightHandLimit - leftHandLimit + 1) + leftHandLimit;
            int aColsNumber = rand() % (rightHandLimit - leftHandLimit + 1) + leftHandLimit;
            int bColsNumber = rand() % (rightHandLimit - leftHandLimit + 1) + leftHandLimit;
            char *aFileName = calloc(100, sizeof(char));
            char *bFileName = calloc(100, sizeof(char));
            char *cFileName = calloc(100, sizeof(char));
            sprintf(aFileName, "files/a%d.txt", i);
            sprintf(bFileName, "files/b%d.txt", i);
            sprintf(cFileName, "files/c%d.txt", i);

            generateMatrix(aRowsNumber, aColsNumber, aFileName);
            generateMatrix(aColsNumber, bColsNumber, bFileName);

            char *command = calloc(1000, sizeof(char));
            sprintf(command, "echo \"%s %s %s\" >> lista", aFileName, bFileName, cFileName);
            system(command);
        }
    }
    else if(strcmp(mode, "check") == 0){
        char aFileName[PATH_MAX + 1];
        char bFileName[PATH_MAX + 1];
        char cFileName[PATH_MAX + 1];
        strcpy(aFileName, argv[2]);
        strcpy(bFileName, argv[3]);
        strcpy(cFileName, argv[4]);
        bool modelIsOK = check_multiply_correctness(aFileName, bFileName, cFileName);
        if(modelIsOK){
            puts("mnozenie ok");
        }
        else{
            puts("error -> mnozenie nie ok");
        }
    }
    else{
        fprintf(stderr, "wrong arg");
    }
    return 0;
}
