#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

const char date_format[] = "%Y-%m-%d %H:%M:%S";
char follow_mode;
char *command;
time_t follow_date;
int max_depth;

void get_info_from_stat(const char *filename, const struct stat *statptr){
    char file_type[64] = "undefined";
    if (S_ISREG(statptr->st_mode)) {
        strcpy(file_type, "file");
    }
    else if (S_ISDIR(statptr->st_mode)) {
        strcpy(file_type, "dir");
    }
    else if (S_ISLNK(statptr->st_mode)) {
        strcpy(file_type, "slink");
    }
    else if (S_ISCHR(statptr->st_mode)) {
        strcpy(file_type, "char dev");
    }
    else if (S_ISBLK(statptr->st_mode)) {
        strcpy(file_type, "block dev");
    }
    else if (S_ISFIFO(statptr->st_mode)) {
        strcpy(file_type, "fifo");
    }
    else if (S_ISSOCK(statptr->st_mode)) {
        strcpy(file_type, "socket");
    }

    struct tm modification_time;
    localtime_r(&statptr->st_mtime, &modification_time);

    char modification_time_string[255];
    strftime(modification_time_string, 255, date_format, &modification_time);

    struct tm access_time;
    localtime_r(&statptr->st_atime, &access_time);

    char access_time_string[255];
    strftime(access_time_string, 255, date_format, &access_time);

    printf("%s | | type: %s -> size: %ld -> m.time: %s -> a.time: %s -> links: %ld\n", filename, file_type, statptr->st_size, modification_time_string, access_time_string, statptr->st_nlink);
}

int file_info(const char *filename, const struct stat *stat_ptr, int flags, struct FTW *ftw_ptr)
{
    if (max_depth != -1 && ftw_ptr->level > max_depth){
        return 0;
    }
    if (strcmp(command, "maxdepth") == 0){
        get_info_from_stat(filename, stat_ptr);
    }
    else if (strcmp(command, "mtime") == 0){
        time_t modification_time = stat_ptr->st_mtime;
        int time_difference = difftime(follow_date, modification_time);
        if (!((time_difference == 0 && follow_mode == '=') || (time_difference > 0 && follow_mode == '+') || (time_difference < 0 && follow_mode == '-'))) {
            return 0;
        }
        get_info_from_stat(filename, stat_ptr);
    }
    else if (strcmp(command, "atime") == 0){
        time_t access_time = stat_ptr->st_atime;
        int time_difference = difftime(follow_date, access_time);
        if (!((time_difference == 0 && follow_mode == '=') || (time_difference > 0 && follow_mode == '+') || (time_difference < 0 && follow_mode == '-'))) {
            return 0;
        }
        get_info_from_stat(filename, stat_ptr);
    }
    return 0;
}

int main(int argc, char *argv[]){
    if (argc < 4) {
        printf("wrong number of arguments");
        return 1;
    }

    char* root_path = argv[1];
    command = argv[2];

    if (strcmp(command, "maxdepth") == 0){
        max_depth = atoi(argv[3]);
        nftw(root_path, file_info, 2, FTW_PHYS);
    }
    else{
        if (argc < 6) {
            printf("wrong number of arguments");
            return 1;
        }

        strcpy(&follow_mode, argv[3]);
        max_depth = atoi(argv[5]);

        time_t rawtime;
        time(&rawtime);
        struct tm *timeinfo = localtime(&rawtime);
        timeinfo->tm_mday -= atoi(argv[4]);
        follow_date = mktime(timeinfo);

        nftw(root_path, file_info, 2, FTW_PHYS);

    }
    return 0;
}
