
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
# define SERVER_IP ""

int main ()
{
	Opt<Socket> result = Socket::makeServer(PORT);
	if (!result.ok)
	{
		std::cerr << "Could not create server (port was" << PORT << ")" << std::endl;
		return 1;
	}
	Socket server = result.val;
	
	result = Socket::makeClient(PORT, SERVER_IP);
	if (!result.ok)
	{
		std::cerr << "Could not create client (port was " << PORT << ", ip was " << SERVER_IP << ")" << std::endl;
		return 1;
	}

	Socket client = result.val;

	(void)server;
	// (void)client;
}
