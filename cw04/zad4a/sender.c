#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef enum Mode{
    KILL,
    QUEUE,
    SIGRT
} Mode;

Mode mode;
int signal_num;
int end_signal_num;
int to_send;
int received_counter;

void handle_signals(int sig, siginfo_t *info, void *ucontext){
    if (sig == signal_num){
        received_counter++;
        if (mode == QUEUE){
            printf("counter: %d, catcher index: %d\n", received_counter, info->si_value.sival_int);
        }
    }
    else if (sig == end_signal_num){
        printf("sender received: %d signals, should receive: %d\n", received_counter, to_send);
        exit(0);
    }
}

int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "usage: [catcher PID] [sigs num] [mode]\n");
    }

    int catcher_PID = atoi(argv[1]);
    to_send = atoi(argv[2]);
    char *mode_string = argv[3];

    if (strcmp("kill", mode_string) == 0){
        mode = KILL;
        signal_num = SIGUSR1;
        end_signal_num = SIGUSR2;
    }
    else if (strcmp("queue", mode_string) == 0){
        mode = QUEUE;
        signal_num = SIGUSR1;
        end_signal_num = SIGUSR2;
    }
    else if (strcmp("sigrt", mode_string) == 0){
        mode = SIGRT;
        signal_num = SIGRTMIN + 1;
        end_signal_num = SIGRTMIN + 2;
    }
    else{
        printf("error");
        exit(-1);
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, signal_num);
    sigdelset(&mask, end_signal_num);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0){
        perror("cannot block sig\n");
        exit(-1);
    }

    struct sigaction sa_handle;
    sa_handle.sa_flags = SA_SIGINFO;
    sa_handle.sa_sigaction = handle_signals;
    sigemptyset(&sa_handle.sa_mask);
    sigaddset(&sa_handle.sa_mask, signal_num);
    sigaddset(&sa_handle.sa_mask, end_signal_num);

    sigaction(signal_num, &sa_handle, NULL);
    sigaction(end_signal_num, &sa_handle, NULL);
    printf("sender has started, PID: %d\n", getpid());

    if (mode == KILL || mode == SIGRT){
        for (int i = 0; i < to_send; ++i){
            kill(catcher_PID, signal_num);
        }
        kill(catcher_PID, end_signal_num);
    }
    else{
        union sigval value;
        value.sival_int = 0;
        for (int i = 0; i < to_send; ++i){
            sigqueue(catcher_PID, signal_num, value);
        }
        sigqueue(catcher_PID, end_signal_num, value);
    }

    while (1){
        usleep(100);
    }
}
