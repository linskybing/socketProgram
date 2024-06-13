
// C++ program to show the example of server application in
// socket programming
#include "src/socket.cpp"

int main()
{
    DB::readUsersData();
    GameSocket::init();
    GameSocket::clear();
    return 0;
}
