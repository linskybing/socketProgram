#include "src/socketc.cpp"

using namespace std;

int main() {
    ClientSocket::init();
    char uid[UID_LENGTH] = "user1";
    ClientSocket::sendRequest(uid, LOBBY);
    ClientSocket::stopThread();
    return 0;
}
