#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

void sig_handler(){
    printf("sig handler has received signal\n");
}

int main(int argc, char **argv)
{

    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0)
            perror("cannot block sig");
    }
    else if (strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, sig_handler);
    }
    else if (strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    }

    raise(SIGUSR1);
    sigset_t newmask;

    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigpending(&newmask);
        printf("signal pending: %d\n", sigismember(&newmask, SIGUSR1));
    }

    if (strcmp(argv[2], "exe") == 0){
        execl("./exe", "./exe", argv[1], NULL);
    }
    else{
        pid_t child_pid = fork();
        if (child_pid == 0){
            if (strcmp(argv[1], "pending") != 0){
                raise(SIGUSR1);
            }
            if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
                sigpending(&newmask);
                printf("signal pending: %d\n", sigismember(&newmask, SIGUSR1));
            }
        }
    }

    return 0;
}
