#include "include/Room.hpp"
#include "include/Player.hpp"

Room::Room(char type)
{
    curPlayersCount = 0;
    totalPlayers = type;
    Player ** pl = (Player **)players;
    pl = new Player*[totalPlayers];
    memset(players, 0, sizeof(pl));
    players = (void **)pl;
    isOpen = true;
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
    curPlayersCount++;
    if (curPlayersCount == totalPlayers)
        startGame();
}

void 
Room::removePlayer(void * player)
{
    Player ** pls = (Player **)players;
    Player * pl = (Player *)player;
    for (int i = 0; i < totalPlayers; i++)
        if (pls[i] == pl)
            pls[i] = NULL;
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
