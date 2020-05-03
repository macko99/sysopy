#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "shared.h"

#define WORKER1 3
#define WORKER2 3
#define WORKER3 3

pid_t pids[WORKER1 + WORKER2 + WORKER3];

void clear(){
    for (int i = 0; i < 6; i++){
        if (sem_unlink(SEMAPHORES[i]) < 0){
            printf("Cannot unlink semaphore %d\n", errno);
            exit(-1);
        }
      }
    if (shm_unlink(SHARED_MEMORY)){
        printf("Cannot unlink shared memory %d\n", errno);
        exit(-1);
    }
}

void handle_SIGINT(int signum){
    for (int i = 0; i < WORKER1 + WORKER2 + WORKER3; i++){
        kill(pids[i], SIGINT);
    }
    clear();
    exit(0);
}

void set_semaphores(){
    sem_t *fd = sem_open(SEMAPHORES[0], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (fd == SEM_FAILED){
        printf("Cannot cretate controll semaphore %d\n", errno);
        exit(-1);
    }
    sem_close(fd);

    for (int i = 1; i < 6; i++){
        fd = sem_open(SEMAPHORES[i], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
        if (fd == SEM_FAILED){
            printf("Cannot create semaphore %d\n", errno);
            exit(-1);
        }
        sem_close(fd);
    }
}

void set_shared_memory(){
    int fd = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd < 0){
        printf("Cannot create shared memory %d\n", errno);
        exit(-1);
    }

    if (ftruncate(fd, sizeof(orders)) < 0){
        printf("Cannot truncate segment size %d\n", errno);
        exit(-1);
    }
}

void run_workers(){
    for (int i = 0; i < WORKER1; i++){
        pid_t child_pid = fork();
        if (child_pid == 0){
            execlp("./worker1", "worker1", NULL);
        }
        pids[i] = child_pid;
    }

    for (int i = 0; i < WORKER2; i++){
        pid_t child_pid = fork();
        if (child_pid == 0){
            execlp("./worker2", "worker2", NULL);
        }
        pids[i + WORKER1] = child_pid;
    }

    for (int i = 0; i < WORKER3; i++){
        pid_t child_pid = fork();
        if (child_pid == 0){
            execlp("./worker3", "worker3", NULL);
        }
        pids[i + WORKER1 + WORKER2] = child_pid;
    }

    for (int i = 0; i < WORKER1 + WORKER2 + WORKER3; i++){
        wait(NULL);
    }
}

int main(){
    signal(SIGINT, handle_SIGINT);
    set_semaphores();
    set_shared_memory();
    run_workers();
    clear();
}
