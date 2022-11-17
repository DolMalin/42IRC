#include "Socket.hpp"

Socket Socket::makeServer(uint16_t port)
{
	Socket server;

	server._type = SERVER;
	server._fd = socket(AF_INET, SOCK_STREAM, 0);
	// @TODO: Check socket opening

	memset(&server._addr, 0, sizeof(server._addr));
	server._addr.sin_family = AF_INET;
	server._addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server._addr.sin_port = htons(port);

	bind(server._fd, (sockaddr *)&server._addr, sizeof(server._addr));
	// @TODO: Check bind

	listen(server._fd, MAX_CLIENTS);

	std::cout << "Server is listening on PORT: " << port << std::endl;
	
	return (server);
}

Socket Socket::makeClient(uint16_t port, const char *ip)
{
	Socket	client;

	client._type = CLIENT;
	client._fd = socket(AF_INET, SOCK_STREAM, 0);
	// @TODO: Check socket opening


    memset (&client._addr, 0, sizeof (client._addr));

    client._addr.sin_family = AF_INET;
    client._addr.sin_port = htons(port);

    int pton_res = inet_pton(AF_INET, ip, &client._addr.sin_addr);
	assert (pton_res >= 0, "inet_pton failed.");
	// @TODO: Check inet_pton


    int connect_res = connect(client._fd, (struct sockaddr *)&client._addr, sizeof(client._addr));
	assert (connect_res >= 0, "Could not connect to server (connect returned " << connect_res << ").");
	// @TODO: Check connect

	// @TODO: Remake properly logs
    std::cout << "Successfully connected to server " << std::string (ip) << std::endl;

    return (client);
}

Socket::Type Socket::type () const { return _type; }
