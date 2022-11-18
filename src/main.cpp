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
	std::cout << "Server is listening on port: " << PORT << std::endl;

	while (1)
	{
		if (server.acceptIncomingConnections ())
			std::cout << "Somebody connected to the server" << std::endl;
		server.sendData ("Hello Sailor\n", sizeof ("Hello Sailor\n"));
	}

	server.close();
}
