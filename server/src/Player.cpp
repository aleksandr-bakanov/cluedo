#include "include/Player.hpp"

Player::Player(int socketDescriptor)
{
    socket = socketDescriptor;
    cout << "Player::Player(" << socket << ")" << endl;
}

Player::~Player()
{
    close(socket);
    cout << "Player::~Player()" << endl;
}

void
Player::run()
{
    cout << "Player.run()" << endl;
}
