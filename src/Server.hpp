#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <list>
#include <map>

#include "common.hpp"
#include "Message.hpp"
#include <list>

struct User {};	// Dummy

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
	std::string getAddressAsString () const;
};

class Server
{
public:
	typedef void (Server::* CommandProc) (User &, const Message &);

private:
	int _socketFd;
	sockaddr_in _addr;
	int _maxConnections;
	std::list<Connection> _connections;
	bool _isRunning;

	std::map<std::string, CommandProc> _commands;

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
	void executeCommand (User &user, const Message &msg);

	bool isRunning () const;
	int getMaxConnections () const;
	uint16_t getPort () const;

	// Commands

	void user (User &u, const Message &msg);

private:
	Server (const Server &);
	Server &operator= (const Server &);
};
