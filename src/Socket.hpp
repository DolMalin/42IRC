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

struct ReadResult
{
	ssize_t	bytes_read;
	bool	valid;
	bool	complete;

	ReadResult ();
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
	Connection	_connections[MAX_CLIENTS];
	int			_connection_count;

	Socket ();

public:
	static Opt<Socket> makeServer(uint16_t port);
	static Opt<Socket> makeClient(uint16_t port, const char *ip);

	bool	acceptIncomingConnection ();
	void	sendDataToAllConnections (const void *data, size_t len);
	ReadResult	readData (int connection_index, void *buff, size_t buff_len);
	void	close();
	void	pollConnectionEvents (int timeout);

	int			fd () const;
	Connection	connection (int index) const;
	int			connection_count () const;
	Type		type () const;
};
