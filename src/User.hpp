#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <list>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.hpp"
#include "Message.hpp"
#include "User.hpp"

class User
{
public:
	typedef typename std::list<User>::iterator UserIt;

	int fd;
	sockaddr_in addr;

	bool	isReadable;
	bool	isWritable;
	bool	isRegistered;
	
	std::string bytesToSend;
	std::string lastReceivedBytes;
	std::string lastReceivedLine;
	std::string	nickname;
	std::string	username;

public:
	User ();
	User (int fd, sockaddr_in addr);

	ssize_t receiveBytes ();
	bool sendBytes (const void *buff, size_t len);
	bool sendBytes (const std::string &str);
	bool flush ();

	std::string getAddressAsString () const;

};
