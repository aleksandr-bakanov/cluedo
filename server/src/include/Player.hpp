#ifndef _PLAYER_
#define _PLAYER_

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

//======================================================================
//  A-star
//======================================================================
struct Cell
{
    char x;
    char y;
    char px; // parent X
    char py; // parent Y
    char f;
    char g;
    char h;
};

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
    
    /**
     * Function resets game params such as guest, x, y, app and cards.
     */
    void resetGameInfo();
    
    /**
     * Function send to client available guest ids.
     * @param   guestMap    bit map: 00100101 - miss Scarlett,
     *                      mrs. White and professor Plum are available
     */
    void sendAvailableGuests(char guestMap);
    
    /**
     * Function send to player result of processing his C_CHOOSE_GUEST
     * request.
     */
    void sendGuestChooseResult(char guestId, bool result);
    
    /**
     * Function adds specified card into the cards array. Returns index
     * of added card.
     */
    char addCard(char card);
    
    /**
     * Function sends to client start game information.
     */
    void sendStartInfo();
    
    /**
     * Function sends next move info, such as guest id and the dice.
     */
    void sendNextMove(char guestId, char firstDie, char secondDie);
    
    /**
     * Function sends S_GUEST_MOVE command.
     */
    void sendGuestMakeMove(char guestId, vector<char> &v);
    
    // A* function
    void aStar(char x, char y, char tox, char toy, vector<char> &v);
//======================================================================
//  Fields
//======================================================================
    // Pointer to RoomManager object
    void * roomManager;
    // Pointer to current Room object
    void * room;
    // Guest id
    char guest;
    // True if player in game now.
    bool inGame;
    // Guest coordinates
    char x;
    char y;
    // Current appartments
    char app;
    char lastAskedApp;
    // Cards
    char cards[8];
    // Intrigues
    char intrigues[10];
    // True if now that player's moving.
    bool myTurn;
    // Count of available steps.
    char steps;
    // Set to true when guest enter room. Player can't move if mustAsk
    // is true.
    bool mustAsk;
    
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
    // C_LEAVE_ROOM command handler.
    void leaveRoomHandler();
    // C_CHOOSE_GUEST command handler.
    void chooseGuestHandler();
    // C_GUEST_MOVE command handler.
    void makeMoveHandler();
    // C_ASK command handler
    void askHandler();
    
    // A* util functions
    int exist(char x, char y, vector<Cell> &v);
    int minFCell(vector<Cell> &v);
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
const int MAX_CARDS = 10;
const int MAX_INTRIGUES = 10;
const int RECV_BUF_SIZE = 256;
// Server side command ids.
const short S_READY = 1;
const short S_NO_ROOM = 3;
const short S_AVAILABLE_GUESTS = 5;
const short S_GUEST_CHOOSE_RESULT = 7;
const short S_START_GAME_INFO = 9;
const short S_GUEST_MOVE = 11;
const short S_NEXT_MOVE = 13;
// Client side command ids.
const short C_ENTER_ROOM = 2;
const short C_LEAVE_ROOM = 4;
const short C_CHOOSE_GUEST = 6;
const short C_GUEST_MOVE = 8;
const short C_ASK = 10;

#endif /* _PLAYER_ */
