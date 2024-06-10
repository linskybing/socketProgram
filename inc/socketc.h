#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H
#define PORT 8080
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "data.h"

#pragma comment(lib, "ws2_32.lib")

const char* host = "127.0.0.1";

Lobby gameLobby;

namespace ClientSocket {
    WSADATA wsaData;
    SOCKET clientSocket;
    std::thread* mainThread;

    void init();

    // handleEvent
    void handleResponse(void*);
    void handleLobbyEvent(SOCKET, int);
    void handleRoomCreate(SOCKET, int);
    void handleRoomDelete(SOCKET, int);

    void sendRequest(char[UID_LENGTH], RequestType, int roomid = -1);

    // clear
    void clear();
    void stopThread();
}

#endif