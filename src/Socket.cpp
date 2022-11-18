#include "Socket.hpp"
#include <poll.h>

Opt<Socket> Socket::makeServer(uint16_t port)
{
	Socket server;

	server._type = SERVER;
	server._fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server._fd < 0)
	{
		std::cerr << "socket failed." << std::endl;
		return make_opt (server, false);
	}

	memset(&server._addr, 0, sizeof(server._addr));
	server._addr.sin_family = AF_INET;
	server._addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server._addr.sin_port = htons(port);

	if (bind(server._fd, (sockaddr *)&server._addr, sizeof(server._addr)) < 0)
	{
		std::cerr << "bind failed." << std::endl;
		return make_opt (server, false);
	}

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

bool Socket::acceptIncomingConnections ()
{
	// @Todo (stefan): handle multiple connections

	sockaddr_in addr;
	socklen_t len = sizeof (addr);

	int confd = accept (_fd, (sockaddr *)&addr, &len);
	if (confd < 0)
		return false;

	_connection.fd = confd;
	_connection.addr = addr;
	_connection.events = 0;

	return true;
}

bool Socket::sendData (const void *data, size_t len)
{
	ssize_t res = send (_connection.fd, data, len, 0);

	return (res >= 0);
}

void	Socket::close()
{
	if (_fd > 0)
		::close(_fd);
	if (_connection.fd > 0)
		::close(_connection.fd);
}

void	Socket::pollConnectionEvents (int timeout)
{
	// @Todo (stefan): Handle multiple connections
	// @Todo (stefan): Handle error events

	pollfd pollfd = {};
	pollfd.fd = _connection.fd;
	pollfd.events = POLLIN | POLLOUT | POLLRDHUP;
	
	int res = poll (&pollfd, 1, timeout);
	if (res < 0)
		return;

	_connection.events = 0;
	if (pollfd.revents & POLLIN)
		_connection.events |= READ_AVAILABLE;
	if (pollfd.revents & POLLOUT)
		_connection.events |= SEND_AVAILABLE;
	if (pollfd.revents & POLLRDHUP)
		_connection.events |= DISCONNECTED;
}

int Socket::fd () const { return _fd; }
Connection Socket::connection () const { return _connection; }
Socket::Type Socket::type () const { return _type; }
