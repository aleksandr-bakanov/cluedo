#ifndef _PLAYER_
#define _PLAYER_

#include <unistd.h>
#include <string.h>
#include <iostream>
using namespace std;

/**
 * Player is a model which represent player condition.
 * Also it is an input point of client commands.
 */
class Player
{
public:
//======================================================================
//  Methods
//======================================================================
    /**
     * Constructor. Store socket descriptor in the private field.
     * @param   socketFd    Socket file descriptor.
     */
    Player(int socketFd);
    ~Player();
    /**
     * Function run listening process. When client close connection
     * this function return.
     */
    void run();
//======================================================================
//  Fields
//======================================================================
    // Pointer to RoomManager object
    void * roomManager;
    // Pointer to current Room object
    void * room;
    
private:
//======================================================================
//  Methods
//======================================================================
    /**
     * Function send bytes from sendBuf to socket.
     * @param   nbytes  Count of bytes to send.
     * @return          Sending result.
     */
    bool sendData(int nbytes);
    
    /**
     * Function calling when there are at least one full command from
     * player. Function read command id (short int) from recvBuf[2]
     * and call appropriate function which will process other data
     * in received command.
     */
    void parse();
    
    /**
     * Function send to client command S_READY, which mean that
     * server is ready to receive commands from client.
     */
    void sendReadyCommand();
    
    // C_ENTER_ROOM command handler.
    void enterRoomHandler();
    // Send S_NO_ROOM to client.
    void sendNoRoom();
//======================================================================
//  Fields
//======================================================================
    // Player socket descriptor
    int socket;
    // Receive buffer
    char recvBuf[256];
    // Send buffer
    char sendBuf[256];
    // Current position in recvBuf.
    int curRecvPos;
};

//======================================================================
//  Consts
//======================================================================
const int RECV_BUF_SIZE = 256;
// Server side command ids.
const short S_READY = 1;
const short S_NO_ROOM = 3;
// Client side command ids.
const short C_ENTER_ROOM = 2;

#endif /* _PLAYER_ */
