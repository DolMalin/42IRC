#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <cstdio>
#include <poll.h>

#include "Socket.hpp"

# define PORT 1234
# define SERVER_IP ""
# define POLL_TIMEOUT 5

int main ()
{
	Opt<Socket> result = Socket::makeServer(PORT);
	if (!result.ok)
	{
		std::cerr << "Could not create server (port was " << PORT << ")" << std::endl;
		return 1;
	}

	Socket server = result.val;
	std::cout << "Server is listening on port: " << PORT << std::endl;

	while (1)
	{
		if (server.acceptIncomingConnections ())
			std::cout << "Somebody connected to the server" << std::endl;
	
		//server.sendData ("Hello Sailor\n", sizeof ("Hello Sailor\n"));

		server.pollConnectionEvents (POLL_TIMEOUT * 1000);

		if (server.connection ().events & READ_AVAILABLE)
			std::cout << "We've got data to read." << std::endl;
		if (server.connection ().events & SEND_AVAILABLE)
			std::cout << "We can send data." << std::endl;
		if (server.connection ().events & DISCONNECTED)
			std::cout << "Client disconnected." << std::endl;
	}

	server.close();
}
