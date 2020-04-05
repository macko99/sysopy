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
int catcher_pid;
int to_send;
int received_counter;
int sent_counter = 0;
union sigval value;

void send_signal(){
    sent_counter++;
    if (mode == KILL || mode == SIGRT){
        kill(catcher_pid, signal_int);
    }
    else{
        sigqueue(catcher_pid, signal_int, value);
    }
}

void send_end_signal()
{
    if (mode == KILL || mode == SIGRT){
        kill(catcher_pid, end_signal_int);
    }
    else{
        sigqueue(catcher_pid, end_signal_int, value);
    }
}

void sig_handler(int sig, siginfo_t *info, void *ucontext){
    if (sig == signal_int){
        received_counter++;
        if (sent_counter < to_send){
            send_signal();
        }
        else{
            send_end_signal();
        }
    }
    else if (sig == end_signal_int){
        printf("sender received: %d signals, should receive: %d\n", received_counter, to_send);
        exit(0);
    }
}

int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "usage: [catcher PID] [num sig to send] [mode]\n");
    }
    catcher_pid = atoi(argv[1]);
    to_send = atoi(argv[2]);
    char *mode_string = argv[3];

    if (strcmp("kill", mode_string) == 0){
        mode = KILL;
        signal_int = SIGUSR1;
        end_signal_int = SIGUSR2;
    }
    else if (strcmp("queue", mode_string) == 0){
        mode = QUEUE;
        signal_int = SIGUSR1;
        end_signal_int = SIGUSR2;
    }
    else if (strcmp("sigrt", mode_string) == 0){
        mode = SIGRT;
        signal_int = SIGRTMIN + 1;
        end_signal_int = SIGRTMIN + 2;
    }
    else{
        fprintf(stderr, "error\n");
        exit(1);
    }

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
    sa_handle.sa_sigaction = sig_handler;
    sigemptyset(&sa_handle.sa_mask);
    sigaddset(&sa_handle.sa_mask, signal_int);
    sigaddset(&sa_handle.sa_mask, end_signal_int);

    sigaction(signal_int, &sa_handle, NULL);
    sigaction(end_signal_int, &sa_handle, NULL);
    printf("sender has started, pid: %d\n", getpid());

    send_signal();

    while (1){
        usleep(100);
    }
}
