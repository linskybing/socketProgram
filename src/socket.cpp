#include "../inc/socket.h"
#include <iostream>
using namespace std;

void GameSocket::init() {
    GameSocket:serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    bind(GameSocket::serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    listen(GameSocket::serverSocket, 5);
    pthread_create(&GameSocket::mainThread, nullptr, &GameSocket::start, nullptr);
}

void* GameSocket::start(void*) {
    while (true) {
        int clientSocket = accept(GameSocket::serverSocket, nullptr, nullptr);
        pthread_t thread;
        pthread_create(&thread, nullptr, &GameSocket::handleClient, &clientSocket);
        pthread_detach(thread);
        if (status == 0) {
            GameSocket::clear();
            break;
        }
    }
    pthread_exit(nullptr);
}

void* GameSocket::handleClient(void* arg) {
    int clientSocket = *((int*) arg);
    int byte_recv = 0;
    RequestData data;
    while ((byte_recv = recv(clientSocket, &data, sizeof(RequestData), 0)) > 0) {
        // close socket
        if (data.type == CLOSE_SOCKET)
            break;

        // handle request
        switch (data.type) {
            case LOBBY:
                GameSocket::handleLobbyEvent(clientSocket, data.uid);
                break;
            case CREATEROOM:
                GameSocket::handleRoomCreate();
            case DELETEROOM:
                GameSocket::handleDeleteRoom(data.roomid);
            case JOINROOM:
                break;
            case GAMESYNC:
                break;
        }
    }

    GameSocket::disconnetion(clientSocket);

    pthread_exit(nullptr);
    return nullptr;
}

void GameSocket::handleLobbyEvent(int clientSocket, char uid[UID_LENGTH]) {

    cout << "[Lobby] " << uid << " enter lobby" << endl;
    // push user data
    string s = uid;
    GameSocket::pushSocket(clientSocket, s);

    // send lobby information to user
    ResponseData data = {LOBBY, gameLobby.size};

    send(clientSocket, (char*) &data, sizeof(data), 0);
}

void GameSocket::pushSocket(int p, string uid) {
    GameSocket::clientSockets[p] = uid;
    cout << clientSockets[p] << endl;
}

void GameSocket::disconnetion(int socket) {
    cout << "[INFO] " << clientSockets[socket] << " disconnetion" << endl;
    clientSockets.erase(socket);
}

void GameSocket::clear() {
    pthread_join(GameSocket::mainThread, NULL);
    close(GameSocket::serverSocket);
}

void GameSocket::handleRoomCreate() {

    Room nRoom = {gameLobby.size++, "test create"};
    gameLobby.push(nRoom.id, nRoom);

    ResponseData data = {CREATEROOM, 1};

    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
        send((it.first), (char*) &nRoom, sizeof(Room), 0);
    }
}

void GameSocket::handleRoomDelete(int id) {
    gameLobby.erase(id);
    ResponseData data = {DELETEROOM, id};

    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
    }

    cout << "[INFO] ROOM " << id << " DELETED\n";
}