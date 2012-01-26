#ifndef _PLAYER_
#define _PLAYER_

#include <unistd.h>
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
     */
    Player(int);
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
//======================================================================
//  Consts
//======================================================================
    
private:
//======================================================================
//  Methods
//======================================================================

//======================================================================
//  Fields
//======================================================================
    // Player socket descriptor
    int socket;
//======================================================================
//  Consts
//======================================================================
};

#endif /* _PLAYER_ */
