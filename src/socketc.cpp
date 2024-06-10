#include "../inc/socketc.h"
#include <iostream>

using namespace std;

void ClientSocket::init() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }

    ClientSocket::clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(host);
    serverAddr.sin_port = htons(PORT);

    if (connect(ClientSocket::clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << "\n";
        return;
    }

    cout << "Connected to server.\n";

    ClientSocket::mainThread = new thread(&ClientSocket::handleResponse, &ClientSocket::clientSocket);
}

void ClientSocket::handleResponse(void* arg) {
    SOCKET clientSocket = *((SOCKET *)arg);
    int byte_recv = 0;
    ResponseData data;
    while ((byte_recv = recv(clientSocket, (char*)& data, sizeof(ResponseData), 0)) > 0) {
        // close socket
        if (data.type == CLOSE_SOCKET)
            break;

        // handle request
        switch (data.type) {
            case LOBBY:
                ClientSocket::handleLobbyEvent(clientSocket, data.size);
                break;
            case CREATEROOM:
                ClientSocket::handleRoomCreate(clientSocket, data.size);
                break;
            case JOINROOM:
                break;
            case DELETEROOM:
                ClientSocket::handleRoomDelete(clientSocket, data.size);
                break;
            case GAMESYNC:
                break;
        }
    }
}

void ClientSocket::handleLobbyEvent(SOCKET clientSocket, int size) {
    if (size) {
        Room* roomdata = new Room[size];
        recv(clientSocket, (char*)& roomdata, sizeof(Room) * size, 0);
        for (int i = 0; i < size; i++) {
            gameLobby.push(roomdata[i].id, roomdata[i]);
        }
    }
}

void ClientSocket::handleRoomCreate(SOCKET clientSocket, int size) {
    if (size) {
        Room roomdata;
        recv(clientSocket, (char*)& roomdata, sizeof(Room) * size, 0);
        gameLobby.push(roomdata.id, roomdata);
        gameLobby.printLobby();
    }
}

void ClientSocket::handleRoomDelete(SOCKET clientSocket, int id) {
    gameLobby.erase(id);
    cout << "[INFO] ROOM DELETE" << endl;
}

void ClientSocket::sendRequest(char uid[UID_LENGTH], RequestType type, int roomId = -1) {
    RequestData data = {type, ""};
    strcpy(data.uid, uid);

    if (roomId != -1)
        data.roomid = roomId;
        
    send(ClientSocket::clientSocket, (char*) &data, sizeof(data), 0);
}

void ClientSocket::clear() {
    closesocket(clientSocket);
    WSACleanup();
}

void ClientSocket::stopThread() {
    if (mainThread) {
        ClientSocket::mainThread->join();
        ClientSocket::clear();
    }
}
