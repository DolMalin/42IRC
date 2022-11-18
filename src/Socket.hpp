#pragma once 

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.hpp"

# define MAX_CLIENTS 5

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
	int			_confd;

	Socket () {}

public:
	static Opt<Socket> makeServer(uint16_t port);
	static Opt<Socket> makeClient(uint16_t port, const char *ip);

	bool	acceptIncomingConnections ();
	bool	sendData (const void *data, size_t len);
	void	close();

	Type	type () const;
};
