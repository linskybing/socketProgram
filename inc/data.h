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
#define ITEMS 3
using namespace std;

enum RequestType { LOGIN, REGISTER, LOBBY, CREATEROOM, JOINROOM, LEAVEROOM, DELETEROOM, LOADGAME, GAMESTART, GAMESYNC, SELECT, REJECT, CLOSE_SOCKET, WRITEBACK, SCORE};
enum AuthState { SUCCESS, ENTERGAME, STARTSYNC, FALE, EXIST, WAIT };
enum CallBackType { START, ROOM, GAME, SHOP };
enum GameSyncType { FREEZE, METEOR, BARREL, SHOVEL, LEVELUP, TOWER, SPEED, WIN, LOSE };
extern map<RequestType, string> typeToStr;
extern map<AuthState, string> authToStr;

struct ScoreData {
    char uid[UID_LENGTH];
    int score;
};

struct UserData {
    int money;
    int score;
    int items[ITEMS];
};

struct GameSync {
    GameSyncType type;
    float x;
    float y;
    int speed;
    int id;
};
struct RequestData {
    RequestType type;
    char uid[UID_LENGTH];
    char pwd[UID_LENGTH];
    int roomid = -1;
    int mapId = 1;
    GameSync gameData;
    UserData udata;
};

struct ResponseData {
    int size = 0;
    int roomid = -1;
    int mapId = 1;
    char uid[UID_LENGTH];
    RequestType type;
    AuthState auth;    
    GameSync gameData;
    UserData udata;
};

struct Room {
    int id;
    char roomName[ROOMNAME_LENGTH];
    char uid[PLAYERS][UID_LENGTH];
    int players = 0;
    int mapId = 1;
};

struct Lobby {
    int size = 0;
    int ids = 0;
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