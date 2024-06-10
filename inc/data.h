#ifndef DATA_H
#define DATA_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#define UID_LENGTH 100
#define ROOMNAME_LENGTH 30

using namespace std;

enum RequestType { LOBBY, CREATEROOM, JOINROOM, DELETEROOM, GAMESYNC, CLOSE_SOCKET };

struct RequestData {
    RequestType type;
    char uid[UID_LENGTH];
    int roomid = 0;
};

struct ResponseData {
    RequestType type;
    int size;
};

struct Room {
    int id;
    char roomName[ROOMNAME_LENGTH];
};

struct Lobby {
    int size = 0;
    std::map<int, Room> rooms;

    void push(int id, Room room) {
        size++;
        rooms[id] = room;
    }

    void erase(int id) {
        if (rooms.count(id)) {
            rooms.erase(id);
        }
    }

    void printLobby() {
        for (auto it: rooms) {
            cout << "[INFO] Room ID: " << it.first << "\tRoom Name: " << it.second.roomName << '\n';
        }
    }
};


#endif