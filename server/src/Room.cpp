#include "include/Room.hpp"
#include "include/Player.hpp"

char ** Room::map = NULL;

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
    dupCards = NULL;
    chooseGuestMutex = PTHREAD_MUTEX_INITIALIZER;
    removePlayerMutex = PTHREAD_MUTEX_INITIALIZER;
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
        // This mutex will be unlocked after finish startGame function.
        pthread_mutex_lock(&removePlayerMutex);
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
    pthread_mutex_lock(&removePlayerMutex);
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
    pthread_mutex_unlock(&removePlayerMutex);
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
    initDuplicateCards();
    curPlayerIndex = 0;
    shuffleGuests();
    dealCards();
    // This mutex was locked in addPlayer function when condition
    // (curPlayersCount == totalPlayers) generates true.
    pthread_mutex_unlock(&removePlayerMutex);
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
Room::shuffleGuests()
{
    char gt;
    int index;
    for (index = 0; index < 6; index++)
    {
        guestsOrder[index] = index + 1;
        if (getPlayerByGuest(index + 1))
            guestsOrder[index] += 10;
    }
    for (int i = 6; i > 1; i--)
    {
        index = rand() % i;
        gt = guestsOrder[index];
        guestsOrder[index] = guestsOrder[i - 1];
        guestsOrder[i - 1] = gt;
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
    if (guest == GT_SCARLETT) {
        x = 0; y = 17;
    }
    else if (guest == GT_GREEN) {
        x = 9; y = 0;
    }
    else if (guest == GT_MUSTARD) {
        x = 14; y = 0;
    }
    else if (guest == GT_PEACOCK) {
        x = 23; y = 6;
    }
    else if (guest == GT_PLUM) {
        x = 23; y = 19;
    }
    else if (guest == GT_WHITE) {
        x = 7; y = 24;
    }
}

void
Room::dealCards()
{
    char count = 25;
    char card;
    char cards[count];
    int i;
    for (i = 0; i < count; i++)
        cards[i] = i + 1;
    SECRET_GT = (rand() % 6) + 1;
    SECRET_WP = (rand() % 9) + 7;
    SECRET_AP = (rand() % 10) + 16;
    cards[SECRET_GT - 1] = cards[count-- - 1];
    cards[SECRET_WP - 1] = cards[count-- - 1];
    cards[SECRET_AP - 1] = cards[count-- - 1];
    Player * pl;
    char index;
    while (count)
    {
        for (i = 0; i < 6 && count; i++)
        {
            if (guestsOrder[i] > 10)
            {
                pl = (Player *)getPlayerByGuest(guestsOrder[i] - 10);
                card = (rand() % count) + 1;
                index = pl->addCard(card);
                dupCards[guestsOrder[i] - 11][index] = card;
                cards[card - 1] = cards[count-- - 1];
            }
        }
    }
}

void *
Room::getPlayerByGuest(char guest)
{
    pthread_mutex_lock(&removePlayerMutex);
    Player ** pls = (Player **)players;
    Player * pl;
    for (char i = 0; i < totalPlayers; i++)
        if (pl = pls[i])
            if (pl->guest == guest)
            {
                pthread_mutex_unlock(&removePlayerMutex);
                return (void *)pl;
            }
    pthread_mutex_unlock(&removePlayerMutex);
    return (void *)NULL;
}

void
Room::initDuplicateCards()
{
    char i;
    if (dupCards)
    {
        for (i = 0; i < 6; i++)
            if (dupCards[i])
                delete [] dupCards[i];
        delete [] dupCards;
        dupCards = NULL;
    }
    dupCards = new char*[6];
    Player * pl;
    for (i = 0; i < 6; i++)
    {
        if (pl = (Player *)getPlayerByGuest(i + 1))
            dupCards[i] = new char[MAX_CARDS];
        else
            dupCards[i] = NULL;
    }
}
