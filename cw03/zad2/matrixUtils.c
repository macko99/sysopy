#define MAX_COLS_NUMBER 1000
#define MAX_LINE_LENGTH (MAX_COLS_NUMBER * 5)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

typedef struct {
    int** values;
    int rowsCount;
    int colsCount;
} matrix;

int getColsCount(char* row) {
    int cols = 0;
    char* value = strtok(row, " ");
    while (value != NULL) {
        if(strcmp(value, "\n") != 0){
            cols++;
        }
        value = strtok(NULL, " ");
    }
    return cols;
}

void setRowsAndCols(FILE* fileName, int* rows, int* cols) {
    char oenLine[MAX_LINE_LENGTH];
    *rows = 0;
    *cols = 0;
    while (fgets(oenLine, MAX_LINE_LENGTH, fileName) != NULL) {
        if (*cols == 0) {
            *cols = getColsCount(oenLine);
        }
        *rows = *rows + 1;
    }
    fseek(fileName, 0, SEEK_SET);
}

matrix matrixFromFile(char* fileName) {
    FILE *matrixFile = fopen(fileName, "r");
    int rows, cols;
    setRowsAndCols(matrixFile, &rows, &cols);
    int** values = calloc(rows, sizeof(int*));
    for (int y = 0; y < rows; y++) {
        values[y] = calloc(cols, sizeof(int));
    }
    int currentX, currentY = 0;
    char oneLine[MAX_LINE_LENGTH];
    while (fgets(oneLine, MAX_LINE_LENGTH, matrixFile) != NULL) {
        currentX = 0;
        char* value = strtok(oneLine, " ");
        while (value != NULL) {
            values[currentY][currentX++] = atoi(value);
            value = strtok(NULL, " ");
        }
        currentY++;
    }
    fclose(matrixFile);
    matrix newMatrix;
    newMatrix.values = values;
    newMatrix.rowsCount = rows;
    newMatrix.colsCount = cols;
    return newMatrix;
}

void generateMatrix(int rows, int cols, char* fileName) {
    FILE* matrixFile = fopen(fileName, "w+");
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x > 0) {
                fprintf(matrixFile, " ");
            }

            fprintf(matrixFile, "%d", rand() % (200 + 1) - 100);
        }
        fprintf(matrixFile, "\n");
    }
    fclose(matrixFile);
}

void matrixToFile(FILE *file, matrix a){
    fseek(file, 0, SEEK_SET);
    for (int y = 0; y < a.rowsCount; y++) {
        for (int x = 0; x < a.colsCount; x++) {
            if (x > 0) {
                fprintf(file, " ");
            }
            fprintf(file, "%d", a.values[y][x]);
        }
        fprintf(file, "\n");
    }
}

void freeMatrix(matrix* m) {
    for (int y = 0; y < m->rowsCount; y++) {
        free(m->values[y]);
    }
    free(m->values);
}

matrix multiplyModelMatrices(matrix a, matrix b){
    int** values = calloc(a.rowsCount, sizeof(int*));
    for (int y = 0; y < a.rowsCount; y++) {
        values[y] = calloc(b.colsCount, sizeof(int));
    }
    for (int i = 0; i < a.rowsCount; i++) {
        for (int j = 0; j < b.colsCount; j++) {
            int value = 0;
            for(int k=0; k<a.colsCount; k++){
                value+= (a.values[i][k] * b.values[k][j]);
            }
            values[i][j] = value;
        }
    }
    matrix newMatrix;
    newMatrix.values = values;
    newMatrix.rowsCount = a.rowsCount;
    newMatrix.colsCount = b.colsCount;
    return newMatrix;
}
