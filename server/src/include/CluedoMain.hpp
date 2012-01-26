#ifndef _CLUEDO_MAIN_
#define _CLUEDO_MAIN_

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "RoomManager.hpp"
#include "Player.hpp"

using namespace std;

struct PlayerStartParams
{
    void * roomManagerPointer;
    int socketFd;
};

/**
 * Function configures server socket. Create it, bind, and make
 * it listen.
 */
void configureServerSocket(int&, struct sockaddr_in&);

/**
 * Function create Player object, save in it client socket descriptor
 * and pointer to RoomManager object.
 */
void * createAndRunPlayer(void * ptr);

/**
 * Function prints error message and finish program by exit() call.
 */
void error(const char *);

#endif /* _CLUEDO_MAIN_ */
