#include "../inc/socket.h"
#include <iostream>
#include <cstring>
using namespace std;

map<RequestType, string> typeToStr = {
    {LOGIN, "LOGIN"},
    {REGISTER, "REGISTER"},
    {LOBBY, "LOBBY"},
    {CREATEROOM, "CREATEROOM"},
    {JOINROOM, "JOINROOM"},
    {LEAVEROOM, "LEAVEROOM"},
    {DELETEROOM, "DELETEROOM"},
    {GAMESYNC, "GAMESYNC"},
    {REJECT, "REJECT"},
    {CLOSE_SOCKET, "CLOSE_SOCKET"}
};

map<AuthState, string> authToStr = {
    {SUCCESS, "SUCCESS"},
    {FALE, "FALE"}, 
    {EXIST, "EXIST"},
    {WAIT, "WAIT"}
};

void GameSocket::init() {
    GameSocket:serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int status;
    if (GameSocket:serverSocket == -1) {
        perror("Socket creation error");
        exit(1);
    }


    if (setsockopt(GameSocket:serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
        perror("Setsockopt error");
        exit(1);
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    status = bind(GameSocket::serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (status == -1) {
        perror("Binding error");
        exit(1);
    }

    status = listen(GameSocket::serverSocket, 5);

    if (status == -1) {
        perror("Listening error");
        exit(1);
    }
    
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
        cout << "[INFO] Reuqest " << data.uid << " Type: " << typeToStr[data.type] << endl;
        ResponseData datar;
        // handle request
        cout << data.uid << " " << data.pwd << " " << DB::hash(data.pwd) << " " << DB::users[data.uid] << " " << DB::login(data.uid, DB::hash(data.pwd)) << endl;
        switch (data.type) {
            case LOGIN:
                if (DB::login(data.uid, DB::hash(data.pwd))) {
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
    ResponseData data;
    data.type = LOBBY;
    data.size = gameLobby.size;
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

    ResponseData data;
    data.type = CREATEROOM;
    data.size = 1;
    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
        send((it.first), (char*) &nRoom, sizeof(Room), 0);
    }
}

void GameSocket::handleRoomDelete(int id) {
    gameLobby.erase(id);
    ResponseData data;
    data.type = DELETEROOM;
    data.size = id;
    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
    }

    cout << "[INFO] ROOM " << id << " DELETED\n";
}

void GameSocket::handleRoomJoin(char* uid, int id) {
    if (id == -1 || gameLobby.rooms[id].players > 1) return;
    strcpy(gameLobby.rooms[id].uid[1], uid);
    gameLobby.rooms[id].players++;

    ResponseData data;
    data.type = JOINROOM;
    data.size = id;
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

    ResponseData data;
    data.type = LEAVEROOM;
    data.size = id;
    strcpy(data.uid, uid);
    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
    }
}