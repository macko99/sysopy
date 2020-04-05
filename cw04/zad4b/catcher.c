#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef enum Mode
{
    KILL,
    QUEUE,
    SIGRT
} Mode;

Mode mode;
int signal_int;
int end_signal_int;
int received_counter = 0;
union sigval value;

void handle_signals(int sig, siginfo_t *info, void *ucontext){
    if (sig == signal_int){
        received_counter++;

        if (mode == KILL || mode == SIGRT){
            kill(info->si_pid, signal_int);
        }
        else{
            sigqueue(info->si_pid, signal_int, value);
        }
    }
    else if (sig == end_signal_int){
        printf("catcher received: %d signals\n", received_counter);
        if (mode == KILL || mode == SIGRT){
            kill(info->si_pid, end_signal_int);
        }
        else{
            sigqueue(info->si_pid, end_signal_int, value);
        }
        exit(0);
    }
}

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "usage: [mode]\n");
        exit(1);
    }
    char *mode_str = argv[1];

    if (strcmp("kill", mode_str) == 0){
        mode = KILL;
        signal_int = SIGUSR1;
        end_signal_int = SIGUSR2;
    }
    else if (strcmp("queue", mode_str) == 0){
        mode = QUEUE;
        signal_int = SIGUSR1;
        end_signal_int = SIGUSR2;
    }
    else if (strcmp("sigrt", mode_str) == 0){
        mode = SIGRT;
        signal_int = SIGRTMIN + 1;
        end_signal_int = SIGRTMIN + 2;
    }
    else{
        fprintf(stderr, "error\n");
        exit(1);
    }

    value.sival_int = 0;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, signal_int);
    sigdelset(&mask, end_signal_int);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0){
        perror("cannot block sig\n");
        exit(1);
    }

    struct sigaction sa_handle;
    sa_handle.sa_flags = SA_SIGINFO;
    sa_handle.sa_sigaction = handle_signals;
    sigemptyset(&sa_handle.sa_mask);
    sigaddset(&sa_handle.sa_mask, signal_int);
    sigaddset(&sa_handle.sa_mask, end_signal_int);

    sigaction(signal_int, &sa_handle, NULL);
    sigaction(end_signal_int, &sa_handle, NULL);
    printf("created has started, PID: %d\n", getpid());

    while (1){
        usleep(100);
    }
}
