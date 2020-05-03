#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int WIDTH;
int HEIGHT;
int **image;
int threadsNumber;
int **histogram;
struct arg_struct{
    int arg1;
    char *mode;
};

void setWidthAndHeight(char *buffer){
    char *width;
    char *height;

    width = strtok_r(buffer, " \t\r\n", &buffer);
    height = strtok_r(buffer, " \t\r\n", &buffer);
    WIDTH = atoi(width);
    HEIGHT = atoi(height);
}

void rowToArray(char *line, int r){
    for (int col = 0; col < WIDTH; col++){
        char* i = strtok_r(line, " \t\r\n", &line);
        image[r][col] = atoi(i);
    }
}

void imageToArray(char *fileName){
    FILE *file = fopen(fileName, "r");
    if (file == NULL){
        fprintf(stderr, "Cannot open file \n");
        exit(-1);
    }

    int maxLength = 128;
    char buffer[maxLength + 1];
    fgets(buffer, maxLength, file);
    do
    {
        fgets(buffer, maxLength, file);
    } while (buffer[0] == '#');

    setWidthAndHeight(buffer);
    fgets(buffer, maxLength, file);
    image = calloc(HEIGHT, sizeof(int *));

    for (int i = 0; i < HEIGHT; i++) {
        image[i] = calloc(WIDTH, sizeof(int));
    }

    char *line = NULL;
    size_t len = 0;
    for (int r = 0; r < HEIGHT; r++){
        getline(&line, &len, file);
        rowToArray(line, r);
    }
    fclose(file);
}

void *number(int index){
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = 255 / threadsNumber;

    for (int w = 0; w < HEIGHT; w++){
        for (int c = 0; c < WIDTH; c++){
            if (image[w][c] / size == index){
                histogram[index][image[w][c]]++;
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}

void *block(int index){
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = WIDTH / threadsNumber;

    for (int col = index * size; col < (index + 1) * size; col++){
        for (int w = 0; w < HEIGHT; w++){
            histogram[index][image[w][col]]++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}

void *cycle(int index){
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int col = index; col < WIDTH; col += threadsNumber){
        for (int w = 0; w < HEIGHT; w++){
            histogram[index][image[w][col]]++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}

void *createHistogram(void *arg){
    struct arg_struct *args = arg;
    if (strcmp(args->mode, "sign") == 0){
        return number(args->arg1);
    }
    else if (strcmp(args->mode, "block") == 0){
        return block(args->arg1);
    }
    else if (strcmp(args->mode, "interleaved") == 0){
        return cycle(args->arg1);
    }
    else{
        printf("wrong mode\n");
        exit(-1);
    }
}
void saveHistogram(char *outputFileName){
    FILE *file = fopen(outputFileName, "w");

    for (int i = 0; i < 256; i++){
        int sum = 0;
        for (int j = 0; j < threadsNumber; j++){
            sum += histogram[j][i];
        }
        fprintf(file, "%d -> %d\n", i, sum);
    }
    fclose(file);
}
int main(int argc, char *argv[])
{
    threadsNumber = atoi(argv[1]);
    char *mode = argv[2];
    char *inputFileName = argv[3];
    char *outputFileName = argv[4];

    imageToArray(inputFileName);

    FILE *timesFile = fopen("times.txt", "a");
    fprintf(timesFile, "mode: %s | threads: %d\n", mode, threadsNumber);

    histogram = calloc(threadsNumber, sizeof(int *));
    for (int i = 0; i < threadsNumber; i++)
        histogram[i] = calloc(256, sizeof(int));

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t *thread_ids = calloc(threadsNumber, sizeof(pthread_t));
    struct arg_struct *args = calloc(threadsNumber, sizeof(struct arg_struct));
    for (int i = 0; i < threadsNumber; i++){
        struct arg_struct arg;
        arg.arg1 = i;
        arg.mode = mode;
        args[i] = arg;
        pthread_create(&thread_ids[i], NULL, createHistogram, (void *) &args[i]);
    }

    for (int i = 0; i < threadsNumber; i++){
        double *y;
        pthread_join(thread_ids[i], (void *)&y);
        printf("thread %d - %lf microseconds\n", i, *y);
        fprintf(timesFile, "thread %d - %lf microseconds\n", i, *y);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    printf("\ntime: %f\n", time);
    fprintf(timesFile, "time: %f\n\n", time);

    saveHistogram(outputFileName);
    fclose(timesFile);
    return 0;
}