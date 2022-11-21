#include "Server.hpp"

# define PORT 1222
# define POLL_TIMEOUT 5

int main ()
{
	Server server (10);
	if (!server.init (PORT))
	{
		std::cerr << "Could not initialize server. Port was " << PORT << "." << std::endl;
		return 1;
	}

	std::cout << "Server is listening on port: " << server.getPort () << std::endl;

	while (server.isRunning ())
	{
		if (server.acceptIncomingConnection ())
			std::cout << "Somebody has connected to the server" << std::endl;

		server.pollConnectionEvents (POLL_TIMEOUT);
		server.receiveDataFromConnections ();
		server.processReceivedMessages ();
	}

	server.close();
}
