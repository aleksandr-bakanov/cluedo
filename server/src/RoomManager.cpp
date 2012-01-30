#include "include/RoomManager.hpp"
#include "include/Room.hpp"

RoomManager::RoomManager()
{
    addPlayerMutex = PTHREAD_MUTEX_INITIALIZER;
    MAX_ONE_TYPE_ROOMS = 10;
    initRooms();
}

RoomManager::~RoomManager()
{
    delete [] threePlayerRooms;
    delete [] fourPlayerRooms;
    delete [] fivePlayerRooms;
    delete [] sixPlayerRooms;
}

void
RoomManager::initRooms()
{
    threePlayerRooms = (void**) new Room*[MAX_ONE_TYPE_ROOMS];
    fourPlayerRooms = (void**) new Room*[MAX_ONE_TYPE_ROOMS];
    fivePlayerRooms = (void**) new Room*[MAX_ONE_TYPE_ROOMS];
    sixPlayerRooms = (void**) new Room*[MAX_ONE_TYPE_ROOMS];
    for (int i = 0; i < MAX_ONE_TYPE_ROOMS; i++)
    {
        threePlayerRooms[i] = new Room(3);
        fourPlayerRooms[i] = new Room(4);
        fivePlayerRooms[i] = new Room(5);
        sixPlayerRooms[i] = new Room(6);
    }
}

bool 
RoomManager::addPlayerIntoRoom(void * player, char type)
{
    pthread_mutex_lock(&addPlayerMutex);
    Room * room = (Room *)getMaxFilledRoom(type);
    if (room)
    {
        room->addPlayer(player);
        pthread_mutex_unlock(&addPlayerMutex);
        return true;
    }
    else
    {
        pthread_mutex_unlock(&addPlayerMutex);
        return false;
    }
}

void * 
RoomManager::getMaxFilledRoom(char type)
{
    Room ** rArr = (Room **)getRooms(type);
    Room * r = NULL;
    int id = -1;
    int curCount = -1;
    for (int i = 0; i < MAX_ONE_TYPE_ROOMS; i++)
    {
        r = rArr[i];
        if (r->isOpen) 
            if (r->curPlayersCount > curCount) 
            {
                curCount = r->curPlayersCount;
                id = i;
            }
    }
    if (id < 0)
        return (void *)NULL;
    else
        return (void *)(rArr[id]);
}

void ** 
RoomManager::getRooms(char type)
{
    Room ** rArr;
    if (type == 3) 
        rArr = (Room **)threePlayerRooms;
    else if (type == 4) 
        rArr = (Room **)fourPlayerRooms;
    else if (type == 5) 
        rArr = (Room **)fivePlayerRooms;
    else if (type == 6) 
        rArr = (Room **)sixPlayerRooms;
    return (void **)rArr;
}
