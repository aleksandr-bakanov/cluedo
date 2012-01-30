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
    {
        isOpen = false;
        pthread_t t;
        pthread_create( &t, NULL, waitCheckGuestDistribution,
                        (void*)this);
        pthread_detach(t);
    }
}

void 
Room::removePlayer(void * player)
{
    Player ** pls = (Player **)players;
    Player * pl = (Player *)player;
    pl->room = NULL;
    for (int i = 0; i < totalPlayers; i++)
    {
        if (pls[i] == pl)
        {
            pls[i] = NULL;
            if (pl->guest && isOpen)
            {
                pthread_mutex_lock(&chooseGuestMutex);
                char mask = char(pow(2, pl->guest - 1));
                availableGuests |= mask;
                for (int j = 0; j < totalPlayers; j++)
                    if (pl = pls[j])
                        pl->sendAvailableGuests(availableGuests);
                pthread_mutex_unlock(&chooseGuestMutex);
            }
            break;
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
    cout << "Start game" << endl;
    setPlayersStartParams();
    shufflePlayers();
    curPlayerIndex = 0;
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

void *
waitCheckGuestDistribution(void * ptr)
{
    sleep(5);
    Room * r = (Room *)ptr;
    r->checkGuestDistribution();
}

void 
Room::checkGuestDistribution()
{
    pthread_mutex_lock(&chooseGuestMutex);
    char arr[] = {0, 0, 0, 0, 0, 0};
    char count = 0;
    for (char i = 0; i < 6; i++)
        if (availableGuests & char(pow(2, i)))
            arr[count++] = i + 1;
    availableGuests = 0;
    Player ** pls = (Player **)players;
    Player * pl;
    for (char j = 0; j < totalPlayers; j++)
        if (pl = pls[j])
            if (!pl->guest)
            {
                char index = rand() % count;
                pl->guest = arr[index];
                arr[index] = arr[--count];
            }
    pthread_mutex_unlock(&chooseGuestMutex);
    startGame();
}

void
Room::shufflePlayers()
{
    void * pl;
    int index;
    for (int i = totalPlayers; i > 1; i--)
    {
        index = rand() % i;
        pl = players[index];
        players[index] = players[i - 1];
        players[i - 1] = pl;
    }
}

void
Room::setPlayersStartParams()
{
    Player ** pls = (Player **)players;
    Player * pl;
    for (char i = 0; i < totalPlayers; i++)
        if (pl = pls[i])
        {
            pl->inGame = true;
            setStartCoordinates(pl->x, pl->y, pl->guest);
        }
}

void
Room::setStartCoordinates(char &x, char &y, char guest)
{
    x = y = 0;
}
