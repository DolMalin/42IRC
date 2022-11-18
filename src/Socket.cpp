#include "Socket.hpp"
#include <poll.h>

ReadResult::ReadResult () :
	bytes_read (0), valid (false), complete (false)
{}

Socket::Socket () :
	_fd (0), _addr (), _type (), _connections (), _connection_count (0)
{}

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

bool Socket::acceptIncomingConnection ()
{
	if (_connection_count >= MAX_CLIENTS)
		return false;

	sockaddr_in addr;
	socklen_t len = sizeof (addr);

	int confd = accept (_fd, (sockaddr *)&addr, &len);
	if (confd < 0)
		return false;

	Connection *conn = &_connections[_connection_count];
	conn->fd = confd;
	conn->addr = addr;
	conn->events = 0;
	_connection_count += 1;

	return true;
}

void Socket::sendDataToAllConnections (const void *data, size_t len)
{
	for (int i = 0; i < _connection_count; i += 1)
	{
		if (!(_connections[i].events & SEND_AVAILABLE))
			continue;

		send (_connections[i].fd, data, len, 0);
	}
}

ReadResult Socket::readData (int connection_index, void *buff, size_t len)
{
	if (len == 0)
		return ReadResult ();

	Connection conn = connection (connection_index);
	ReadResult result;
	result.bytes_read = recv (conn.fd, buff, len, MSG_DONTWAIT);
	result.valid = result.bytes_read >= 0 || errno == EAGAIN || errno == EWOULDBLOCK;
	result.complete = result.bytes_read >= 0;

	return result;
}

void	Socket::close()
{
	if (_fd > 0)
		::close(_fd);
	
	for (int i = 0; i < _connection_count; i += 1)
		::close(_connections[i].fd);
	
	_fd = 0;
	_connection_count = 0;
}

void	Socket::pollConnectionEvents (int timeout)
{
	// @Todo (stefan): Handle error events

	pollfd pollfd[MAX_CLIENTS];
	for (int i = 0; i < _connection_count; i += 1)
	{
		pollfd[i].fd = _connections[i].fd;
		pollfd[i].events = POLLIN | POLLOUT | POLLRDHUP;
		pollfd[i].revents = 0;
	}

	int res = poll (pollfd, _connection_count, timeout);
	if (res < 0)
	{
		std::cerr << "poll error" << std::endl;
		return;
	}

	for (int i = 0; i < _connection_count; i += 1)
	{
		_connections[i].events = 0;
		if ((pollfd[i].revents & POLLIN) == POLLIN)
			_connections[i].events |= READ_AVAILABLE;
		if ((pollfd[i].revents & POLLOUT) == POLLOUT)
			_connections[i].events |= SEND_AVAILABLE;
		if ((pollfd[i].revents & POLLRDHUP) == POLLRDHUP)
			_connections[i].events |= DISCONNECTED;
	}
}

int Socket::fd () const { return _fd; }

Connection Socket::connection (int index) const
{
	assert (index >= 0 && index < _connection_count, "Invalid connection index (was " << index << ", max is " << _connection_count << ")");

	return _connections[index];
}

int Socket::connection_count () const { return _connection_count; }

Socket::Type Socket::type () const { return _type; }
