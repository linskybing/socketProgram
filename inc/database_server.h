#ifndef DATABASE_H
#define DATABASE_H
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

namespace DB {
    map<string, string> users;
    map<string, int> score;
    map<string, int[ITEMS]> items;
    map<string, int> money;
    string hash(string pwd) {
        unsigned long h = 5381;
        stringstream ss;
        string result;
        for (auto c : pwd) {
            h = ((h << 5) + h) + (int) c;
        }
        ss << h;
        ss >> result;
        return result;
    }

    void readUserScore(string userName) {
        ifstream ifs;
        ifs.open("score/" + userName + ".txt");
        int s;
        ifs >> s;
        DB::score[userName] = s;
        ifs.close();
    }

    void readUserItem(string userName) {
        ifstream ifs;
        ifs.open("item/" + userName + ".txt");
        int t;
        int i = 0;
        while (ifs >> t) {
            items[userName][i++] = t;
        }
        ifs.close();
    }

    void readUserMoney(string userName) {
        ifstream ifs;
        ifs.open("money/" + userName + ".txt");
        int t;
        ifs >> t;
        DB::money[userName] = t;
        ifs.close();
    }

    void readUsersData() {
        ifstream ifs;
        ifs.open("auth.txt");
        string userName, password;

        while (ifs >> userName >> password) {
            DB::users[userName] = password;
            readUserItem(userName);
            readUserScore(userName);
            readUserMoney(userName);
        }

        ifs.close();
    }

    bool login(char name[UID_LENGTH], string password) {
        return (DB::users.count(name) && DB::users[name] == password);
    }

    void registerUser(string userName, string password) {
        DB::users[userName] = password;
        ofstream ofs;
        ofs.open("auth.txt", ios_base::app);
        ofs << userName << "\t" << password << "\n";
        ofs.close();
    }

    void writeUserScore(string userName) {
        ofstream ofs;
        ofs.open("score/" + userName + ".txt");
        ofs << ((DB::score.count(userName))? DB::score[userName]: 0);
        ofs.close();
    }

    void writeUserItem(string userName) {
        ofstream ofs;
        ofs.open("item/" + userName + ".txt");
        if (DB::items.count(userName)) {
            ofs << "0\t0\t0\t";
        }
        else {
            for (int i = 0; i < ITEMS; i++) {
                ofs << DB::items[userName][i] << "\t";
            }
        }
        ofs.close();
    }

    void writeUserMoney(string userName) {
        ofstream ofs;
        ofs.open("money/" + userName + ".txt");
        if (DB::money.count(userName)) {
            ofs << DB::money[userName];
        }
        else {
            ofs << 0 << endl;
        }
        ofs.close();
    }
    
    void writeUserData() {
        for (auto it: DB::users) {
            writeUserItem(it.first);
            writeUserMoney(it.first);
            writeUserScore(it.first);
        }
    }
};
#endif