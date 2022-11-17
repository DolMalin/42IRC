#include "Socket.hpp"

Socket Socket::makeServer(uint16_t port)
{
	Socket server;

	server._fd = socket(AF_INET, SOCK_STREAM, 0);
	// @TODO: Check socket opening

	memset(&server._addr, 0, sizeof(server._addr));
	server._addr.sin_family = AF_INET;
	server._addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server._addr.sin_port = htons(port);

	bind(server._fd, (sockaddr *)&server._addr, sizeof(server._addr));
	// @TODO: Check bind

	listen(server._fd, MAX_CLIENTS);
	
	return (server);
}

Socket Socket::makeClient(uint16_t port, const char *ip)
{
	Socket	client;

	client._fd = socket(AF_INET, SOCK_STREAM, 0);
	// @TODO: Check socket opening


    memset (&client._addr, 0, sizeof (client._addr));

    client._addr.sin_family = AF_INET;
    client._addr.sin_port = htons(port);

    inet_pton(AF_INET, ip, &client._addr.sin_addr);
	// @TODO: Check inet_pton


    connect(client._fd, (struct sockaddr *)&client._addr, sizeof(client._addr));
	// @TODO: Check connect

	// @TODO: Remake properly logs
    std::cout << "Successfully connected to server " << std::string (ip) << std::endl;

    return (client);
}