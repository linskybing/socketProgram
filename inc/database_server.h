#ifndef DATABASE_H
#define DATABASE_H
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

namespace DB {
    map<string, string> users;
    
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
    
    void readUsersData() {
        ifstream ifs;
        ifs.open("auth.txt");
        string userName, password;

        while (ifs >> userName >> password) {
            DB::users[userName] = password;
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
}
#endif