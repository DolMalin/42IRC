#include "Socket.hpp"

Opt<Socket> Socket::makeServer(uint16_t port)
{
	Socket server;

	server._type = SERVER;
	server._fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server._fd < 0)
		return make_opt (server, false);

	memset(&server._addr, 0, sizeof(server._addr));
	server._addr.sin_family = AF_INET;
	server._addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server._addr.sin_port = htons(port);

	if (bind(server._fd, (sockaddr *)&server._addr, sizeof(server._addr)) < 0)
		return make_opt (server, false);

	if (fcntl (server._fd, F_SETFL, O_NONBLOCK) < 0)
		return make_opt (server, false);

	listen(server._fd, MAX_CLIENTS);

	return make_opt(server, true);
}

Opt<Socket> Socket::makeClient(uint16_t port, const char *ip)
{
	Socket	client;

	client._type = CLIENT;
	client._fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client._fd < 0)
		return make_opt(client, false);


	memset (&client._addr, 0, sizeof (client._addr));

	client._addr.sin_family = AF_INET;
	client._addr.sin_port = htons(port);

	int pton_res = inet_pton(AF_INET, ip, &client._addr.sin_addr);
	if (pton_res < 0)
		return make_opt (client, false);

	int connect_res = connect(client._fd, (struct sockaddr *)&client._addr, sizeof(client._addr));
	if (connect_res < 0)
		return make_opt (client, false);

	return make_opt (client, true);
}

Socket::Type Socket::type () const { return _type; }

bool Socket::acceptIncomingConnections ()
{
	int confd = accept (_fd, NULL, NULL);
	if (confd < 0)
		return false;

	_confd = confd;

	return true;
}

bool Socket::sendData (const void *data, size_t len)
{
	ssize_t res = send (_confd, data, len, 0);

	return (res >= 0);
}
