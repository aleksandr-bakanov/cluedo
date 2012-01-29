#include "include/Player.hpp"
#include "include/BytesConverter.hpp"
#include "include/RoomManager.hpp"
#include "include/Room.hpp"

Player::Player(int socketDescriptor)
{
    socket = socketDescriptor;
    curRecvPos = 0;
    memset(recvBuf, 0, RECV_BUF_SIZE);
    memset(sendBuf, 0, RECV_BUF_SIZE);
    room = NULL;
    cout << "Player::Player(" << socket << ")" << endl;
}

Player::~Player()
{
    if (room)
    {
        Room * r = (Room *)room;
        r->removePlayer((void *)this);
        room = NULL;
    }
    close(socket);
    cout << "Player::~Player()" << endl;
}

void
Player::run()
{
    sendReadyCommand();
    while (true)
    {
        int recvCount = 0;
        recvCount = read(socket, recvBuf, RECV_BUF_SIZE - curRecvPos);
        if (recvCount <= 0)
            break;
        else
        {
            curRecvPos += recvCount;
            parse();
        }
    }
}

void 
Player::parse()
{
    short comSize = BCgetShort(recvBuf);
    if (curRecvPos - SHORT_SIZE >= comSize)
    {
        curRecvPos = SHORT_SIZE;
        short comId = BCgetShort(recvBuf, curRecvPos);
        switch (comId)
        {
            case C_ENTER_ROOM: enterRoomHandler(); break;
            default: break;
        }
        curRecvPos = 0;
    }
}

bool 
Player::sendData(int nbytes)
{
    if (nbytes <= 0) 
        return false;
    int sendCount = write(socket, sendBuf, nbytes);
    if (sendCount <= 0)
        return false;
    else
        return true;
}

void 
Player::sendReadyCommand()
{
    short cmdSize = SHORT_SIZE;
    short cmdId = S_READY;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    sendData(SHORT_SIZE * 2);
}

void 
Player::enterRoomHandler()
{
    char type = BCgetChar(recvBuf, curRecvPos);
    if (type >= 3 && type <= 6)
    {
        RoomManager * rm = (RoomManager *)roomManager;
        bool success = rm->addPlayerIntoRoom((void *)this, type);
        if (!success)
            sendNoRoom();
    }
}

void
Player::sendNoRoom()
{
    short cmdSize = SHORT_SIZE;
    short cmdId = S_NO_ROOM;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    sendData(SHORT_SIZE * 2);
}
