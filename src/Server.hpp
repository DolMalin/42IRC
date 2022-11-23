#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <list>
#include <map>

#include "common.hpp"
#include "Message.hpp"
#include "User.hpp"

#define SENDPING_T 60
#define PONG_DELAY 20

class Server
{
public:
	typedef void (Server::* CommandProc) (User &, const Message &);
	typedef std::list<User>::iterator UserIt;

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
	void disconnect (User &user);
	void executeCommand (User &user, const Message &msg);
	void reply (User &user, const Message &msg);
	void removeDisconnectedUsers ();
	void testPings();

	User *findUserByNickname (const std::string &nick);

	bool isRunning () const;
	int getMaxConnections () const;
	uint16_t getPort () const;

	// Commands
	void nick (User &u, const Message &msg);
	void user (User &u, const Message &msg);
	void quit (User &u, const Message &msg);
	void cap  (User &, const Message &);
	void ping (User &u, const Message &msg);
	void pong (User &u, const Message &msg);

private:
	Server (const Server &);
	Server &operator= (const Server &);
};
