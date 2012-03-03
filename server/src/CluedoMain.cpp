#include "include/CluedoMain.hpp"

static void
sigpipeHandler(int sig, siginfo_t *si, void *unused)
{
	cout << "SIGPIPE: " << long(si->si_addr) << endl;
}

int 
main()
{
	int serverSocket, clisockfd, n;
	socklen_t clilen;
	struct sockaddr_in serverAddr, cli_addr;
	
	configureServerSocket(serverSocket, serverAddr);
	configureSignals();
	
	RoomManager * roomManager = new RoomManager();
	struct PlayerStartParams * ptr;
	pthread_t playerThread;
	
	clilen = sizeof(cli_addr);
	
	while (true)
	{
		clisockfd = accept(serverSocket,
						   (struct sockaddr *) &cli_addr, 
						   &clilen);
		
		if (clisockfd < 0) 
			error("ERROR on accept");
		
		ptr = new struct PlayerStartParams;
		ptr->socketFd = clisockfd;
		ptr->roomManagerPointer = (void*)roomManager;
		pthread_create( &playerThread, NULL, createAndRunPlayer,
						(void*) ptr);
		pthread_detach(playerThread);
	}

	return 0;
}

void 
configureServerSocket(int& sockfd, struct sockaddr_in& addr)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	memset((char *) &addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(7003);

	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		error("ERROR binding");


	listen(sockfd, 128);
}

void * 
createAndRunPlayer(void * ptr)
{
	struct PlayerStartParams * params = (struct PlayerStartParams *)ptr;
	Player * player = new Player(params->socketFd);
	player->roomManager = params->roomManagerPointer;
	delete params;
	player->run();
	delete player;
}

void 
error(const char * msg)
{
	cerr << msg << endl;
	exit(1);
}

void
configureSignals()
{
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = sigpipeHandler;
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
		error("sigaction error");
}
