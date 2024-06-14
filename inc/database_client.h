#ifndef DATABASE_H
#define DATABASE_H
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;
#define LENGTH 100
namespace Auth {
    char userName[LENGTH];
    char password[LENGTH];

    void writeCookie(string name, string pwd) {
        ofstream ofs;
        ofs.open("cookie.txt");
        ofs << name << "\t" << pwd << "\n";
        ofs.close();
    }

    void readCookie() {
        ifstream ifs;
        ifs.open("cookie.txt");
        if (ifs.is_open()) {
            ifs >> Auth::userName >> Auth::password;
        }
        ifs.close();
    }

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
}
#endif