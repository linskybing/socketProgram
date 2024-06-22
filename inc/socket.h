#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "data.h"
#include "database_server.h"
#define PORT 7658

#define PARALLEL 5

using namespace std;
Lobby gameLobby;

namespace GameSocket {
    int serverSocket;
    int status = 1;

    pthread_t mainThread;

    std::map<int, std::string> clientSockets;
    std::map<std::string, int> clientSockets_r;

    void init();
    void* start(void*);
    void pushSocket(int, string uid);

    // handle function
    void* handleClient(void*);
    void handleLobbyEvent(int, char[UID_LENGTH]);

    void handleRoomCreate(char*, char*);
    void handleRoomDelete(int);
    void handleRoomJoin(char*, int);
    void handleRoomLeave(const char*, int);
    void handleGameSync(RequestData data);
    void handleGameStart(RequestData data);
    void quickSend(RequestData data);
    void handleMapSelect(RequestData data);
    void checkAndAssgin(char*);
    // clear function
    void disconnetion(int socket);
    void clear();

}
#endif