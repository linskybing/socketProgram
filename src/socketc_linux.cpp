#include "../inc/socketc_linux.h"
#include <iostream>

using namespace std;

void ClientSocket::init() {

    struct sockaddr_in server_name;

    ClientSocket::clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   // server address
    server_name.sin_family = AF_INET;
    inet_aton(host, &server_name.sin_addr);
    server_name.sin_port = htons(PORT);

    if (connect(ClientSocket::clientSocket, (sockaddr *)&server_name, sizeof(server_name)) == -1) {
        return;
    }

    cout << "Connected to server.\n";
    pthread_create(&ClientSocket::mainThread, nullptr, &ClientSocket::handleResponse, &ClientSocket::clientSocket);
}

void* ClientSocket::handleResponse(void* arg) {
    int clientSocket = *((int *)arg);
    int byte_recv = 0;
    ResponseData data;
    while ((byte_recv = recv(clientSocket, (char*)& data, sizeof(ResponseData), 0)) > 0) {
        cout << "[INFO] receive response Type: " << data.type << " size: " << data.size << endl;
        // close int
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
                ClientSocket::handleRoomJoin(data.uid, data.size);
                break;
            case DELETEROOM:
                ClientSocket::handleRoomDelete(data.size);
                break;
            case LEAVEROOM:
                ClientSocket::handleRoomLeave(data.uid, data.size);
                break;
            case GAMESYNC:
                break;
        }
        gameLobby.printLobby();
    }
    return NULL;
}

void ClientSocket::handleLobbyEvent(int clientSocket, int size) {
    if (size) {
        for (int i = 0; i < size; i++) {
            Room data;
            recv(clientSocket, (char*)& data, sizeof(Room), 0);
            if (strcmp(data.uid[0], auth_uid) == 0) {
                GameManager::enterExistRoom(data.id);
            }        
            gameLobby.push(data.id, data);
        }
    }
}

void ClientSocket::handleRoomCreate(int clientSocket, int size) {
    if (size) {
        Room roomdata;
        recv(clientSocket, (char*)& roomdata, sizeof(Room) * size, 0);
        if (strcmp(roomdata.uid[0], auth_uid) == 0)
            GameManager::enterExistRoom(roomdata.id);
        gameLobby.push(roomdata.id, roomdata);  
    }
}

void ClientSocket::handleRoomDelete(int id) {
    gameLobby.erase(id);
    cout << "[INFO] Room " << id << " DELETE" << endl;
}

void ClientSocket::handleRoomJoin(char* uid, int id) {
    if (strcmp(uid, auth_uid) == 0) {
        GameManager::enterExistRoom(id);
    }
    
    strcpy(gameLobby.rooms[id].uid[1], uid);
    gameLobby.rooms[id].players++;
}

void ClientSocket::handleRoomLeave(char* uid, int id) {
    if (strcmp(uid, auth_uid) == 0) {
        cout << "[INFO] Leave room " << id << endl;
        GameManager::currentRoom = -1;
    }

    gameLobby.rooms[id].players--;
    
    if (!gameLobby.rooms[id].players) {
        ClientSocket::handleRoomDelete(id);
        return;
    }

    if (strcmp(gameLobby.rooms[id].uid[0], uid) == 0) {
        swap(gameLobby.rooms[id].uid[0], gameLobby.rooms[id].uid[1]);
    }

}

void ClientSocket::sendRequest(char uid[UID_LENGTH], RequestType type, int roomId = -1) {
    cout << "[INFO] send Request Type: " << type << endl;
    RequestData data = {type, ""};
    strcpy(data.uid, uid);

    if (roomId != -1)
        data.roomid = roomId;
    
    send(ClientSocket::clientSocket, (char*) &data, sizeof(data), 0);
}

void ClientSocket::clear() {
    close(ClientSocket::clientSocket);
}

void ClientSocket::stopThread() {
    if (ClientSocket::mainThread) {
        pthread_join(ClientSocket::mainThread, NULL);
        ClientSocket::clear();
    }
}


void GameManager::enterExistRoom(int id) {
    cout << "[INFO] Enter room " << id << endl;
    GameManager::currentRoom = id;
}


