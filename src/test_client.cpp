#include "Socket.hpp"

#define PORT 4242

int main (int argc, char **args)
{
	if (argc < 2)
	{
		std::cerr << "Usage: ./test_client server_ip" << std::endl;
		return 1;
	}

	const char *server_ip = args[1];
	Opt<Socket> res = Socket::makeClient (PORT, server_ip);
	if (!res.ok)
	{
		std::cerr << "Could not connect to server " << server_ip << ":" << PORT << std::endl;
		return 1;
	}

	std::cout << "Successfully connected to server " << server_ip << ":" << PORT << std::endl;

	return 0;
}
