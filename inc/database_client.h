#ifndef DATABASE_H
#define DATABASE_H
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

namespace Auth {
    string userName;
    string password;
    void writeCookie(string name, string pwd) {
        ofstream ofs;
        ofs.open("cookie.txt");
        ofs << name << "\t" << pwd << "\n";
        ofs.close();
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