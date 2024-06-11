#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H
#define PORT 8080
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "data.h"

#pragma comment(lib, "ws2_32.lib")

const char* host = "192.168.56.1";

Lobby gameLobby;
char auth_uid[UID_LENGTH];
namespace ClientSocket {
    WSADATA wsaData;
    SOCKET clientSocket;
    std::thread* mainThread;
    
    void init();

    // handleEvent
    void handleResponse(void*);
    void handleLobbyEvent(SOCKET, int);
    void handleRoomCreate(SOCKET, int);
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