#include "Server.hpp"

Server::Server (int maxUsers) :
	_socketFd (-1), _addr (), _maxUsers (maxUsers), _users (), _isRunning (true)
{}

Server::Server (const Server &) {}
Server &Server::operator= (const Server &) { return *this; }

bool Server::init (uint16_t port)
{
	_socketFd = ::socket (AF_INET, SOCK_STREAM, 0);
	if (_socketFd < 0)
		return false;
	
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	_addr.sin_port = htons (port);

	if (::bind (_socketFd, (sockaddr *)&_addr, sizeof (_addr)) < 0)
		return false;

	if (::fcntl (_socketFd, F_SETFL, O_NONBLOCK) < 0)
		return false;
	
	::listen (_socketFd, _maxUsers);

	int opt_val = 1;
	::setsockopt (_socketFd, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof (opt_val));

	_isRunning = true;

	return true;
}

void Server::close ()
{
	::close (_socketFd);
	_socketFd = -1;

	for (User::UserIt it = _users.begin (); it != _users.end (); it++)
		it = disconnect (it);
}

User *Server::acceptIncomingUser ()
{
	sockaddr_in addr;
	socklen_t len = sizeof (addr);

	int fd = ::accept (_socketFd, (sockaddr *)&addr, &len);
	if (fd < 0)
		return NULL;

	_users.push_back (User (fd, addr));
	User *conn = &_users.back ();

	std::cout << conn->getAddressAsString () << " has connected to the server." << std::endl;

	return conn;
}

void Server::pollUserEvents (int timeout)
{
	pollfd *pollfds = new pollfd[_users.size ()];

	int i = 0;
	for (std::list<User>::iterator it = _users.begin (); it != _users.end (); it++, i++)
	{
		pollfds[i].fd = it->fd;
		pollfds[i].events = POLLIN | POLLOUT;
		pollfds[i].revents = 0;
	}

	int res = poll (pollfds, _users.size (), timeout);
	if (res < 0)
		return;

	i = 0;
	for (std::list<User>::iterator it = _users.begin (); it != _users.end (); it++, i++)
	{
		it->isReadable = (pollfds[i].revents & POLLIN) == POLLIN;
		it->isWritable = (pollfds[i].revents & POLLOUT) == POLLOUT;
	}

	delete[] pollfds;
}

void Server::receiveDataFromUsers ()
{
	for (std::list<User>::iterator it = _users.begin (); it != _users.end (); it++)
	{
		if (it->isReadable)
		{
			ssize_t bytesRead = it->receiveBytes ();
			if (bytesRead == 0)
				it = disconnect (it);
		}
	}
}

// For now this just clears the line we received, but later on we'll parse the
// line and execute commands
void Server::processReceivedMessages ()
{
	for (User::UserIt it = _users.begin (); it != _users.end (); it++)
	{
		if (it->lastReceivedLine.empty ())
			continue;

		Opt<Message> msgOpt = Message::parseRequest (it->lastReceivedLine);
		if (!msgOpt.ok)
		{
			std::cerr << "Invalid message (" << it->lastReceivedLine << ")" << std::endl;
			it->lastReceivedLine.clear ();
			
			continue;
		}

		Message msg = msgOpt.val;
		it->lastReceivedLine.clear ();

		std::cout << msg.stringify () << std::endl;
	}
}

User::UserIt Server::disconnect (User::UserIt user)
{
	if (user->lastReceivedBytes.length () != 0)
	{
		std::cout << "User " << user->getAddressAsString () << " still has pending data when disconnecting." << std::endl;
		std::cout << "data was: " << user->lastReceivedBytes << std::endl;
	}

	assert (user->lastReceivedLine.length () == 0, "User " << user->getAddressAsString () << " still has unprocessed message.");

	std::cout << "Client " << user->getAddressAsString () << " has disconnected." << std::endl;

	::close (user->fd);

	return _users.erase (user);
}

bool Server::isRunning () const { return _isRunning; }
int Server::getMaxConnections () const { return _maxUsers; }
uint16_t Server::getPort () const { return ntohs (_addr.sin_port); }
