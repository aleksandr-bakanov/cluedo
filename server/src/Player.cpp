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
    leaveRoomHandler();
    close(socket);
    cout << "Player::~Player(" << socket << ")" << endl;
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
            case C_LEAVE_ROOM: leaveRoomHandler(); break;
            case C_CHOOSE_GUEST: chooseGuestHandler(); break;
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

void 
Player::resetGameInfo()
{
    guest = x = y = app = 0;
    for (int i = 0; i < 50; i++)
        cards[i] = 0;
}

void 
Player::leaveRoomHandler()
{
    if (room)
    {
        Room * r = (Room *)room;
        r->removePlayer((void *)this);
    }
}

void
Player::sendAvailableGuests(char guestMap)
{
    short cmdSize;
    int pos = 4;
    short cmdId = S_AVAILABLE_GUESTS;
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    for (int i = 0; i < 6; i++)
        if (guestMap & char(pow(2, i)))
            sendBuf[pos++] = char(i + 1);
    cmdSize = SHORT_SIZE + pos - 4;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    sendData(pos);
}

void 
Player::chooseGuestHandler()
{
    if (room && !guest)
    {
        char id = BCgetChar(recvBuf, curRecvPos);
        if (id >= 1 && id <= 6)
        {
            Room * r = (Room *)room;
            bool result = r->chooseGuest(id);
            if (result)
                guest = id;
            sendGuestChooseResult(id, result);
        }
    }
}

void 
Player::sendGuestChooseResult(char guestId, bool result)
{
    short cmdSize = 3;
    short cmdId = S_GUEST_CHOOSE_RESULT;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    unsigned char res = 0;
    res |= char(pow(2, guestId - 1));
    if (result) res |= char(64);
    sendBuf[4] = res;
    sendData(5);
}
