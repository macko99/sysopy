#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    char *consumer[] = {"./consumer", "fif", "./files/consumer", "10", NULL};
    char *producer1[] = {"./producer", "fif", "./files/a", "5", NULL};
    char *producer2[] = {"./producer", "fif", "./files/b", "5", NULL};
    char *producer3[] = {"./producer", "fif", "./files/c", "5", NULL};
    char *producer4[] = {"./producer", "fif", "./files/d", "5", NULL};
    char *producer5[] = {"./producer", "fif", "./files/e", "5", NULL};

    mkfifo("fif", S_IRUSR | S_IWUSR);
    pid_t PIDs[6];

    if ((PIDs[0] = fork()) == 0)
        execvp(consumer[0], consumer);
    if ((PIDs[1] = fork()) == 0)
        execvp(producer1[0], producer1);
    if ((PIDs[2] = fork()) == 0)
        execvp(producer2[0], producer2);
    if ((PIDs[3] = fork()) == 0)
        execvp(producer3[0], producer3);
    if ((PIDs[4] = fork()) == 0)
        execvp(producer4[0], producer4);
    if ((PIDs[5] = fork()) == 0)
        execvp(producer5[0], producer5);

    for (int i = 0; i < 6; i++)
        waitpid(PIDs[i], NULL, 0);

    printf("consumer has created output file: ./files/consumer\n");
    return 0;
}
