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

// Guest constants
const char GT_SCARLETT = 1;
const char GT_MUSTARD = 2;
const char GT_WHITE = 3;
const char GT_GREEN = 4;
const char GT_PEACOCK = 5;
const char GT_PLUM = 6;
// Weapon constants
const char WP_CANDLESTICK = 1;
const char WP_KNIFE = 2;
const char WP_AXE = 3;
const char WP_PISTOL = 4;
const char WP_ROPE = 5;
const char WP_TROPHY = 6;
const char WP_BAT = 7;
const char WP_DUMBBELL = 8;
const char WP_POISON = 9;
// Appartments constants
const char AP_ = 1;

#endif /* _ROOM_ */
