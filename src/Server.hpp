#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "common.hpp"
#include <list>

struct Connection
{
	int fd;
	sockaddr_in addr;
	bool isReadable;
	bool isWritable;
	std::string lastReceivedBytes;
	std::string lastReceivedLine;

	Connection ();
	Connection (int fd, sockaddr_in addr);

	ssize_t receiveBytes ();
};

class Server
{
private:
	int _socketFd;
	sockaddr_in _addr;
	int _maxConnections;
	std::list<Connection> _connections;
	bool _isRunning;

public:
	typedef typename std::list<Connection>::iterator ConnectionIt;

public:
	explicit Server (int maxConnections);

	bool init (uint16_t port);
	void close ();

	Connection *acceptIncomingConnection ();
	void pollConnectionEvents (int timeout);
	void receiveDataFromConnections ();
	void processReceivedMessages ();
	ConnectionIt disconnect (ConnectionIt connection);

	bool isRunning () const;
	int getMaxConnections () const;

private:
	Server (const Server &);
	Server &operator= (const Server &);
};
