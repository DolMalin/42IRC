#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <list>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>

#include "common.hpp"
#include "Message.hpp"
#include "User.hpp"

class User
{
public:
	int fd;
	sockaddr_in addr;

	bool	isReadable;
	bool	isWritable;
	bool	isDisconnected;
	
	std::string bytesToSend;
	std::string lastReceivedBytes;
	std::string	nickname;
	std::string	username;
	std::string	realname;
	std::time_t	lastPing;
	std::time_t lastPong;
	std::string givenPassword;
	mode		mode;	



public:
	User ();
	User (int fd, sockaddr_in addr);

	ssize_t receiveBytes ();
	bool sendBytes (const void *buff, size_t len);
	bool sendBytes (const std::string &str);
	bool flush ();
	void updateLastPing();
	void updateLastPong();
	std::string getAddressAsString () const;
	std::string	prefix() const;
	bool isRegistered () const;
};
