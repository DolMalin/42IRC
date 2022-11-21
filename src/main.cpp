#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <cstdio>

#include "Socket.hpp"
#include "Message.hpp"

# define PORT 5252
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

	Opt<Message> request = Message::parseRequest("JOIN Michel");
	if (!request.ok)
	{
		std::cerr << "Invalid message request" << std::endl;
		return 1;
	}

	Opt<Message> reply = Message::makeReply(":irc.example.com", 202, "michel", "Hello coquinou !");
	if (!reply.ok)
	{
		std::cerr << "Invalid message reply" << std::endl;
		return 1;
	}

	while (1)
	{
		if (server.acceptIncomingConnections ())
			std::cout << "Somebody connected to the server" << std::endl;
		server.sendData ("Hello Sailor\n", sizeof ("Hello Sailor\n"));
	}

	server.close();
}
