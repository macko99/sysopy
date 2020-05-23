#define _POSIX_C_SOURCE 200112L
#define MAX_MESSAGE_LENGTH 256
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int serverSocket;
int localSocket;
int amIanO;
char buffer[MAX_MESSAGE_LENGTH + 1];
char *name;
typedef enum{
    FREE,
    O,
    X
} figure;

typedef struct{
    int move;
    figure figures[9];
} playGround;

int move(playGround *playGround, int coordinate){
    if (coordinate < 0 || coordinate > 9 || playGround->figures[coordinate] != FREE) {
        return 0;
    }
    playGround->figures[coordinate] = playGround->move ? O : X;
    playGround->move = !playGround->move;
    return 1;
}

figure endCondition(playGround *playGround){
    figure column = FREE;
    for (int x = 0; x < 3; x++){
        figure first = playGround->figures[x];
        figure second = playGround->figures[x + 3];
        figure third = playGround->figures[x + 6];
        if (first == second && first == third && first != FREE)
            return column;
    }

    figure row = FREE;
    for (int y = 0; y < 3; y++){
        figure first = playGround->figures[3 * y];
        figure second = playGround->figures[3 * y + 1];
        figure third = playGround->figures[3 * y + 2];
        if (first == second && first == third && first != FREE)
            return row;
    }

    figure diagonal1 = FREE;
    figure first = playGround->figures[0];
    figure second = playGround->figures[4];
    figure third = playGround->figures[8];
    if (first == second && first == third && first != FREE)
        return diagonal1;

    figure diagonal2 = FREE;
    first = playGround->figures[2];
    second = playGround->figures[4];
    third = playGround->figures[6];
    if (first == second && first == third && first != FREE)
        return diagonal2;

    return FREE;
}

playGround board;
typedef enum{
    PLAYING,
    WAITING,
    WAITING_FOR_MOVE,
    RIVAL_MOVING,
    MOVING,
    QUIT
} Status;

Status status = PLAYING;
char *command, *arg;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void quit(){
    sprintf(buffer, "quit: :%s", name);
    send(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
    exit(0);
}

void checkGameStatus(){
    int win = 0;
    figure winner = endCondition(&board);
    if (winner != FREE){
        if ((amIanO && winner == O) || (!amIanO && winner == X))
            printf("WIN!\n");
        else
            printf("LOST!\n");
        win = 1;
    }

    int draw = 1;
    for (int i = 0; i < 9; i++){
        if (board.figures[i] == FREE){
            draw = 0;
            break;
        }
    }

    if (draw && !win)
        printf("DRAW\n");
    if (win || draw)
        status = QUIT;
}

playGround newPlayGround(){
    playGround playGround = {1, {FREE}};
    return playGround;
}

void drawPlayGround(){
    char shape;
    for (int y = 0; y < 3; y++){
        for (int x = 0; x < 3; x++){
            if (board.figures[y * 3 + x] == FREE){
                shape = y * 3 + x + 1 + '0';
            }
            else if (board.figures[y * 3 + x] == O){
                shape = 'O';
            }
            else{
                shape = 'X';
            }
            printf("  %c  ", shape);
        }
        printf("\n______________________\n");
    }
}

void gameHandler(){
    while (1){
        if (status == PLAYING){
            if (strcmp(arg, "name_taken") == 0){
                printf("Name is already taken\n");
                exit(1);
            }
            else if (strcmp(arg, "no_enemy") == 0){
                printf("Waiting for rival\n");
                status = WAITING;
            }
            else{
                board = newPlayGround();
                amIanO = arg[0] == 'O';
                status = amIanO ? MOVING : WAITING_FOR_MOVE;
            }
        }
        else if (status == WAITING){
            pthread_mutex_lock(&mutex);
            while (status != PLAYING && status != QUIT){
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
            board = newPlayGround();
            amIanO = arg[0] == 'O';
            status = amIanO ? MOVING : WAITING_FOR_MOVE;
        }
        else if (status == WAITING_FOR_MOVE){
            printf("Waiting for rivals move\n");
            pthread_mutex_lock(&mutex);
            while (status != RIVAL_MOVING && status != QUIT){
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        else if (status == RIVAL_MOVING){
            int pos = atoi(arg);
            move(&board, pos);
            checkGameStatus();
            if (status != QUIT){
                status = MOVING;
            }
        }
        else if (status == MOVING){
            drawPlayGround();
            int pos;
            do
            {
                printf("Next move (%c): ", amIanO ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            } while (!move(&board, pos));

            drawPlayGround();
            sprintf(buffer, "move:%d:%s", pos, name);
            send(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
            checkGameStatus();
            if (status != QUIT)
                status = WAITING_FOR_MOVE;
        }
        else if (status == QUIT){
            quit();
        }
    }
}

void initServerConnection(char *destination, int isLocal){
    struct sockaddr_un sockaddr;
    if (isLocal){
        serverSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
        memset(&sockaddr, 0, sizeof(struct sockaddr_un));
        sockaddr.sun_family = AF_UNIX;
        strcpy(sockaddr.sun_path, destination);
        connect(serverSocket, (struct sockaddr *)&sockaddr,sizeof(struct sockaddr_un));

        localSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un sockaddr2;
        memset(&sockaddr2, 0, sizeof(struct sockaddr_un));
        sockaddr2.sun_family = AF_UNIX;
        sprintf(sockaddr2.sun_path, "%d", getpid());
        bind(localSocket, (struct sockaddr *)&sockaddr2,sizeof(struct sockaddr_un));
    }
    else{
        struct addrinfo *info;
        struct addrinfo addrinfo;
        memset(&addrinfo, 0, sizeof(struct addrinfo));
        addrinfo.ai_family = AF_INET;
        addrinfo.ai_socktype = SOCK_DGRAM;
        getaddrinfo("127.0.0.1", destination, &addrinfo, &info);
        serverSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        connect(serverSocket, info->ai_addr, info->ai_addrlen);
        freeaddrinfo(info);
    }

    sprintf(buffer, "add: :%s", name);
    if (isLocal)
        sendto(localSocket, buffer, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_un));
    else
        send(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
}

void messageReceiver(int isLocal){
    int threadRunning = 0;
    while (1){
        if (isLocal)
            recv(localSocket, buffer, MAX_MESSAGE_LENGTH, 0);
        else
            recv(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
        command = strtok(buffer, ":");
        arg = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);

        if (strcmp(command, "add") == 0){
            status = PLAYING;
            if (!threadRunning){
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *)) gameHandler, NULL);
                threadRunning = 1;
            }
        }
        else if (strcmp(command, "move") == 0){
            status = RIVAL_MOVING;
        }
        else if (strcmp(command, "quit") == 0){
            status = QUIT;
            exit(0);
        }
        else if (strcmp(command, "ping") == 0){
            sprintf(buffer, "pong: :%s", name);
            send(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "./client [name] [type] [destination]");
        return 1;
    }
    name = argv[1];
    char *type = argv[2];
    char *destination = argv[3];
    signal(SIGINT, quit);
    int isLocal = strcmp(type, "local") == 0;
    initServerConnection(destination, isLocal);
    messageReceiver(isLocal);
    return 0;
}
