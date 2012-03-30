#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
	// Выясняем размер файла crossdomain.xml
	int size;
	ifstream cross;
	cross.open("crossdomain.xml", ios::in | ios::binary | ios::ate);
	size = cross.tellg();

	// Считываем его.
	char * crossdomain = new char[size + 1];
	cross.seekg(0, ios::beg);
	cross.read(crossdomain, size);
	crossdomain[size] = 0;

	// Создаем и настраиваем серверный сокет
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(843);
	bind(serverFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
	listen(serverFd, 128);

	// Переменные клиентского сокета
	int clientFd;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;

	char requestBuffer[24];
	requestBuffer[23] = 0;
	// cout << "Security server started." << endl;

	while (true)
	{
		clientFd = accept(serverFd, (struct sockaddr*) &clientAddr, &clientAddrLen);
		int r = read(clientFd, requestBuffer, 23);
		// cout << r << " bytes received: " << requestBuffer << endl;
		int w = write(clientFd, crossdomain, size + 1);
		// cout << w << " bytes writed";
		// cout << " to: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << endl;
		close(clientFd);
	}
	close(serverFd);
	return 0;
}
