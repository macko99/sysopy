#define _XOPEN_SOURCE 500
#define MAX_COLS_NUMBER 1000
#define MAX_LINE_LENGTH (MAX_COLS_NUMBER * 5)
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "matrixUtils.c"
#include <sys/resource.h>

int pairCount = 0;
struct Task{
    int pairID;
    int colID;
};

void setLimits(char const * timeLimit, char const * memoryLimit){
    long int timeLim = strtol(timeLimit, NULL, 10);
    long int memLim = 1048576 * strtol(memoryLimit, NULL, 10);

    struct rlimit cpuRLimit;
    struct rlimit memRLimit;

    cpuRLimit.rlim_max = (rlim_t) timeLim;
    cpuRLimit.rlim_cur = (rlim_t) timeLim;
    setrlimit(RLIMIT_CPU, &cpuRLimit);

    memRLimit.rlim_max = (rlim_t) memLim;
    memRLimit.rlim_cur = (rlim_t) memLim;
    setrlimit(RLIMIT_AS, &memRLimit);
}

long int getUsage(struct timeval * t){
    return (long int)t->tv_sec * 1000000 + (long int)t->tv_usec;
}

void reportUsage(struct rusage usage1, struct rusage usage2){
    long int userTime = abs(getUsage(&usage2.ru_utime) - getUsage(&usage1.ru_stime));
    long int systemTime = abs(getUsage(&usage2.ru_stime) - getUsage(&usage1.ru_stime));
    printf("user CPU time used: %lf\n", (double)userTime / 1000000);
    printf("system CPU time used: %lf\n\n", (double)systemTime / 1000000);
}

struct Task getTask() {
    struct Task task;
    task.colID = -1;
    task.pairID = -1;
    for(int i=0; i < pairCount; i++){

        char* taskFileName = calloc(100, sizeof(char));
        sprintf(taskFileName, ".tmp/tasks%d", i);
        FILE* taskFile = fopen(taskFileName, "r+");
        int fd = fileno(taskFile);
        flock(fd, LOCK_EX);

        char* tasks = calloc(1000, sizeof(char));
        fseek(taskFile, 0, SEEK_SET);
        fread(tasks, 1, 1000, taskFile);

        char* firstZeroPtr = strchr(tasks, '0');
        int taskID = firstZeroPtr != NULL ? firstZeroPtr - tasks : -1;

        if (taskID >= 0) {
            char* lineEnd = strchr(tasks, '\0');
            int size = lineEnd - tasks;

            char* correctSizeTasks = calloc(size + 1, sizeof(char));
            for(int j=0; j<size; j++){
                correctSizeTasks[j] = tasks[j];
            }

            correctSizeTasks[taskID] = '1';
            fseek(taskFile, 0, SEEK_SET);
            fwrite(correctSizeTasks, 1, size, taskFile);
            task.pairID = i;
            task.colID = taskID;
            flock(fd, LOCK_UN);
            fclose(taskFile);
            break;
        }
        flock(fd, LOCK_UN);
        fclose(taskFile);
    }
    return task;
}

void multiplyColumn(char* aFileName, char* bFileName, int colID, int pairID) {
    matrix a = matrixFromFile(aFileName);
    matrix b = matrixFromFile(bFileName);
    char* fileName = calloc(20, sizeof(char));
    sprintf(fileName, ".tmp/part%d%04d", pairID, colID);
    FILE* partialFile = fopen(fileName, "w+");

    for (int y = 0; y < a.rowsCount; y++) {
        int result = 0;
        for (int x = 0; x < a.colsCount; x++) {
            result += a.values[y][x] * b.values[x][colID];
        }
        if(y == a.rowsCount - 1 ) fprintf(partialFile, "%d ", result);
        else fprintf(partialFile, "%d \n", result);
    }
    fclose(partialFile);
}

void multiplyColumn_oneFile(char* aFilename, char* bFilename, int colID, char *resFileName) {
    matrix a = matrixFromFile(aFilename);
    matrix b = matrixFromFile(bFilename);
    FILE* file = fopen(resFileName, "r+");
    int fd = fileno(file);
    flock(fd, LOCK_EX);
    matrix c = matrixFromFile(resFileName);

    for (int y = 0; y < a.rowsCount; y++) {
        int result = 0;
        for (int x = 0; x < a.colsCount; x++) {
            result += a.values[y][x] * b.values[x][colID];
        }
        c.values[y][colID] = result;
    }
    matrixToFile(file, c);
    flock(fd, LOCK_UN);
    fclose(file);
}

