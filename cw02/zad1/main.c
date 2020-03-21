#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>

#define sys_mode 1
#define lib_mode 0

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int generate(char *path, int amount, int len) {
    FILE *file = fopen(path, "w+");
    FILE *random = fopen("/dev/urandom", "r");
    char *buffer = malloc(len * sizeof(char) + 1);

    for (int i = 0; i < amount; ++i) {
        fread(buffer, sizeof(char), (size_t) len + 1, random);
        for (int j = 0; j < len; ++j) {
            buffer[j] = (char) (abs(buffer[j]) % 25 + 'a');
        }
        buffer[len] = 10;
        fwrite(buffer, sizeof(char), (size_t) len + 1, file);
    }
    fclose(file);
    fclose(random);
    return 0;
}

int lib_sort(char *path, int amount, int len) {
    FILE *file = fopen(path, "r+");
    char *reg1 = malloc((len + 1) * sizeof(char));
    char *reg2 = malloc((len + 1) * sizeof(char));

    long int offset = (long int) ((len + 1) * sizeof(char));

    for (int i = 0; i < amount; i++) {
        fseek(file, i * offset, 0);
        fread(reg1, sizeof(char), (size_t)(len + 1), file);

        for (int j = 0; j < i; j++) {
            fseek(file, j * offset, 0);
            fread(reg2, sizeof(char), (size_t)(len + 1), file);

            if (reg2[0] > reg1[0]) {
                fseek(file, j * offset, 0);
                fwrite(reg1, sizeof(char), (size_t)(len + 1), file);

                fseek(file, i * offset, 0);
                fwrite(reg2, sizeof(char), (size_t)(len + 1), file);

                char *tmp = reg1;
                reg1 = reg2;
                reg2 = tmp;
            }
        }
    }
    fclose(file);
    return 0;
}

int sys_sort(char *path, int amount, int len) {
    int file = open(path, O_RDWR);
    char *reg1 = malloc((len + 1) * sizeof(char));
    char *reg2 = malloc((len + 1) * sizeof(char));
    long int offset = (long int) ((len + 1) * sizeof(char));

    for (int i = 0; i < amount; i++) {
        lseek(file, i * offset, SEEK_SET);

        read(file, reg1, (size_t)(len + 1) * sizeof(char));

        for (int j = 0; j < i; j++) {
            lseek(file, j * offset, SEEK_SET);
            read(file, reg2, sizeof(char) * (len + 1));
            if (reg2[0] > reg1[0]) {
                lseek(file, j * offset, 0);
                write(file, reg1, sizeof(char) * (len + 1));

                lseek(file, i * offset, 0);
                write(file, reg2, sizeof(char) * (len + 1));

                char *tmp = reg1;
                reg1 = reg2;
                reg2 = tmp;
            }
        }
    }
    close(file);
    return 0;
}


void sort_mode(char *path, int amount, int len, int mode) {
    if (mode == lib_mode) {
        lib_sort(path, amount, len);
    }
    else {
        sys_sort(path, amount, len);
    }
}

int sys_copy(char *path, char *dest, int amount, int len){
    int source = open(path, O_RDONLY);
    int target = open(dest, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    char *tmp = malloc(len * sizeof(char));

    for (int i = 0; i < amount; i++){
        read(source, tmp, (size_t) (len + 1) * sizeof(char));
        write(target, tmp, (size_t) (len + 1) * sizeof(char));
    }
    close(source);
    close(target);
    return 0;
}

int lib_copy(char *path, char *dest, int amount, int len) {
    FILE *source = fopen(path, "r");
    FILE *target = fopen(dest, "w+");
    char *tmp = malloc(len * sizeof(char));

    for (int i = 0; i < amount; i++){
        fread(tmp, sizeof(char), (size_t) (len + 1), source);
        fwrite(tmp, sizeof(char), (size_t) (len + 1), target);
    }

    fclose(source);
    fclose(target);
    return 0;
}

void copy_mode(char *source, char *destination, int amount, int buffer, int mode) {
    if (mode == lib_mode) {
        lib_copy(source, destination, amount, buffer);
    }
    else {
        sys_copy(source, destination, amount, buffer);
    }
}

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("wrong number of arguments");
        return 1;
    }

    char* name = malloc(128*sizeof(char));
    FILE *report_file = fopen("wyniki.txt", "a");


    struct tms **tms_time = malloc(2 * sizeof(struct tms *));
    clock_t real_time[6];
    for (int i = 0; i < 2; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms *));
    }

    real_time[0] = times(tms_time[0]);

    if (strcmp(argv[1], "generate") == 0) {
        int amount = atoi(argv[3]);
        int len = atoi(argv[4]);
        strcpy(name, "generate: ");
        strcat(name, argv[3]);
        strcat(name, " : ");
        strcat(name, argv[4]);
        generate(argv[2], amount, len);
    }
    else if (strcmp(argv[1], "sort") == 0) {
        if (argc < 6) {
            printf("wrong number of arguments");
            return 1;
        }
        int amount = atoi(argv[3]);
        int len = atoi(argv[4]);
        if (strcmp(argv[5], "sys") == 0) {
            sort_mode(argv[2], amount, len, sys_mode);
        }
        else if (strcmp(argv[5], "lib") == 0) {
            sort_mode(argv[2], amount, len, lib_mode);
        }
        else {
            printf("unknown sort mode");
        }
        strcpy(name, "sort: ");
        strcat(name, argv[3]);
        strcat(name, " : ");
        strcat(name, argv[4]);
        strcat(name, " : ");
        strcat(name, argv[5]);
    }
    else if (strcmp(argv[1], "copy") == 0) {
        if (argc < 7) {
            printf("wrong number of arguments");
            return 1;
        }
        int amount = atoi(argv[4]);
        int len = atoi(argv[5]);
        if (strcmp(argv[6], "sys") == 0) {
            copy_mode(argv[2], argv[3], amount, len, sys_mode);
        }
        else if (strcmp(argv[6], "lib") == 0) {
            copy_mode(argv[2], argv[3], amount, len, lib_mode);

        }
        else {
            printf("uknown copy mode");
        }
        strcpy(name, "copy: ");
        strcat(name, argv[4]);
        strcat(name, " : ");
        strcat(name, argv[5]);
        strcat(name, " : ");
        strcat(name, argv[6]);
    }
    else {
        printf("unknon command!");
    }

    real_time[1] = times(tms_time[1]);

    fprintf(report_file, "%30s\t\t%15f\t%15f\t%15f\t\n",
            name,
            calculate_time(real_time[0], real_time[1]),
            calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
            calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    return 0;
}
