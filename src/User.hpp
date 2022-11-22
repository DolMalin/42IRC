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

#define END_OF_MESSAGE_STRING "\r\n"

struct User
{
public:
	typedef typename std::list<User>::iterator UserIt;

	int fd;
	sockaddr_in addr;
	bool isReadable;
	bool isWritable;
	std::string lastReceivedBytes;
	std::string lastReceivedLine;

	User ();
	User (int fd, sockaddr_in addr);

	ssize_t receiveBytes ();
	std::string getAddressAsString () const;
};