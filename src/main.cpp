
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <cstdio>

#include "Socket.hpp"

# define PORT 4242
# define SERVER_IP "127.0.0.1"

int main ()
{
	Socket server = Socket::makeServer(PORT);
	Socket client = Socket::makeClient(PORT, SERVER_IP);

	(void)server;
	(void)client;

}
