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

# define PORT 1222
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
		if (server.acceptIncomingConnection ())
			std::cout << "Somebody has connected to the server" << std::endl;

		server.pollConnectionEvents (POLL_TIMEOUT * 1000);

		for (int i = 0; i < server.connection_count (); i += 1)
		{
			Connection conn = server.connection (i);

			if (conn.events & READ_AVAILABLE)
				std::cout << "We've got data to read at connection " << i << "." << std::endl;
			if (conn.events & SEND_AVAILABLE)
				std::cout << "We can send data to connection " << i << "." << std::endl;
			if (conn.events & DISCONNECTED)
				std::cout << "Client " << i << " disconnected." << std::endl;
		}

		sleep (5);
	}

	server.close();
}