int workToDo(char** aMatrices, char** bMatrices, int timeLimit, int mode, char **resMatrices) {
    time_t startTime = time(NULL);
    int multipliesCounter = 0;
    while (1) {
        if ((time(NULL) - startTime) >= timeLimit) {
            puts("timeLimit exceeded ");
            break;
        }
        struct Task task = getTask();
        if (task.colID == -1) {
            break;
        }
        if(mode == 1) {
            multiplyColumn_oneFile(aMatrices[task.pairID], bMatrices[task.pairID], task.colID, resMatrices[task.pairID]);
        }
        else {
            multiplyColumn(aMatrices[task.pairID], bMatrices[task.pairID], task.colID, task.pairID);
        }
        multipliesCounter++;
    }
    return multipliesCounter;
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        fprintf(stderr, "error -> wrong args number");
        return 1;
    }
    int childrenLimit = atoi(argv[2]);
    int timeLimit = atoi(argv[3]);
    int mode = atoi(argv[4]);
    char **aFileNames = calloc(100, sizeof(char*));
    char **bFileNames = calloc(100, sizeof(char*));
    char **cFileNames = calloc(100, sizeof(char*));
    system("rm -rf .tmp");
    system("mkdir -p .tmp");

    FILE* listFile = fopen(argv[1], "r");
    char oneLine[PATH_MAX * 3 + 3];
    int pairCounter = 0;
    while (fgets(oneLine, PATH_MAX * 3 + 3, listFile) != NULL) {

        aFileNames[pairCounter] = calloc(PATH_MAX, sizeof(char));
        bFileNames[pairCounter] = calloc(PATH_MAX, sizeof(char));
        cFileNames[pairCounter] = calloc(PATH_MAX, sizeof(char));

        strcpy(aFileNames[pairCounter], strtok(oneLine, " "));
        strcpy(bFileNames[pairCounter], strtok(NULL, " "));
        strcpy(cFileNames[pairCounter], strtok(NULL, " "));

        matrix a = matrixFromFile(aFileNames[pairCounter]);
        matrix b = matrixFromFile(bFileNames[pairCounter]);

        if(mode == 1){
            generateMatrix(a.rowsCount, b.colsCount, cFileNames[pairCounter]);
        }
        char* tasksFileName = calloc(100, sizeof(char));
        sprintf(tasksFileName, ".tmp/tasks%d", pairCounter);
        FILE* tasksFile = fopen(tasksFileName, "w+");

        char* tasks = calloc(b.colsCount + 1, sizeof(char));
        sprintf(tasks, "%0*d", b.colsCount, 0);
        fwrite(tasks, 1, b.colsCount, tasksFile);
        free(tasks);
        free(tasksFileName);
        fclose(tasksFile);
        pairCounter++;
    }
    pairCount = pairCounter;

    char *strictTimeLimit = argv[5];
    char *strictMemoryLimit = argv[6];
    pid_t* children = calloc(childrenLimit, sizeof(int));

    for (int i = 0; i < childrenLimit; i++) {
        pid_t working_child = fork();
        if (working_child == 0) {
            setLimits(strictTimeLimit, strictMemoryLimit);
            return workToDo(aFileNames, bFileNames, timeLimit, mode, cFileNames);
        } else {
            children[i] = working_child;
        }
    }

    for (int i = 0; i < childrenLimit; i++) {
        struct rusage usage1;
        struct rusage usage2;
        getrusage(RUSAGE_CHILDREN, &usage1);
        int status;
        waitpid(children[i], &status, 0);
        printf("proces %d wykonal %d mnozen\n", children[i], WEXITSTATUS(status));
        getrusage(RUSAGE_CHILDREN, &usage2);
        reportUsage(usage1, usage2);
    }
    free(children);

    if(mode == 2){
        for(int i=0; i < pairCounter; i++){
            char *buffer = calloc(1000, sizeof(char));
            sprintf(buffer, "paste .tmp/part%d* > %s", i, cFileNames[i]);
            system(buffer);
        }
    }
    return 0;
}
