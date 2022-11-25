#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <list>
#include <map>

#include "common.hpp"
#include "Message.hpp"
#include "User.hpp"
#include "Channel.hpp"

#define SENDPING_T 60
#define PONG_DELAY 20

class Server
{
public:
	typedef void (Server::* CommandProc) (User &, const Message &);
	typedef std::list<User>::iterator UserIt;
	typedef std::list<Channel>::iterator ChannelIt;

private:
	int _socketFd;
	sockaddr_in _addr;
	int _maxUsers;
	bool _isRunning;

	std::list<User> _users;
	std::list<Channel> _channels;

	std::map<std::string, CommandProc> _commands;

	std::string	_password;

public:
	explicit Server (int maxUsers);

	bool init (uint16_t port, std::string password);
	void close ();

	User		*acceptIncomingUser ();
	void		pollUserEvents (int timeout);
	void		receiveDataFromUsers ();
	void		processReceivedMessages ();
	void		disconnect (User &user);
	void		executeCommand (User &user, const Message &msg);
	void		reply (User &user, const Message &msg);
	void		removeDisconnectedUsers ();
	void		testPings();
	std::string	getPassword();

	Channel *addChannel (const std::string &name, const std::string &topic);
	Channel *findChannelByName (const std::string &name);

	User *findUserByNickname (const std::string &nick);
	User *findUserByUsername (const std::string &name);

	bool isRunning () const;
	int getMaxConnections () const;
	uint16_t getPort () const;

	// Commands
	void nick (User &u, const Message &msg);
	void user (User &u, const Message &msg);
	void quit (User &u, const Message &msg);
	void join (User &u, const Message &msg);
	void ping (User &u, const Message &msg);
	void pong (User &u, const Message &msg);
	void kill (User &u, const Message &msg);
	void pass(User &u, const Message &msg);
	void list(User &u, const Message &msg);

private:
	Server (const Server &);
	Server &operator= (const Server &);
};
