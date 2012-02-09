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
    cout << "Start game: room = " << this << endl;
    isWaitingIntrigue = isQuestioning = false;
    winner = 0;
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
            pl->isLose = false;
            setStartCoordinates(pl->x, pl->y, pl->guest);
        }
}

void
Room::setStartCoordinates(char &x, char &y, char guest)
{
    if (guest == GT_SCARLETT) {
        x = 0; y = 17;
    }
    else if (guest == GT_MUSTARD) {
        x = 9; y = 0;
    }
    else if (guest == GT_WHITE) {
        x = 14; y = 0;
    }
    else if (guest == GT_GREEN) {
        x = 23; y = 6;
    }
    else if (guest == GT_PEACOCK) {
        x = 23; y = 19;
    }
    else if (guest == GT_PLUM) {
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
        {
            dupCards[i] = new char[MAX_CARDS];
            for (int j = 0; j < MAX_CARDS; j++)
                dupCards[i][j] = 0;
        }
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
            pl->sendStartInfo(guestsOrder);
}

void
Room::nextMove()
{
    pthread_mutex_lock(&removePlayerMutex);
    curMove++;
    curGuestIndex++;
    if (curGuestIndex == MAX_GUESTS)
        curGuestIndex = 0;
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
    if (i == MAX_GUESTS && !pl)
        endGame();
    else
    {
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
        pthread_create(&waitNextMoveThread, NULL, waitNextMove, (void*)this);
        pthread_detach(waitNextMoveThread);
    }
    pthread_mutex_unlock(&removePlayerMutex);
}

void
Room::endGame()
{
    Player ** pls = (Player **)players;
    Player * pl;
    for (int i = 0; i < totalPlayers; i++)
        if (pl = pls[i])
        {
            pl->resetGameInfo();
            pl->room = NULL;
            pls[i] = NULL;
        }
    
    curPlayersCount = 0;
    isOpen = true;
    availableGuests = 63;
    cout << "Room::endGame\n";
}

void
Room::guestMakeMove(void * player, char x, char y)
{
    pthread_mutex_lock(&removePlayerMutex);
    Player ** pls = (Player **)players;
    Player * pl = (Player *)player;
    char gt = pl->guest;
    bool success = false;
    vector<char> v;
    int i;
    
    if (x >= 0 && x <= 23 && y >= 0 && y <= 24)
    {
        char c = Room::map[y][x];
        char pc = Room::map[pl->y][pl->x];
        if (c != '#')
        {
            char nextApp = getAppByCoordinates(x, y);
            // Secret paths
            if ((pc == 'k' && c == 's') || (pc == 's' && c == 'k') ||
                (pc == 'l' && c == 'C') || (pc == 'C' && c == 'l'))
            {
                pl->steps = 0;
                pl->app = nextApp;
                pl->mustAsk = true;
                success = true;
                v.clear();
                v.push_back(y);
                v.push_back(x);
            }
            // Passing in rooms
            else if ((pc == 'B' && c == 'B') || (pc == 'b' && c == 'b') ||
                     (pc == 'd' && c == 'd') || (pc == 'L' && c == 'L') ||
                     (pc == 'h' && c == 'h'))
            {
                pl->app = nextApp;
                success = true;
                v.clear();
                v.push_back(y);
                v.push_back(x);
            }
            // No secret path
            else
            {
                bool available = true;
                for (i = 0; i < totalPlayers; i++)
                    if (pl = pls[i])
                        if (pl->x == x && pl->y == y)
                            available = false;

                if ((c == ':' && available) ||
                    (c != ':' && nextApp != pl->lastAskedApp))
                {
                    v.clear();
                    /// TODO: Add into v intrigue cells
                    pl = (Player *)player;
                    pl->aStar(pl->x, pl->y, x, y, v);
                    if (v.size() && (v.size() / 2 <= pl->steps))
                    {
                        pl->steps -= v.size() / 2;
                        pl->app = nextApp;
                        pl->mustAsk = bool(nextApp);
                        if (c == ':' && pl->steps == 0)
                            pl->lastAskedApp = 0;
                        success = true;
                    }
                }
            }
        }
    }
    if (success)
    {
        for (i = 0; i < totalPlayers; i++)
            if (pl = pls[i])
                pl->sendGuestMakeMove(gt, v);
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
Room::moveGuestToRoom(char gt, char ap)
{
    Player ** pls = (Player **)players;
    Player * pl = (Player *)getPlayerByGuest(gt);
    vector<char> v;
    v.clear();
    char x, y;
    getAppCoordinates(ap, x, y);
    if (pl)
    {
        pl->x = x;
        pl->y = y;
        pl->app = ap;
    }
    v.push_back(y);
    v.push_back(x);
    for (int i = 0; i < totalPlayers; i++)
        if (pl = pls[i])
            pl->sendGuestMakeMove(gt, v);
}

void
Room::getAppCoordinates(char ap, char &x, char &y)
{
    if (ap == AP_KITCHEN) {
        x = 4; y = 6;
    }
    else if (ap == AP_BALLROOM) {
        x = 8; y = 5;
    }
    else if (ap == AP_CONSERVATORY) {
        x = 19; y = 5;
    }
    else if (ap == AP_DINING_ROOM) {
        x = 7; y = 12;
    }
    else if (ap == AP_BILLIARD_ROOM) {
        x = 18; y = 9;
    }
    else if (ap == AP_LIBRARY) {
        x = 20; y = 14;
    }
    else if (ap == AP_LOUNGE) {
        x = 6; y = 19;
    }
    else if (ap == AP_HALL) {
        x = 11; y = 18;
    }
    else if (ap == AP_STUDY) {
        x = 17; y = 21;
    }
}

void
Room::playerGuessSecret(void * player, char ap, char gt, char wp)
{
    pthread_mutex_lock(&removePlayerMutex);
    Player ** pls = (Player **)players;
    Player * pl = (Player *)player;
    char guestId = pl->guest;
    if (ap == SECRET_AP && gt == SECRET_GT && wp == SECRET_WP)
        winner = guestId;
    else
        pl->isLose = true;
    for (int i = 0; i < totalPlayers; i++)
        if (pl = pls[i])
            if (winner)
                pl->sendGuessSecret(guestId, ap, gt, wp);
            else
                pl->sendGuessSecret(guestId, 0, 0, 0);
    if (winner)
    {
        endGame();
    }
    pthread_mutex_unlock(&removePlayerMutex);
}

void
Room::playerAsk(void * player, char guest, char weapon)
{
    if (!isQuestioning)
    {
        pthread_mutex_lock(&removePlayerMutex);
        // Protect from waitNextMove function
        pthread_cancel(waitNextMoveThread);
        isQuestioning = true;
        char cardToSend = 0;
        // Send S_PLAYER_ASK
        Player ** pls = (Player **)players;
        Player * pl = (Player *)player;
        pl->lastAskedApp = pl->app;
        pl->mustAsk = false;
        char enquirer = pl->guest;
        curEnquiror = enquirer;
        suspectAp = pl->app;
        suspectGt = guest;
        suspectWp = weapon;
        int i;
        char gt;
        for (i = 0; i < totalPlayers; i++)
            if (pl = pls[i])
                pl->sendPlayerAsk(enquirer, suspectAp, guest, weapon);
        // Move guest
        moveGuestToRoom(suspectGt, suspectAp);

        curAnswerIndex = getNextAnswerIndex(curGuestIndex);
        pl = (Player *)getPlayerByGuest(guestsOrder[curAnswerIndex]);
        while (!pl)
        {
            if (guestsOrder[curAnswerIndex] > 10)
            {
                cardToSend = searchCard(suspectAp, suspectGt, suspectWp,
                                        dupCards[curAnswerIndex]);
                gt = guestsOrder[curAnswerIndex];
                for (i = 0; i < totalPlayers; i++)
                    if (pl = pls[i])
                        pl->sendWaitAnswer(gt, WAIT_ANSWER_DELAY);
                sleep(MAX_THINK_IMIT);
                if (cardToSend)
                    break;
                else
                {
                    for (i = 0; i < totalPlayers; i++)
                        if (pl = pls[i])
                            pl->sendNoCards(gt, suspectAp, suspectGt,
                                            suspectWp);
                }
            }
            curAnswerIndex = getNextAnswerIndex(curAnswerIndex);
            pl = (Player*)getPlayerByGuest(guestsOrder[curAnswerIndex]);
        }
        if (pl && pl != player)
        {
            gt = guestsOrder[curAnswerIndex];
            for (i = 0; i < totalPlayers; i++)
                if (pl = pls[i])
                    pl->sendWaitAnswer(gt, WAIT_ANSWER_DELAY);
            pthread_create(&waitAnswerThread, NULL, waitAutoAnswer, (void*)this);
            pthread_detach(waitAnswerThread);
        }
        else if (pl && pl == player)
        {
            stopQuestioning();
        }
        if (cardToSend)
        {
            for (i = 0; i < totalPlayers; i++)
                if (pl = pls[i])
                    pl->sendPlayerAnswer(guestsOrder[curAnswerIndex],
                                         pl == player ? cardToSend : 0);
            stopQuestioning();
        }
        pthread_mutex_unlock(&removePlayerMutex);
    }
}

char
Room::getNextAnswerIndex(char current)
{
    char i = current + 1;
    for (; i != curGuestIndex; i++)
    {
        if (i == MAX_GUESTS) i = 0;
        if (guestsOrder[i] > 10)
            return i;
    }
    return curGuestIndex;
}

char
Room::searchCard(char ap, char gt, char wp, const char * cards)
{
    if (ap > 0 && ap < 25 && gt > 0 && gt < 25 && wp > 0 && wp < 25)
        for (int i = 0; i < MAX_CARDS; i++)
            if (cards[i] == ap || cards[i] == wp || cards[i] == gt)
                return cards[i];
    return 0;
}

void
Room::playerAnswer(void * player, char card)
{
    if (isQuestioning)
    {
        pthread_mutex_lock(&removePlayerMutex);
        Player ** pls = (Player **)players;
        Player * pl = (Player *)player;
        char gt = guestsOrder[curAnswerIndex];
        int i;
        if (gt == pl->guest)
        {
            if (searchCard(card, card, card, (char *)(pl->cards)))
            {
                pthread_cancel(waitAnswerThread);
                for (i = 0; i < totalPlayers; i++)
                    if (pl = pls[i])
                        pl->sendPlayerAnswer(gt,
                                   pl->guest == curEnquiror ? card : 0);
                stopQuestioning();
            }
            else if (card < 0)
            {
                pthread_cancel(waitAnswerThread);
                for (i = 0; i < totalPlayers; i++)
                    if (pl = pls[i])
                        pl->sendNoCards(gt, suspectAp, suspectGt,
                                        suspectWp);
                // Wait information from next player
                curAnswerIndex = getNextAnswerIndex(curGuestIndex);
                pl = (Player *)getPlayerByGuest(guestsOrder[curAnswerIndex]);
                char cardToSend = 0;
                while (!pl)
                {
                    if (guestsOrder[curAnswerIndex] > 10)
                    {
                        cardToSend = searchCard(suspectAp, suspectGt, suspectWp,
                                                dupCards[curAnswerIndex]);
                        gt = guestsOrder[curAnswerIndex];
                        for (i = 0; i < totalPlayers; i++)
                            if (pl = pls[i])
                                pl->sendWaitAnswer(gt, WAIT_ANSWER_DELAY);
                        sleep(MAX_THINK_IMIT);
                        if (cardToSend)
                            break;
                        else
                        {
                            for (i = 0; i < totalPlayers; i++)
                                if (pl = pls[i])
                                    pl->sendNoCards(gt, suspectAp, suspectGt,
                                                    suspectWp);
                        }
                    }
                    curAnswerIndex = getNextAnswerIndex(curAnswerIndex);
                    pl = (Player*)getPlayerByGuest(guestsOrder[curAnswerIndex]);
                }
                if (pl && pl->guest != curEnquiror)
                {
                    gt = guestsOrder[curAnswerIndex];
                    for (i = 0; i < totalPlayers; i++)
                        if (pl = pls[i])
                            pl->sendWaitAnswer(gt, WAIT_ANSWER_DELAY);
                    pthread_create(&waitAnswerThread, NULL, waitAutoAnswer, (void*)this);
                    pthread_detach(waitAnswerThread);
                }
                else if (pl && pl->guest == curEnquiror)
                {
                    stopQuestioning();
                }
                if (cardToSend)
                {
                    for (i = 0; i < totalPlayers; i++)
                        if (pl = pls[i])
                            pl->sendPlayerAnswer(guestsOrder[curAnswerIndex],
                                    pl->guest == curEnquiror ? cardToSend : 0);
                    stopQuestioning();
                }
            }
        }
        pthread_mutex_unlock(&removePlayerMutex);
    }
}

void
Room::autoAnswer()
{
    pthread_mutex_lock(&removePlayerMutex);
    Player ** pls = (Player **)players;
    Player* pl = (Player*)getPlayerByGuest(guestsOrder[curAnswerIndex]);
    char gt;
    int i;

    char cardToSend = searchCard(suspectAp, suspectGt, suspectWp,
                            dupCards[curAnswerIndex]);
    gt = guestsOrder[curAnswerIndex];
    if (cardToSend)
        for (i = 0; i < totalPlayers; i++)
            if (pl = pls[i])
                pl->sendPlayerAnswer(gt,
                            pl->guest == curEnquiror ? cardToSend : 0);
    else
        for (i = 0; i < totalPlayers; i++)
            if (pl = pls[i])
                pl->sendNoCards(gt, suspectAp, suspectGt,
                                suspectWp);
        
    if (!cardToSend)
    {
        curAnswerIndex = getNextAnswerIndex(curAnswerIndex);
        pl = (Player*)getPlayerByGuest(guestsOrder[curAnswerIndex]);
        while (!pl)
        {
            if (guestsOrder[curAnswerIndex] > 10)
            {
                cardToSend = searchCard(suspectAp, suspectGt, suspectWp,
                                        dupCards[curAnswerIndex]);
                gt = guestsOrder[curAnswerIndex];
                for (i = 0; i < totalPlayers; i++)
                    if (pl = pls[i])
                        pl->sendWaitAnswer(gt, WAIT_ANSWER_DELAY);
                sleep(MAX_THINK_IMIT);
                if (cardToSend)
                    break;
                else
                {
                    for (i = 0; i < totalPlayers; i++)
                        if (pl = pls[i])
                            pl->sendNoCards(gt, suspectAp, suspectGt,
                                            suspectWp);
                }
            }
            curAnswerIndex = getNextAnswerIndex(curAnswerIndex);
            pl = (Player*)getPlayerByGuest(guestsOrder[curAnswerIndex]);
        }
        if (pl && pl->guest != curEnquiror)
        {
            gt = guestsOrder[curAnswerIndex];
            for (i = 0; i < totalPlayers; i++)
                if (pl = pls[i])
                    pl->sendWaitAnswer(gt, WAIT_ANSWER_DELAY);
            pthread_create(&waitAnswerThread, NULL, waitAutoAnswer, (void*)this);
            pthread_detach(waitAnswerThread);
        }
        else if (pl && pl->guest == curEnquiror)
        {
            stopQuestioning();
        }
        if (cardToSend)
        {
            for (i = 0; i < totalPlayers; i++)
                if (pl = pls[i])
                    pl->sendPlayerAnswer(guestsOrder[curAnswerIndex],
                            pl->guest == curEnquiror ? cardToSend : 0);
            stopQuestioning();
        }
    }
    else
        stopQuestioning();
    pthread_mutex_unlock(&removePlayerMutex);
}

void
Room::stopQuestioning()
{
    isQuestioning = false;
    pthread_mutex_unlock(&removePlayerMutex);
    nextMove();
}

void *
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
    if (cm == r->curMove && !r->isQuestioning)
        r->nextMove();
}

void *
waitAutoAnswer(void * ptr)
{
    sleep(WAIT_ANSWER_DELAY);
    Room * r = (Room *)ptr;
    r->autoAnswer();
}
