#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <list>

#include "common.hpp"
#include "Message.hpp"
#include "User.hpp"

class Server
{
private:
	int _socketFd;
	sockaddr_in _addr;
	int _maxUsers;
	std::list<User> _users;
	bool _isRunning;

public:
	explicit Server (int maxUsers);

	bool init (uint16_t port);
	void close ();

	User *acceptIncomingUser ();
	void pollUserEvents (int timeout);
	void receiveDataFromUsers ();
	void processReceivedMessages ();
	User::UserIt disconnect (User::UserIt user);

	bool isRunning () const;
	int getMaxConnections () const;
	uint16_t getPort () const;

private:
	Server (const Server &);
	Server &operator= (const Server &);
};
