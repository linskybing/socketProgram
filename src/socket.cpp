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
    {CLOSE_SOCKET, "CLOSE_SOCKET"},
    {LOADGAME, "LOADGAME"},
    {GAMESTART, "GAMESTART"},
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
    int on = 1;
    if (GameSocket::serverSocket == -1) {
        perror("Socket creation error");
        exit(1);
    }


    if (setsockopt(GameSocket::serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
        perror("Setsockopt error");
        exit(1);
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    bind(GameSocket::serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));


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
                if (!DB::users.count(data.uid)) {
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
                GameSocket::handleRoomCreate(data.uid, data.uid);
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
            case GAMESTART:
                GameSocket::quickSend(data);
                break;
            case SELECT:
                GameSocket::handleMapSelect(data);
                break;
            case LOADGAME:
                GameSocket::handleGameStart(data);
                break;
            case GAMESYNC: 
                GameSocket::handleGameSync(data);
                break;
        }
        gameLobby.printLobby();
    }

    GameSocket::disconnetion(clientSocket);

    pthread_exit(nullptr);
    return nullptr;
}

void GameSocket::checkAndAssgin(char uid[UID_LENGTH]) {
    if (!DB::score.count(uid)) {
        DB::score[uid] = 0;
        DB::writeUserScore(uid);
    }

    if (!DB::items.count(uid)) {
        DB::items[uid] = {0, 0, 0};
        DB::writeUserItem(uid);
    }

    if (!DB::money.count(uid)) {
        DB::money[uid] = 0;
        DB::writeUserScore(uid);
    }
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

    checkAndAssgin(uid);
    UserData udata;
    udata.money = DB::money[uid];
    udata.score = DB::score[uid];
    udata.items = DB::items[uid];
    send(clientSocket, (char*) &udata, sizeof(UserData), 0);
}

void GameSocket::pushSocket(int p, string uid) {
    GameSocket::clientSockets[p] = uid;
    GameSocket::clientSockets_r[uid] = p;
}

void GameSocket::disconnetion(int socket) {
    cout << "[INFO] " << clientSockets[socket] << " Disconnetion" << endl;
    string userName(clientSockets[socket]);
    clientSockets_r.erase(userName);
    clientSockets.erase(socket);
    if (gameLobby.userToRoom.count(userName))
        GameSocket::handleRoomLeave(userName.c_str(), gameLobby.userToRoom[userName]);
}

void GameSocket::clear() {
    pthread_join(GameSocket::mainThread, NULL);
    close(GameSocket::serverSocket);
}

void GameSocket::handleRoomCreate(char* uid, char* roomName) {

    Room nRoom;
    if (gameLobby.size)
        nRoom.id = (gameLobby.rooms.rbegin()->first) + 1;
    else
        nRoom.id = 0;
    strcpy(nRoom.roomName, roomName);
    strcpy(nRoom.uid[nRoom.players++], uid);
    gameLobby.push(nRoom.id, nRoom);
    gameLobby.userToRoom[string(uid)] = nRoom.id;

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
    gameLobby.userToRoom[string(uid)] = id;
    ResponseData data;
    data.type = JOINROOM;
    data.size = id;
    strcpy(data.uid, uid);
    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &data, sizeof(ResponseData), 0);
    }
}

void GameSocket::handleRoomLeave(const char* uid, int id) {
    if (id == -1) return;
    gameLobby.userToRoom.erase(string(uid));
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

void printData(RequestData data) {
    cout << "TYPE: " << typeToStr[data.type] << endl;
    cout << "ROOMID: " << data.roomid << endl;
    cout << "GAMETYPE " << data.gameData.type << endl;
}

void GameSocket::handleGameSync(RequestData data) {
    if (data.roomid == -1 || !gameLobby.rooms.count(data.roomid))
        return;
    int id = data.roomid;
    auto gData = gameLobby.rooms[id];
    ResponseData rdata;
    strcpy(rdata.uid, data.uid);
    rdata.roomid = data.roomid;
    rdata.type = GAMESYNC;
    rdata.gameData = data.gameData;
    for (int i = 0; i < gData.players; i++) {
        int target = clientSockets_r[gData.uid[i]];
        send(target, (char*) &rdata, sizeof(ResponseData), 0);
    }
}

void GameSocket::handleGameStart(RequestData data) {
    if (data.roomid == -1 || !gameLobby.rooms.count(data.roomid))
        return;
    int id = data.roomid;
    auto gData = gameLobby.rooms[id];
    ResponseData rdata;
    strcpy(rdata.uid, data.uid);
    rdata.roomid = data.roomid;
    rdata.type = LOADGAME;
    for (int i = 0; i < gData.players; i++) {
        int target = clientSockets_r[gData.uid[i]];
        send(target, (char*) &rdata, sizeof(ResponseData), 0);
    }
}

void GameSocket::quickSend(RequestData data) {
    ResponseData rdata;
    rdata.type = GAMESTART;
    strcpy(rdata.uid, data.uid);
    rdata.roomid = data.roomid;
    auto gData = gameLobby.rooms[data.roomid];
    for (int i = 0; i < gData.players; i++) {
        if (!strcmp(gData.uid[i], data.uid)) {
            int target = clientSockets_r[gData.uid[i]];
            send(target, (char*) &rdata, sizeof(ResponseData), 0);
            return;
        }
    }
}

void GameSocket::handleMapSelect(RequestData data) {
    if (data.roomid == -1 || !gameLobby.rooms.count(data.roomid))
        return;
    ResponseData rdata;
    strcpy(rdata.uid, data.uid);
    rdata.roomid = data.roomid;
    rdata.type = SELECT;
    rdata.mapId = data.mapId;
    std::cout << data.mapId << std::endl;
    for (auto it: GameSocket::clientSockets) {
        send((it.first), (char*) &rdata, sizeof(ResponseData), 0);
    }
}