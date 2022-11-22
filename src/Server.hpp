#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <list>
#include <map>

#include "common.hpp"
#include "Message.hpp"
#include "User.hpp"

class Server
{
public:
	typedef void (Server::* CommandProc) (User &, const Message &);

private:
	int _socketFd;
	sockaddr_in _addr;
	int _maxUsers;
	std::list<User> _users;
	bool _isRunning;

	std::map<std::string, CommandProc> _commands;

public:
	explicit Server (int maxUsers);

	bool init (uint16_t port);
	void close ();

	User *acceptIncomingUser ();
	void pollUserEvents (int timeout);
	void receiveDataFromUsers ();
	void processReceivedMessages ();
	User::UserIt disconnect (User::UserIt user);
	void executeCommand (User &user, const Message &msg);
	void reply (User &user, const Message &msg);

	User *findUserByNickname (const std::string &nick);

	bool isRunning () const;
	int getMaxConnections () const;
	uint16_t getPort () const;

	// Commands

	void nick (User &u, const Message &msg);
	void user (User &u, const Message &msg);

private:
	Server (const Server &);
	Server &operator= (const Server &);
};
