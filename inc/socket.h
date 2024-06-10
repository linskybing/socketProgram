#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "data.h"

#define PORT 8080

#define PARALLEL 5

using namespace std;
Lobby gameLobby;

namespace GameSocket {
    int serverSocket;
    int status = 1;

    pthread_t mainThread;

    std::map<int, std::string> clientSockets;

    void init();
    void* start(void*);
    void pushSocket(int, string uid);

    // handle function
    void* handleClient(void*);
    void handleLobbyEvent(int, char[UID_LENGTH]);

    void handleRoomCreate(int);
    void handleDeleteRoom(int);

    // clear function
    void disconnetion(int socket);
    void clear();

}
#endif