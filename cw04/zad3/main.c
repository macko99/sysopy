#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

void child_handler(int pid, siginfo_t *info, void *ucontext) {
    printf("signal number %d\n", info->si_signo);
    printf("sending process pid %d\n", info->si_pid);
    printf("child code %d\n", info->si_status);
}

void sigint_handler(int pid, siginfo_t *info, void *ucontext) {
    printf("signal number %d\n", info->si_signo);
    printf("sending process pid %d\n", info->si_pid);

    if (info->si_code == SI_USER){
        printf("USER\n");
    }
    else if (info->si_code == SI_KERNEL){
        printf("KERNEL\n");
    }
    else if (info->si_code == SI_QUEUE){
        printf("QUEUE\n");
    }
}

void queue_handler(int pid, siginfo_t *info, void *ucontext)
{
    printf("signal number %d\n", info->si_signo);
    printf("sending process pid %d\n", info->si_pid);
    printf("code %d\n", info->si_value.sival_int);
}

int main(int argc, char **argv){
    if (argc < 2){
        printf("usage: ./main [child/status/queue]");
        exit(-1);
    }

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    if (strcmp(argv[1], "child") == 0){
        action.sa_sigaction = child_handler;
        sigaction(SIGCHLD, &action, NULL);
        pid_t child_pid = fork();

        if (child_pid == 0){
            exit(666);
        }
        wait(NULL);

    }
    else if (strcmp(argv[1], "queue") == 0){
        action.sa_sigaction = queue_handler;
        sigaction(SIGINT, &action, NULL);
        union sigval sigval;
        sigval.sival_int = 777;

        sigqueue(getpid(), SIGINT, sigval);

    }
    else if (strcmp(argv[1], "sigint") == 0){
        action.sa_sigaction = sigint_handler;
        sigaction(SIGINT, &action, NULL);
        union sigval sigval;

        sigqueue(getpid(), SIGINT, sigval);
    }

    return 0;
}
