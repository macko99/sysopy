#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

void dir_tree()
{
    DIR *root_path = opendir(".");
    if (root_path == NULL){
        fprintf(stderr, "cannot open dir, errno: %s\n", strerror(errno));
        exit(-1);
    }
    struct dirent *file;

    while ((file = readdir(root_path)) != NULL){
        struct stat file_stats;

        if (lstat(file->d_name, &file_stats) < 0){
            fprintf(stderr, "cannot lstat file, errno: %s\n", strerror(errno));
            exit(-1);
        }
        if (S_ISDIR(file_stats.st_mode)){
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                continue;
            }
            if (chdir(file->d_name) != 0){
                fprintf(stderr, "cannot chdir, errno: %s\n", strerror(errno));
                exit(-1);
            }

            dir_tree();
            pid_t child_process = fork();

            if (child_process < 0){
                fprintf(stderr, "cannot fork, errno: %s\n", strerror(errno));
                exit(-1);
            }
            else if (child_process == 0){
                char current_dir[4096];

                if (getcwd(current_dir, 4096) == NULL){
                    fprintf(stderr, "cannot get current dir, errno: %s", strerror(errno));
                    exit(-1);
                }
                printf("dir: %s, PID: %d\n", current_dir, getpid());

                int exec_status = execlp("ls", "ls", "-l", NULL);
                if (exec_status != 0){
                    fprintf(stderr, "cannot exec, errno: %s\n", strerror(errno));
                    exit(-1);
                }
                exit(exec_status);
            }
            else{
                wait(0);
            }
            if (chdir("..") != 0){
                fprintf(stderr, "cannot chdir, errno: %s\n", strerror(errno));
                exit(-1);
            }
        }
    }

    closedir(root_path);
}

int main(int argc, char *argv[])
{
    if (argc != 2){
        fprintf(stderr, "specify correct path!\n");
        exit(-1);
    }

    if (chdir(argv[1]) != 0){
        fprintf(stderr, "cannot chdir to %s\n", argv[1]);
        exit(-1);
    }
    dir_tree();

    return 0;
}
