#ifndef int_CLIENT_LINUX_H
#define int_CLIENT_LINUX_H
#define PORT 8080
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include "data.h"


const char* host = "127.0.0.1";

Lobby gameLobby;
char auth_uid[UID_LENGTH];
namespace ClientSocket {
    int clientSocket;
    pthread_t mainThread;
    
    void init();

    // handleEvent
    void* handleResponse(void*);
    void handleLobbyEvent(int, int);
    void handleRoomCreate(int, int);
    void handleRoomDelete(int);
    void handleRoomJoin(char*, int);
    void handleRoomLeave(char*, int);
    void sendRequest(char[UID_LENGTH], RequestType, int);

    // clear
    void clear();
    void stopThread();
}

namespace GameManager {
    int currentRoom = -1;

    void enterExistRoom(int);
}
#endif