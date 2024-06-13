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
        cout << "[INFO] receive response Type: " << data.type << " size: " << data.size << endl;
        // close socket
        if (data.type == CLOSE_SOCKET)
            break;
            
        // handle request
        switch (data.type) {
            case LOGIN:
                if (data.flag) {
                    Auth::writeCookie(Auth::userName, Auth::password);
                }
                else cout << "userName or Password incorrect !!" << endl;
                break;
            case REGISTER:
                if (data.flag) {
                    Auth::writeCookie(Auth::userName, Auth::password);
                }
                else cout << "FAIL" << endl;
                break;
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
}

void ClientSocket::handleLobbyEvent(SOCKET clientSocket, int size) {
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

void ClientSocket::handleRoomCreate(SOCKET clientSocket, int size) {
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
    RequestData data;
    data.type = type;
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


void GameManager::enterExistRoom(int id) {
    cout << "[INFO] Enter room " << id << endl;
    GameManager::currentRoom = id;
}

void loginToLobby(char* name, char* pwd) {
    RequestData data;
    data.type = LOGIN;
    strcpy(data.uid, name);

    send(ClientSocket::clientSocket, (char*) &data, sizeof(RequestData), 0);
}


void registerAccount(char* name, char* pwd) {
    RequestData data;
    data.type = REGISTER;
    strcpy(data.uid, name);

    send(ClientSocket::clientSocket, (char*) &data, sizeof(RequestData), 0);
}