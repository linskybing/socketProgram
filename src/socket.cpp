#include "../inc/socket.h"
#include <iostream>
#include <cstring>
using namespace std;

void GameSocket::init() {
    GameSocket:serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    bind(GameSocket::serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    listen(GameSocket::serverSocket, 5);
    cout << "server listen ... \n";
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
        cout << "[INFO] Reuqest " << data.uid << " Type: " << data.type << endl;
        ResponseData datar;
        // handle request
        switch (data.type) {
            case LOGIN:
                if (DB::login(data.uid, data.pwd)) {
                    datar.type = LOGIN;
                    datar.auth = SUCCESS;
                }
                else {
                    datar.type = LOGIN;
                    datar.auth = FALE;
                }
                send((clientSocket), (char*) &datar, sizeof(ResponseData), 0);
                break;
            case REGISTER:
                if (!DB::login(data.uid, data.pwd)) {
                    datar.type = REGISTER;
                    datar.auth = SUCCESS;
                    DB::registerUser(data.uid, DB::hash(data.pwd));
                }
                else {
                    datar.type = REGISTER;
                    datar.auth = EXIST;
                }
                send((clientSocket), (char*) &datar, sizeof(ResponseData), 0);
                break;
            case LOBBY:
                GameSocket::handleLobbyEvent(clientSocket, data.uid);
                break;
            case CREATEROOM:
                GameSocket::handleRoomCreate(data.uid, "test");
                break;
            case DELETEROOM:
                GameSocket::handleRoomDelete(data.roomid);
                break;
            case JOINROOM:
                GameSocket::handleRoomJoin(data.uid, data.roomid);
                break;
            case LEAVEROOM:
                GameSocket::handleRoomLeave(data.uid, data.roomid);
                break;
            case CLOSE_SOCKET:
                break;
            case GAMESYNC:
                break;
        }
        gameLobby.printLobby();
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

    send(clientSocket, (char*) &data, sizeof(ResponseData), 0);
    
    for (auto it: gameLobby.rooms) {
        send(clientSocket, (char*) &(it.second), sizeof(Room), 0);
    }
}

void GameSocket::pushSocket(int p, string uid) {
    GameSocket::clientSockets[p] = uid;
    GameSocket::clientSockets_r[uid] = p;
}

void GameSocket::disconnetion(int socket) {
    cout << "[INFO] " << clientSockets[socket] << " Disconnetion" << endl;
    clientSockets_r.erase(clientSockets[socket]);
    clientSockets.erase(socket);
}

void GameSocket::clear() {
    pthread_join(GameSocket::mainThread, NULL);
    close(GameSocket::serverSocket);
}

void GameSocket::handleRoomCreate(char* uid, char* roomName) {

    Room nRoom = {gameLobby.size, "", ""};
    strcpy(nRoom.roomName, roomName);
    strcpy(nRoom.uid[nRoom.players++], uid);
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

void GameSocket::handleRoomJoin(char* uid, int id) {
    if (id == -1 || gameLobby.rooms[id].players > 1) return;
    strcpy(gameLobby.rooms[id].uid[1], uid);
    gameLobby.rooms[id].players++;

    ResponseData data = {JOINROOM, id};
    strcpy(data.uid, uid);
    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
    }
}

void GameSocket::handleRoomLeave(char* uid, int id) {
    if (id == -1) return;

    gameLobby.rooms[id].players--;
    if (!gameLobby.rooms[id].players) {
        GameSocket::handleRoomDelete(id);
        return;
    }

    if (strcmp(gameLobby.rooms[id].uid[0], uid) == 0) {
        swap(gameLobby.rooms[id].uid[0], gameLobby.rooms[id].uid[1]);
    }

    ResponseData data = {LEAVEROOM, id};
    strcpy(data.uid, uid);
    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
    }
}