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
#define MIN_VALUE 1
#define MAX_VALUE 100
#define rand_int (rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE)
#define rand_time ((rand() % (MAX_SLEEP - MIN_SLEEP + 1) + MIN_SLEEP) * 1000)

typedef struct{
    int values[MAX_ORDERS];
} orders;

int get_semaphore();
int get_shared_memory();

#endif
