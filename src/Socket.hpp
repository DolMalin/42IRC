#pragma once 

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <cstdio>
#include <stdint.h>

# define MAX_CLIENTS 5

class Socket {

	public:
		enum Type
		{
			SOCKET_SERVER,
			SOCKET_CLIENT
		};

	private:
		int			_fd;
		int			_confd;
		sockaddr_in	_addr;
		char		_buffer[512];

		Socket () {}

	public:
		static Socket makeServer(uint16_t port);
		static Socket makeClient(uint16_t port, const char *ip);
};
