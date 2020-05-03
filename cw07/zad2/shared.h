#ifndef SHARED_H
#define SHARED_H

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

#define MAX_ORDERS 5
#define MIN_SLEEP 100
#define MAX_SLEEP 1000
#define MIN_VAL 1
#define MAX_VAL 100
#define SHARED_MEMORY "/SHARED_MEMORY"
#define rand_int (rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL)
#define rand_time ((rand() % (MAX_SLEEP - MIN_SLEEP + 1) + MIN_SLEEP) * 1000)

int shared_memory_descriptor;
const char *SEMAPHORES[6] = {"/CONTROLLER", "/INDEX", "/ORDER_INDEX", "/ORDERS_TO_PREPARE", "/SEND_INDEX", "/ORDERS_TO_SEND"};
sem_t *semaphores[6];
typedef struct{
    int values[MAX_ORDERS];
} orders;

#endif
