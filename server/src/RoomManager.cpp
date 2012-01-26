#include "include/RoomManager.hpp"
#include "include/Room.hpp"

RoomManager::RoomManager()
{
    MAX_ONE_TYPE_ROOMS = 100;
    initRooms();
}

RoomManager::~RoomManager()
{
    delete threePlayerRooms;
    delete fourPlayerRooms;
    delete fivePlayerRooms;
    delete sixPlayerRooms;
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
        threePlayerRooms[i] = NULL;
        fourPlayerRooms[i] = NULL;
        fivePlayerRooms[i] = NULL;
        sixPlayerRooms[i] = NULL;
    }
}

