#ifndef _ROOM_
#define _ROOM_

/**
 * Room class is a main controller of a game process.
 */
class Room
{
public:
//======================================================================
//  Methods
//======================================================================
    /**
     * Constructor. It creates arrays of pointers to Player objects.
     * Init these arrays by NULL pointers.
     * @param   type    count of players
     */
    Room(char type);
    ~Room();
    
    /**
     * Add new player into players array and start the game if player's
     * array is full.
     */
    void addPlayer(void * player);
    
    /**
     * Remove player from players array.
     */
    void removePlayer(void * player);
    
//======================================================================
//  Fields
//======================================================================
    bool isOpen;
    char curPlayersCount;
    
private:
//======================================================================
//  Methods
//======================================================================
    /**
     * Function starts the game process.
     */
    void startGame();
    
    /**
     * Function returns index of the first NULL pointer into players
     * array.
     */
    int getEmptyIndex();
//======================================================================
//  Fields
//======================================================================
    // Array of players pointers.
    void ** players;
    char totalPlayers;
};

#endif /* _ROOM_ */
