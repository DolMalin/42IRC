#pragma once 

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.hpp"

# define MAX_CLIENTS 5

#define SEND_AVAILABLE 0x01
#define READ_AVAILABLE 0x02
#define DISCONNECTED   0x04

struct Connection
{
	typedef int Events;

	int			fd;
	sockaddr_in	addr;
	Events		events;
};

class Socket
{
public:
	enum Type
	{
		SERVER,
		CLIENT
	};

private:
	int			_fd;
	sockaddr_in	_addr;
	Type		_type;
	Connection	_connection;

	Socket () {}

public:
	static Opt<Socket> makeServer(uint16_t port);
	static Opt<Socket> makeClient(uint16_t port, const char *ip);

	bool	acceptIncomingConnections ();
	bool	sendData (const void *data, size_t len);
	void	close();
	void	pollConnectionEvents (int timeout);

	int			fd () const;
	Connection	connection () const;
	Type		type () const;
};
