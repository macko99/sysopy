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
#include <time.h>
#include "shared.h"

void handle_SIGINT(int signum){
    for (int i = 0; i < 6; i++){
        if (sem_close(semaphores[i]) < 0){
            printf("Cannot close semaphore %d\n", errno);
            exit(-1);
        }
    }
    exit(0);
}

int get_value(int index){
    int value;
    sem_getvalue(semaphores[index], &value);
    return value;
}

void pack_order(){
    sem_wait(semaphores[0]);
    sem_post(semaphores[2]);
    sem_wait(semaphores[3]);

    orders *order = mmap(NULL, sizeof(orders), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_descriptor, 0);
    if (order == (void *)-1){
        printf("Cannot map shared memory %d\n", errno);
        exit(-1);
    }

    int index = (get_value(2) - 1) % MAX_ORDERS;
    order->values[index] *= 2;
    int orders_to_prepare = get_value(3);
    int orders_to_send = get_value(5) + 1;
    printf("[%d | %ld] -> packed: %d | ready to pack: %d | ready to send: %d\n",
           getpid(), time(NULL), order->values[index], orders_to_prepare, orders_to_send);

    if (munmap(order, sizeof(orders)) < 0){
        printf("Cannot unmap shared memory %d\n", errno);
        exit(-1);
    }

    sem_post(semaphores[0]);
    sem_post(semaphores[5]);
}

void init_semaphores(){
    for (int i = 0; i < 6; i++){
        semaphores[i] = sem_open(SEMAPHORES[i], O_RDWR);

        if (semaphores[i] < 0){
            printf("Cannot open semaphore %d\n", errno);
            exit(-1);
        }
    }
}

void init_shared_memory(){
    shared_memory_descriptor = shm_open(SHARED_MEMORY, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

    if (shared_memory_descriptor < 0){
        printf("Cannot open shared memory %d\n", errno);
        exit(-1);
    }
}

int main()
{
    srand(time(NULL));
    signal(SIGINT, handle_SIGINT);

    init_semaphores();
    init_shared_memory();

    while (1){
        usleep(rand_time);
        if (get_value(3) > 0){
            pack_order();
        }
    }
}
