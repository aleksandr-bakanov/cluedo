#ifndef _ROOM_MANAGER_
#define _ROOM_MANAGER_

#include <pthread.h>

/**
 * RoomManager class control room creating and deleting process,
 * place new players into rooms.
 */
class RoomManager
{
public:
//======================================================================
//  Methods
//======================================================================
    /**
     * Constructor. It creates arrays of pointers to Room objects.
     * Init these arrays by NULL pointers.
     */
    RoomManager();
    ~RoomManager();
    
    /**
     * Function looks for room of choisen type in which player could
     * be added, add player into it and return pointer to this room.
     * If there are no free room function creates one and place player
     * into it. If room can not be created function returns NULL
     * pointer.
     */
    void * addPlayerIntoRoom(void * player, char type);
//======================================================================
//  Fields
//======================================================================
    
//======================================================================
//  Consts
//======================================================================
    
private:
//======================================================================
//  Methods
//======================================================================
    /**
     * Function creates arrays of pointers to Room object and fill
     * them by NULL pointers.
     */
    void initRooms();
//======================================================================
//  Fields
//======================================================================
    // Arrays of pointers to Rooms which control games with
    // different players count.
    void ** threePlayerRooms;
    void ** fourPlayerRooms;
    void ** fivePlayerRooms;
    void ** sixPlayerRooms;
//======================================================================
//  Consts
//======================================================================
    // Maximum count of rooms of one type.
    short MAX_ONE_TYPE_ROOMS;
};

#endif /* _ROOM_MANAGER_ */
