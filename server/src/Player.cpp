#include "include/Player.hpp"
#include "include/BytesConverter.hpp"

Player::Player(int socketDescriptor)
{
    socket = socketDescriptor;
    curRecvPos = 0;
    memset(recvBuf, 0, RECV_BUF_SIZE);
    memset(sendBuf, 0, RECV_BUF_SIZE);
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
        int comId = BCgetShort(recvBuf, curRecvPos);
        switch (comId)
        {
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
    memcpy(sendBuf, &cmdId, SHORT_SIZE);
    sendData(SHORT_SIZE * 2);
}
