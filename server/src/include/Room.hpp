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
     * Function starts next move (or turn). Sends to all players which
     * guest is moving now and how much steps he has.
     */
    void nextMove();
    
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
    
    /**
     * Function move guest on the specified coordinates if it's
     * possible. On success function says to all players about this
     * moving.
     */
    void guestMakeMove(void * player, char x, char y);
    
    /**
     * Player suspect murderer and weapon.
     */
    void playerAsk(void * player, char guest, char weapon);
    
    /**
     * Function transport guest to the room directly without calling
     * A* algorithm.
     */
    void moveGuestToRoom(char gt, char ap);
    
    void getAppCoordinates(char ap, char &x, char &y);
    
    /**
     * Some player answer on suspect.
     */
    void playerAnswer(void * player, char card);
    
    /**
     * Function makes auto answer based on cards of player who must
     * answer now.
     */
    void autoAnswer();
    
    /**
     * Player make his final suspect. Function compare ap, gt and wp
     * with secret cards and if they are equals player win the game.
     * Else he lose and lost ability to move and ask.
     */
    void playerGuessSecret(void * player, char ap, char gt, char wp);

//======================================================================
//  Fields
//======================================================================
    bool isOpen;
    char curPlayersCount;
    // Index of the guest who makes his move at this moment.
    char curGuestIndex;
    // This variable increments each nextMove call. When in nextMove
    // creates timer thread it save current value of curMove. After
    // sleep it compare saved value with current curMove value. If
    // they are equals nextMove calls from timer thread.
    int curMove;
    // True if somebody start suspect someone else. Set to false when
    // questioning is over.
    bool isQuestioning;
    // Map
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
     * Function ends game.
     */
    void endGame();
    
    /**
     * Function shuffles player's pointers in array.
     */
    void shuffleGuests();
    
    /**
     * Function sets inGame, x and y coordinates to each player.
     */
    void setPlayersStartParams();
    
    /**
     * Function sets start coordinates based on player's guest.
     */
    void setStartCoordinates(char &x, char &y, char guest);
    
    /**
     * Function sets three secret cards and deal remained
     * to the players.
     */
    void dealCards();
    
    /**
     * Function returns index of the first NULL pointer into players
     * array.
     */
    int getEmptyIndex();
    
    /**
     * Function returns pointer to Player with specified guest.
     * If there is no such a player, function returns NULL.
     */
    void * getPlayerByGuest(char guest);
    
    /**
     * Function inits duplicates of player's cards.
     */
    void initDuplicateCards();
    
    /**
     * Function sends to players start game information.
     */
    void sendStartInfo();
    
    /**
     * Function returns appartment id. If there is no appartment on
     * these coordinates return 0.
     */
    char getAppByCoordinates(char x, char y);
    
    /**
     * Function returns index of the next player who sits on the left
     * hand.
     */
    char getNextAnswerIndex(char current);
    
    /**
     * Function returns card id if it's in the cards array, else
     * returns 0.
     */
    char searchCard(char ap, char gt, char wp, const char * cards);
    
    /**
     * Function sets isQuestioning to false and run nextMove function.
     */
    void stopQuestioning();
    
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
    // Secret cards
    char SECRET_GT;
    char SECRET_WP;
    char SECRET_AP;
    // Server duplicates of the player's cards. In case some player
    // leave the room, server will show the cards of this player
    // from these duplicates.
    char ** dupCards;
    // Order of guest's moves
    char guestsOrder[6];
    // Index of the guest who must answer now.
    char curAnswerIndex;
    char curEnquiror;
    // Wait for somebody play intrigue card such as view the card.
    bool isWaitingIntrigue;
    // Current sustected cards
    char suspectGt;
    char suspectWp;
    char suspectAp;
    // Winner
    char winner;
    
    // Waiting threads
    pthread_t waitAnswerThread;
    pthread_t waitNextMoveThread;
    
    // Mutexes
    pthread_mutex_t chooseGuestMutex;
    pthread_mutex_t removePlayerMutex;
};

// Timer (waiting) functions
void * waitCheckGuestDistribution(void * ptr);
void * waitNextMove(void * ptr);
void * waitAutoAnswer(void * ptr);

const char MAX_GUESTS = 6;
const char ONE_TURN_DELAY = 30;
const char WAIT_ANSWER_DELAY = 20;
const char MAX_THINK_IMIT = 5;

// Guest constants
const char GT_SCARLETT = 1;
const char GT_MUSTARD = 2;
const char GT_WHITE = 3;
const char GT_GREEN = 4;
const char GT_PEACOCK = 5;
const char GT_PLUM = 6;
// Weapon constants
const char WP_CANDLESTICK = 7;
const char WP_KNIFE = 8;
const char WP_AXE = 9;
const char WP_PISTOL = 10;
const char WP_ROPE = 11;
const char WP_TROPHY = 12;
const char WP_BAT = 13;
const char WP_DUMBBELL = 14;
const char WP_POISON = 15;
// Appartments constants
const char AP_KITCHEN = 16;          // k
const char AP_BALLROOM = 17;         // B
const char AP_CONSERVATORY = 18;     // C
const char AP_DINING_ROOM = 19;      // d
const char AP_BILLIARD_ROOM = 20;    // b
const char AP_LIBRARY = 21;          // L
const char AP_LOUNGE = 22;           // l
const char AP_HALL = 23;             // h
const char AP_STUDY = 24;            // s

#endif /* _ROOM_ */
