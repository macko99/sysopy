#define _POSIX_C_SOURCE 200112L
#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MESSAGE_LENGTH 256
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct{
    char *name;
    int fd;
    int isOnline;
} player;

player *players[MAX_PLAYERS] = {NULL};
int playersCount = 0;

int getRival(int index){
    return index + 1 ? index % 2 == 0 : index - 1;
}

int addPlayer(char *name, int fd){
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i] != NULL && strcmp(players[i]->name, name) == 0) {
            return -1;
        }
    }
    int index = -1;
    for (int i = 0; i < MAX_PLAYERS; i += 2){
        if (players[i] != NULL && players[i + 1] == NULL){
            index = i + 1;
            break;
        }
    }
    if (index == -1){
        for (int i = 0; i < MAX_PLAYERS; i++){
            if (players[i] == NULL){
                index = i;
                break;
            }
        }
    }
    if (index != -1){
        player *newPlayer = calloc(1, sizeof(player));
        newPlayer->name = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        strcpy(newPlayer->name, name);
        newPlayer->fd = fd;
        newPlayer->isOnline = 1;
        players[index] = newPlayer;
        playersCount++;
    }
    return index;
}

int getIndexFromName(char *name){
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (players[i] != NULL && strcmp(players[i]->name, name) == 0){
            return i;
        }
    }
    return -1;
}

void removePlayer(int index)
{
    free(players[index]->name);
    free(players[index]);
    players[index] = NULL;
    playersCount--;
    int rival = getRival(index);
    if (players[rival] != NULL){
        printf("Removing rival: %s\n", players[rival]->name);
        send(players[rival]->fd, "quit", MAX_MESSAGE_LENGTH, 0);
        free(players[rival]->name);
        free(players[rival]);
        players[rival] = NULL;
        playersCount--;
    }
}

void ping(){
    while (1){
        printf("*PINGING*\n");
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_PLAYERS; i++){
            if (players[i] != NULL && !players[i]->isOnline){
                int index = getIndexFromName(players[i]->name);
                printf("Removing client: %s\n", players[i]->name);
                removePlayer(index);
            }
        }
        for (int i = 0; i < MAX_PLAYERS; i++){
            if (players[i] != NULL){
                send(players[i]->fd, "ping: ", MAX_MESSAGE_LENGTH, 0);
                players[i]->isOnline = 0;
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(3);
    }
}

int initLocalSocket(char *path){
    int localSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un sockaddr;
    memset(&sockaddr, 0, sizeof(struct sockaddr_un));
    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, path);
    unlink(path);
    bind(localSocket, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_un));
    listen(localSocket, MAX_BACKLOG);
    return localSocket;
}

int initNetworkSocket(char *port){
    struct addrinfo *info;
    struct addrinfo addrinfo;
    memset(&addrinfo, 0, sizeof(struct addrinfo));
    addrinfo.ai_family = AF_UNSPEC;
    addrinfo.ai_socktype = SOCK_STREAM;
    addrinfo.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &addrinfo, &info);
    int networkSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(networkSocket, info->ai_addr, info->ai_addrlen);
    listen(networkSocket, MAX_BACKLOG);
    freeaddrinfo(info);
    return networkSocket;
}

int messageReceiver(int localSocket, int networkSocket){
    struct pollfd *fds = calloc(2 + playersCount, sizeof(struct pollfd));
    fds[0].fd = localSocket;
    fds[0].events = POLLIN;
    fds[1].fd = networkSocket;
    fds[1].events = POLLIN;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < playersCount; i++){
        fds[i + 2].fd = players[i]->fd;
        fds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);
    poll(fds, playersCount + 2, -1);
    int value;
    for (int i = 0; i < playersCount + 2; i++){
        if (fds[i].revents & POLLIN){
            value = fds[i].fd;
            break;
        }
    }
    if (value == localSocket || value == networkSocket){
        value = accept(value, NULL, NULL);
    }
    free(fds);
    return value;
}

int main(int argc, char *argv[]){
    srand(time(NULL));
    if (argc != 3){
        printf("USAGE: ./server [port] [path]\n");
        printf("ERRNO: %d\n", errno);
        exit(1);
    }
    char *port = argv[1];
    char *path = argv[2];
    int localSocket = initLocalSocket(path);
    int networkSocket = initNetworkSocket(port);
    pthread_t t;
    pthread_create(&t, NULL, (void *(*)(void *))ping, NULL);

    while (1){
        int playerFD = messageReceiver(localSocket, networkSocket);
        char buffer[MAX_MESSAGE_LENGTH + 1];
        recv(playerFD, buffer, MAX_MESSAGE_LENGTH, 0);
        printf("%s\n", buffer);
        char *command = strtok(buffer, ":");
        char *arg = strtok(NULL, ":");
        char *name = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);

        if (strcmp(command, "add") == 0) {
            int index = addPlayer(name, playerFD);
            if (index == -1) {
                send(playerFD, "add:name_taken", MAX_MESSAGE_LENGTH, 0);
                close(playerFD);
            } else if (index % 2 == 0) {
                send(playerFD, "add:no_enemy", MAX_MESSAGE_LENGTH, 0);
            } else {
                int first = index;
                int second = getRival(index);
                if (((int) rand() % 100) % 2 == 0) {
                    int tmp = first;
                    first = second;
                    second = tmp;
                }
                send(players[first]->fd, "add:O", MAX_MESSAGE_LENGTH, 0);
                send(players[second]->fd, "add:X", MAX_MESSAGE_LENGTH, 0);
            }
        }
        if (strcmp(command, "move") == 0){
            int move = atoi(arg);
            int player = getIndexFromName(name);
            sprintf(buffer, "move:%d", move);
            send(players[getRival(player)]->fd, buffer, MAX_MESSAGE_LENGTH,0);
        }
        if (strcmp(command, "quit") == 0){
            int index = getIndexFromName(name);
            printf("Removing client: %s\n", name);
            removePlayer(index);
        }
        if (strcmp(command, "pong") == 0){
            int player = getIndexFromName(name);
            if (player != -1){
                players[player]->isOnline = 1;
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

