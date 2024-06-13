#include "src/socketc.cpp"

using namespace std;

int main() {
    ClientSocket::init();

    int request;
    int id;
    while (cin >> request) {
        if (request == -1) break;
        if (request == LEAVEROOM) {
            ClientSocket::sendRequest(auth_uid, (RequestType) request, GameManager::currentRoom);
        }
        else if (request == LOBBY) {
            ClientSocket::sendRequest(auth_uid, LOBBY);
        }
        else if (request == LOGIN) {
            char name[UID_LENGTH];
            char pwd[UID_LENGTH];
            cin >> name >> pwd;
            loginToLobby(name, pwd);
        }
        else if (request == REGISTER) {
            char name[UID_LENGTH];
            char pwd[UID_LENGTH];
            cin >> name >> pwd;
            registerAccount(name, pwd);
        }
        else if (request == JOINROOM) {
            cout << "enter room id: ";
            cin >> id;
            ClientSocket::sendRequest(auth_uid, (RequestType) request, id);
        }
        else {
            ClientSocket::sendRequest(auth_uid, (RequestType) request);
        }
    }
    ClientSocket::stopThread();

    return 0;
}
