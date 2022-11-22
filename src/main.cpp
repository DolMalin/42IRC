#include "Server.hpp"
#include <sstream>

# define POLL_TIMEOUT 5

int main (int argc, char **args)
{
	if (argc < 2)
	{
		std::cerr << "Usage: ./ircserv <port>" << std::endl;
		return 1;
	}

	uint16_t port;


	std::istringstream iss (args[1]);
	iss >> port;	// @Todo: error checking

	Server server (port);
	if (!server.init (port))
	{
		std::cerr << "Could not initialize server. Port was " << port << "." << std::endl;
		return 1;
	}

	std::cout << "Server is listening on port: " << server.getPort () << std::endl;

	while (server.isRunning ())
	{
		server.acceptIncomingConnection ();
		server.pollConnectionEvents (POLL_TIMEOUT);
		server.receiveDataFromConnections ();
		server.processReceivedMessages ();
	}

	server.close();
}
