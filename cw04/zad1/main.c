#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int isNextTime = 0;

void sigint_handler(){
    printf("\nreceived SIGINT\n");
    exit(0);
}

void sigtstp_handler(){
    if (isNextTime){
        isNextTime = 0;
        return;
    }

    printf("\ninput CTRL+Z to continue or CTR+C to terminate \n");
    isNextTime = 1;
}

int main()
{
    signal(SIGINT, sigint_handler);

    struct sigaction action;
    action.sa_handler = sigtstp_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGTSTP, &action, NULL);

    while (1){
        if (!isNextTime){
            system("ls -la");
            sleep(1);
        }
        else{
            pause();
        }
    }
}
