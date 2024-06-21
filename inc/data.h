#ifndef DATA_H
#define DATA_H

#include <vector>
#include <map>
#include <string>
#include <queue>
#include <iostream>
#define UID_LENGTH 100
#define ROOMNAME_LENGTH 30
#define PLAYERS 2
using namespace std;

enum RequestType { LOGIN, REGISTER, LOBBY, CREATEROOM, JOINROOM, LEAVEROOM, DELETEROOM, LOADGAME, GAMESTART, GAMESYNC, SELECT, REJECT, CLOSE_SOCKET };
enum AuthState { SUCCESS, ENTERGAME, FALE, EXIST, WAIT };
enum CallBackType { START, ROOMLIST, ROOM, GAME };
enum GameSyncType { FREEZE, METEOR, BARREL, SHOVEL, LEVELUP, TOWER };
extern map<RequestType, string> typeToStr;
extern map<AuthState, string> authToStr;

struct GameSync {
    GameSyncType type;
    float x;
    float y;
};
struct RequestData {
    RequestType type;
    char uid[UID_LENGTH];
    char pwd[UID_LENGTH];
    int roomid = -1;
    int mapId = 1;
    GameSync gameData;
};

struct ResponseData {
    int size = 0;
    int roomid = -1;
    int mapId = 1;
    char uid[UID_LENGTH];
    RequestType type;
    AuthState auth;    
    GameSync gameData;
};

struct Room {
    int id;
    char roomName[ROOMNAME_LENGTH];
    char uid[PLAYERS][UID_LENGTH];
    int mapId = 1;
    int players = 0;
};

struct UserData {
    int money;
    int score;
};

struct Lobby {
    int size = 0;
    std::map<int, Room> rooms;
    std::map<std::string, int> userToRoom;
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