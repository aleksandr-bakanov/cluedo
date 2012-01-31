#ifndef _ROOM_
#define _ROOM_

#include <pthread.h>
#include <cstdlib>
#include <ctime>
using namespace std;

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
    
    /**
     * Function process player's choose guest request. Returns true
     * if player successfuly choose guest.
     */
    bool chooseGuest(char guestId);
    
    /**
     * Function check whether all players in room choose guest.
     * If not, function set remained guest to players randomly.
     * Then the game starts.
     */
    void checkGuestDistribution();
//======================================================================
//  Fields
//======================================================================
    bool isOpen;
    char curPlayersCount;
    // Index of player who makes his move at this moment.
    char curPlayerIndex;
    static char ** map;
    
private:
//======================================================================
//  Methods
//======================================================================
    /**
     * Function starts the game process.
     */
    void startGame();
    
    /**
     * Function shuffles player's pointers in array.
     */
    void shufflePlayers();
    
    /**
     * Function sets inGame, x and y coordinates to each player.
     */
    void setPlayersStartParams();
    
    /**
     * Function sets start coordinates based on player's guest.
     */
    void setStartCoordinates(char &x, char &y, char guest);
    
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
    // Bit map of the available guests.
    // 00100101 - miss Scarlett, mrs. White and professor Plum
    // are available.
    char availableGuests;
    // Mutexes
    pthread_mutex_t chooseGuestMutex;
};

// Timer (waiting) functions
void * waitCheckGuestDistribution(void * ptr);

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
const char AP_KITCHEN = 1;          // k
const char AP_BALLROOM = 2;         // B
const char AP_CONSERVATORY = 3;     // C
const char AP_DINING_ROOM = 4;      // d
const char AP_CELLAR = 5;           // c
const char AP_BILLIARD_ROOM = 6;    // b
const char AP_LIBRARY = 7;          // L
const char AP_LOUNGE = 8;           // l
const char AP_HALL = 9;             // h
const char AP_STUDY = 2;            // s

#endif /* _ROOM_ */
