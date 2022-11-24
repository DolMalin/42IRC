#include "Server.hpp"
#include <sstream>

# define POLL_TIMEOUT 5

int main (int argc, char **args)
{
	if (argc < 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	uint16_t port;

	std::istringstream iss (args[1]);
	iss >> port;
	if (iss.fail() || iss.tellg() != -1 || size_t(log10(port) + 1) != strlen(args[1]))
	{
		std::cout << "Port (" << args[1] << ") is invalid." << std::endl;
		iss.clear();
		return 1;
	}

	Server server (SOMAXCONN);
	if (!server.init (port, args[2]))
	{
		std::cerr << "Could not initialize server. Port was " << port << "." << std::endl;
		return 1;
	}

	std::cout << "Server is listening on port: " << server.getPort () << std::endl;

	while (server.isRunning ())
	{
		server.acceptIncomingUser ();
		server.pollUserEvents (POLL_TIMEOUT);
		server.receiveDataFromUsers ();
		server.processReceivedMessages ();
		server.removeDisconnectedUsers ();
		server.testPings();
	}

	server.close();
}
