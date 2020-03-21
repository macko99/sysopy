#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#define M_TIME 0
#define A_TIME 1
const char date_format[] = "%Y-%m-%d %H:%M:%S";

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

void max_depth(char *root_path, int maxdepth_parameter)
{
    if (maxdepth_parameter == 0 || root_path == NULL) {
        return;
    }

    DIR *directory = opendir(root_path);
    if (directory == NULL){
        fprintf(stderr, "error opening directory: %s\n", strerror(errno));
        exit(-1);
    }

    struct dirent *file;
    char path_builder[256];

    while ((file = readdir(directory)) != NULL){
        strcpy(path_builder, root_path);
        strcat(path_builder, "/");
        strcat(path_builder, file->d_name);

        struct stat file_stats;
        if (lstat(path_builder, &file_stats) < 0){
            fprintf(stderr, "unable to lstat file %s: %s\n", path_builder, strerror(errno));
            exit(-1);
        }

        if (S_ISDIR(file_stats.st_mode)){
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                continue;
            }
            max_depth(path_builder, maxdepth_parameter - 1);
        }

        get_info_from_stat(path_builder, &file_stats);
    }
    closedir(directory);
}

void whatever_time(char *root_path, char mode, int maxdepth_parameter, time_t date, int mORaTIME) {
    if (maxdepth_parameter == 0 || root_path == NULL){
        return;
    }

    DIR *directory = opendir(root_path);
    if (directory == NULL){
        fprintf(stderr, "error opening directory: %s\n", strerror(errno));
        exit(-1);
    }

    struct dirent *file;
    char path_builder[256];

    while ((file = readdir(directory)) != NULL){
        strcpy(path_builder, root_path);
        strcat(path_builder, "/");
        strcat(path_builder, file->d_name);

        struct stat file_stats;
        if (lstat(path_builder, &file_stats) < 0){
            fprintf(stderr, "unable to lstat file %s: %s\n", path_builder, strerror(errno));
            exit(-1);
        }

        if (S_ISDIR(file_stats.st_mode)){
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                continue;
            }
            whatever_time(path_builder, mode, maxdepth_parameter - 1, date, mORaTIME);
        }

        time_t modification_time;
        if(mORaTIME == M_TIME){
            modification_time = file_stats.st_mtime;
        }
        else{
            modification_time = file_stats.st_atime;
        }

        int time_difference = difftime(date, modification_time);

        if (!((time_difference == 0 && mode == '=') || (time_difference > 0 && mode == '+') || (time_difference < 0 && mode == '-'))) {
            continue;
        }

        get_info_from_stat(path_builder, &file_stats);
    }
    closedir(directory);
}

int main(int argc, char const *argv[]) {
  if (argc < 4) {
    printf("wrong number of arguments");
    return 1;
  }

  char* root_path = malloc(128*sizeof(char));
  char* command = malloc(16*sizeof(char));
  strcpy(root_path, argv[1]);
  strcpy(command, argv[2]);

  if (strcmp(command, "maxdepth") == 0){
    max_depth(root_path, atoi(argv[3]));
  }
  else{
    if (argc < 6) {
      printf("wrong number of arguments");
      return 1;
    }
    char mode_parameter;
    strcpy(&mode_parameter, argv[3]);
    int days_parameter = atoi(argv[4]);
    int maxdepth_parameter = atoi(argv[5]);

    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    timeinfo->tm_mday -= days_parameter;

    if (strcmp(command, "mtime") == 0){
      if (maxdepth_parameter >= 0){
          whatever_time(root_path, mode_parameter, maxdepth_parameter, mktime(timeinfo), M_TIME);
      }
      else{
          whatever_time(root_path, mode_parameter, -1, mktime(timeinfo), M_TIME);
      }
    }
    else if (strcmp(command, "atime") == 0){
      if (maxdepth_parameter >= 0){
          whatever_time(root_path, mode_parameter, maxdepth_parameter, mktime(timeinfo), A_TIME);
      }
      else{
          whatever_time(root_path, mode_parameter, -1, mktime(timeinfo), A_TIME);
      }
    }
  }
  return 0;
}
