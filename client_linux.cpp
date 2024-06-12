#include "src/socketc_linux.cpp"

using namespace std;

int main() {
    ClientSocket::init();

    cin >> auth_uid;
    ClientSocket::sendRequest(auth_uid, LOBBY);
    int request;
    int id;
    cout << "enter request code: ";
    while (cin >> request) {
        if (request == -1) break;
        if (request == LEAVEROOM) {
            ClientSocket::sendRequest(auth_uid, (RequestType) request, GameManager::currentRoom);
        }
        else if (request == JOINROOM) {
            cout << "enter room id: ";
            cin >> id;
            ClientSocket::sendRequest(auth_uid, (RequestType) request, id);
        }
        else {
            ClientSocket::sendRequest(auth_uid, (RequestType) request);
        }
        cout << "enter request code: ";
    }
    ClientSocket::stopThread();
    return 0;
}
