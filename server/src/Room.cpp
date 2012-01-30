#include "include/Room.hpp"
#include "include/Player.hpp"

Room::Room(char type)
{
    curPlayersCount = 0;
    totalPlayers = type;
    Player ** pl = (Player **)players;
    pl = new Player*[totalPlayers];
    for (int i = 0; i < totalPlayers; i++)
        pl[i] = NULL;
    players = (void **)pl;
    isOpen = true;
    availableGuests = 63; // 00111111 - all guests available
    chooseGuestMutex = PTHREAD_MUTEX_INITIALIZER;
}

Room::~Room()
{
    Player ** pl = (Player **)players;
    delete [] pl;
}

void 
Room::addPlayer(void * player)
{
    Player ** pls = (Player **)players;
    Player * pl = (Player *)player;
    pls[getEmptyIndex()] = pl;
    pl->room = (void *)this;
    pl->resetGameInfo();
    pl->sendAvailableGuests(availableGuests);
    curPlayersCount++;
    
    if (curPlayersCount == totalPlayers)
        startGame();
}

void 
Room::removePlayer(void * player)
{
    Player ** pls = (Player **)players;
    Player * pl = (Player *)player;
    pl->room = NULL;
    for (int i = 0; i < totalPlayers; i++)
        if (pls[i] == pl)
        {
            pls[i] = NULL;
            if (pl->guest)
            {
                pthread_mutex_lock(&chooseGuestMutex);
                char mask = char(pow(2, pl->guest - 1));
                availableGuests |= mask;
                for (int j = 0; j < totalPlayers; j++)
                    if (pl = pls[j])
                        pl->sendAvailableGuests(availableGuests);
                pthread_mutex_unlock(&chooseGuestMutex);
            }
        }
    curPlayersCount--;
}

int 
Room::getEmptyIndex()
{
    for (int i = 0; i < totalPlayers; i++)
        if (!players[i]) 
            return i;
}

void 
Room::startGame()
{
    isOpen = false;
    cout << "Start game" << endl;
}

bool 
Room::chooseGuest(char guestId)
{
    pthread_mutex_lock(&chooseGuestMutex);
    char mask = char(pow(2, guestId - 1));
    if (mask & availableGuests)
    {
        availableGuests &= ~mask;
        Player ** pls = (Player **)players;
        Player * pl;
        for (int i = 0; i < totalPlayers; i++)
            if (pl = pls[i])
                pl->sendAvailableGuests(availableGuests);
        pthread_mutex_unlock(&chooseGuestMutex);
        return true;
    }
    else
    {
        pthread_mutex_unlock(&chooseGuestMutex);
        return false;
    }
}
