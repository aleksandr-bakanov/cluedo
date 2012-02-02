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
    pl = (Player *)player;
    pl->resetGameInfo();
    pl->room = NULL;
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
    curGuestIndex = curMove = -1;
    shuffleGuests();
    dealCards();
    sendStartInfo();
    // This mutex was locked in addPlayer function when condition
    // (curPlayersCount == totalPlayers) generates true.
    pthread_mutex_unlock(&removePlayerMutex);
    nextMove();
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

void 
Room::checkGuestDistribution()
{
    pthread_mutex_lock(&chooseGuestMutex);
    char arr[] = {0, 0, 0, 0, 0, 0};
    char count = 0;
    for (char i = 0; i < MAX_GUESTS; i++)
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
    for (index = 0; index < MAX_GUESTS; index++)
    {
        guestsOrder[index] = index + 1;
        if (getPlayerByGuest(index + 1))
            guestsOrder[index] += 10;
    }
    for (int i = MAX_GUESTS; i > 1; i--)
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
    char count = 24;
    char card;
    char cards[count];
    int i;
    for (i = 0; i < count; i++)
        cards[i] = i + 1;
    SECRET_GT = (rand() % MAX_GUESTS) + 1;
    SECRET_WP = (rand() % 9) + 7;
    SECRET_AP = (rand() % 10) + 16;
    cards[SECRET_GT - 1] = cards[count-- - 1];
    cards[SECRET_WP - 1] = cards[count-- - 1];
    cards[SECRET_AP - 1] = cards[count-- - 1];
    Player * pl;
    char index, in;
    while (count)
    {
        for (i = 0; i < MAX_GUESTS && count; i++)
        {
            if (guestsOrder[i] > 10)
            {
                pl = (Player *)getPlayerByGuest(guestsOrder[i] - 10);
                in = rand() % count;
                card = cards[in];
                index = pl->addCard(card);
                dupCards[guestsOrder[i] - 11][index] = card;
                cards[in] = cards[count-- - 1];
            }
        }
    }
}

void *
Room::getPlayerByGuest(char guest)
{
    Player ** pls = (Player **)players;
    Player * pl;
    for (char i = 0; i < totalPlayers; i++)
        if (pl = pls[i])
            if (pl->guest == guest)
                return (void *)pl;
    return (void *)NULL;
}

void
Room::initDuplicateCards()
{
    char i;
    if (dupCards)
    {
        for (i = 0; i < MAX_GUESTS; i++)
            if (dupCards[i])
                delete [] dupCards[i];
        delete [] dupCards;
        dupCards = NULL;
    }
    dupCards = new char*[MAX_GUESTS];
    Player * pl;
    for (i = 0; i < MAX_GUESTS; i++)
    {
        if (pl = (Player *)getPlayerByGuest(i + 1))
            dupCards[i] = new char[MAX_CARDS];
        else
            dupCards[i] = NULL;
    }
}

void
Room::sendStartInfo()
{
    Player ** pls = (Player **)players;
    Player * pl;
    for (int i = 0; i < totalPlayers; i++)
        if (pl = pls[i])
            pl->sendStartInfo();
}

void
Room::nextMove()
{
    pthread_mutex_lock(&removePlayerMutex);
    curMove++;
    curGuestIndex++;
    Player * pl = NULL;
    int i;
    char gt;
    for (i = 0; i < MAX_GUESTS && !pl; i++)
    {
        gt = guestsOrder[curGuestIndex];
        if (gt > 10)
            pl = (Player *)getPlayerByGuest(gt - 10);
        if (pl)
            break;
        else
            curGuestIndex++;
        if (curGuestIndex == MAX_GUESTS)
            curGuestIndex = 0;
    }
    if (i == MAX_GUESTS && !pl) endGame();

    if (pl)
    {
        Player ** pls = (Player **)players;
        char firstDie = (rand() % 6) + 1;
        char secondDie = (rand() % 6) + 1;
        pl->steps = firstDie + (secondDie != 1 ? secondDie : 0);
        /// TODO: send intrigue if secondDie == 1
        for (i = 0; i < totalPlayers; i++)
            if (pl = pls[i])
            {
                pl->myTurn = pl->guest == gt;
                pl->sendNextMove(gt, firstDie, secondDie);
            }
    }
    pthread_t t;
    pthread_create( &t, NULL, waitNextMove, (void*)this);
    pthread_detach(t);
    pthread_mutex_unlock(&removePlayerMutex);
}

void
Room::endGame()
{
    cout << "Room::endGame\n";
}

void
Room::guestMakeStep(void * player, char x, char y)
{
    pthread_mutex_lock(&removePlayerMutex);
    Player * pl = (Player *)player;
    char gt = pl->guest;
    bool success = false;
    if (x >= 0 && x <= 23 && y >= 0 && y <= 24)
    {
        char c = Room::map[y][x];
        char pc = Room::map[pl->y][pl->x];
        if (c != '#')
        {
            char nextApp = getAppByCoordinates(x, y);
            if ((pl->x == x && abs(pl->y - y) == 1) ||
                (pl->y == y && abs(pl->x - x) == 1) ||
                (pc == 'k' && c == 's') || (pc == 's' && c == 'k') ||
                (pc == 'l' && c == 'C') || (pc == 'C' && c == 'l') &&
                !(c != ":" && (nextApp == pl->lastAskedApp)))
            {
                pl->x = x;
                pl->y = y;
                pl->app = nextApp;
                pl->mustAsk = bool(nextApp);
                c != ':' ? pl->steps = 0 : pl->steps--;
                if (c == ':' && pl->steps == 0)
                    pl->lastAskedApp = 0;
                success = true;
            }
        }
    }
    if (success)
    {
        Player ** pls = (Player **)players;
        for (int i = 0; i < totalPlayers; i++)
            if (pl = pls[i])
                pl->sendGuestMakeStep(gt, x, y);
    }
    pthread_mutex_unlock(&removePlayerMutex);
}

char
Room::getAppByCoordinates(char x, char y)
{
    char r = 0;
    if (x == 4 && y == 6)
        r = AP_KITCHEN;
    else if ((x == 8 && y == 5) || (x == 15 && y == 5) ||
             (x == 9 && y == 7) || (x == 14 && y == 7))
        r = AP_BALLROOM;
    else if (x == 19 && y == 5)
        r = AP_CONSERVATORY;
    else if ((x == 7 && y == 12) || (x == 6 && y == 15))
        r = AP_DINING_ROOM;
    else if ((x == 18 && y == 9) || (x == 22 && y == 12))
        r = AP_BILLIARD_ROOM;
    else if ((x == 20 && y == 14) || (x == 17 && y == 16))
        r = AP_LIBRARY;
    else if (x == 6 && y == 19)
        r = AP_LOUNGE;
    else if (((x == 11 || x == 12) && y == 18) || (x == 14 && y == 20))
        r = AP_HALL;
    else if (x == 17 && y == 21)
        r = AP_STUDY;
    return r;
}

void
waitCheckGuestDistribution(void * ptr)
{
    sleep(5);
    Room * r = (Room *)ptr;
    r->checkGuestDistribution();
}

void *
waitNextMove(void * ptr)
{
    Room * r = (Room *)ptr;
    int cm = r->curMove;
    sleep(ONE_TURN_DELAY);
    if (cm == r->curMove)
        r->nextMove();
}
