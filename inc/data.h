#ifndef DATA_H
#define DATA_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#define UID_LENGTH 100
#define ROOMNAME_LENGTH 30
#define PLAYERS 2
using namespace std;

enum RequestType { LOBBY, CREATEROOM, JOINROOM, LEAVEROOM, DELETEROOM, GAMESYNC, REJECT, CLOSE_SOCKET };

struct RequestData {
    RequestType type;
    char uid[UID_LENGTH];
    int roomid = -1;
};

struct ResponseData {
    RequestType type;
    int size;
    char uid[UID_LENGTH];
};

struct Room {
    int id;
    char roomName[ROOMNAME_LENGTH];
    char uid[PLAYERS][UID_LENGTH];
    int players = 0;
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
            if (size)
                size--;
        }
    }

    void printLobby() {
        if (rooms.empty()) {
            cout << "[INFO] Empty !" << endl;
            return;
        }
        for (auto it: rooms) {
            cout << "[INFO] Room ID: " << it.first << "\tRoom Name: " << it.second.roomName << '\n';
            cout << "\tplayer number: " << it.second.players;
            for (int i = 0; i < it.second.players; i++) {
                cout << "\tplayer" << (i+1) << ": " << it.second.uid[i]; 
            }
            cout << endl;
        }
    }
};


#endif