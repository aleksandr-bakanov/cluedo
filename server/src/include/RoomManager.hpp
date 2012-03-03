#ifndef _ROOM_MANAGER_
#define _ROOM_MANAGER_

#include <pthread.h>
#include <string.h>
#include <fstream>
#include <iostream>
using namespace std;

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
	 * into it. If room can not be created function returns false.
	 * On success function returns true.
	 */
	bool addPlayerIntoRoom(void * player, char type);
//======================================================================
//  Fields
//======================================================================
	
private:
//======================================================================
//  Methods
//======================================================================
	/**
	 * Function creates arrays of pointers to Room object and then
	 * creates Room objects.
	 */
	void initRooms();
	
	/**
	 * Function read map from files/map file and store it into
	 * Room::map static variable.
	 */
	void initMap();
	
	/**
	 * Function returns pointer to not full filled room of specified
	 * type. If there isn't room it will be created. If room cannot be
	 * created the function returns NULL pointer.
	 */
	void * getFreeRoom(char type);
	
	/**
	 * Function creates room of specified type and return pointer to it.
	 */
	void * createRoom(char type);
	
	/**
	 * Function returns index of maximum filled room of specified type.
	 * If there is no free room function returns -1.
	 */
	void * getMaxFilledRoom(char type);
	
	/**
	 * Function returns pointer to array of rooms of specified type,
	 * also it assign maximum index in this array to maxP.
	 */
	void ** getRooms(char type);
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
	// Mutexes
	pthread_mutex_t addPlayerMutex;
};

#endif /* _ROOM_MANAGER_ */
