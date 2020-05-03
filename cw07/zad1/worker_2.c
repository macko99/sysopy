#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "shared.h"

int semaphore_id;
int shared_memory_id;
typedef struct sembuf sembuf;

void pack_order(){
    sembuf *start = calloc(4, sizeof(sembuf));
    start[0].sem_num = 0;
    start[0].sem_op = 0;
    start[0].sem_flg = 0;

    start[1].sem_num = 0;
    start[1].sem_op = 1;
    start[1].sem_flg = 0;

    start[2].sem_num = 2;
    start[2].sem_op = 1;
    start[2].sem_flg = 0;

    start[3].sem_num = 3;
    start[3].sem_op = -1;
    start[3].sem_flg = 0;
    semop(semaphore_id, start, 4);

    orders *order = shmat(shared_memory_id, NULL, 0);
    int index = (semctl(semaphore_id, 2, GETVAL, NULL) - 1) % MAX_ORDERS;
    order->values[index] *= 2;
    int orders_to_prepare = semctl(semaphore_id, 3, GETVAL, NULL);
    int orders_to_send = semctl(semaphore_id, 5, GETVAL, NULL) + 1;
    printf("[%d | %ld] -> packed: %d | ready to pack: %d | ready to send: %d.\n",
           getpid(), time(NULL), order->values[index], orders_to_prepare, orders_to_send);
    shmdt(order);

    sembuf *stop = calloc(2, sizeof(sembuf));
    stop[0].sem_num = 0;
    stop[0].sem_op = -1;
    stop[0].sem_flg = 0;

    stop[1].sem_num = 5;
    stop[1].sem_op = 1;
    stop[1].sem_flg = 0;
    semop(semaphore_id, stop, 2);
}

int main(){
    srand(time(NULL));

    semaphore_id = get_semaphore();
    shared_memory_id = get_shared_memory();

    while (1){
        usleep(rand_time);
        if (semctl(semaphore_id, 3, GETVAL, NULL) > 0){
            pack_order();
        }
    }
}
