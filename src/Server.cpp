#include "Server.hpp"

#define END_OF_MESSAGE_STRING "\n"

Connection::Connection () :
	fd (0), addr (), isReadable (false), isWritable (false), lastReceivedBytes (), lastReceivedLine ()
{}

Connection::Connection (int fd, sockaddr_in addr) :
	fd (fd), addr (addr), isReadable (false), isWritable (false), lastReceivedBytes (), lastReceivedLine ()
{}

ssize_t Connection::receiveBytes ()
{
	char buffer[1024];

	ssize_t totalReceivedBytes = 0;
	while (true)
	{
		ssize_t bytesRead = ::recv (fd, buffer, sizeof (buffer), MSG_DONTWAIT);

		// If we can't read because the read would block, try again next time
		if (bytesRead < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
			break;

		if (bytesRead <= 0)	// @Todo: this might be an error if bytesRead < 0
			break;

		lastReceivedBytes.append (buffer, (size_t)bytesRead);
		size_t endOfLine = lastReceivedBytes.find (END_OF_MESSAGE_STRING);
		if (endOfLine != std::string::npos)
		{
			lastReceivedLine.assign (lastReceivedBytes, 0, endOfLine);
			lastReceivedBytes.erase (0, endOfLine + (sizeof (END_OF_MESSAGE_STRING) - 1));
		
			//std::cout << "received line: " << lastReceivedLine << std::endl;
		}

		totalReceivedBytes += bytesRead;
	}

	return totalReceivedBytes;
}

Server::Server (int maxConnections) :
	_socketFd (-1), _addr (), _maxConnections (maxConnections), _connections (), _isRunning (true)
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
	
	::listen (_socketFd, _maxConnections);

	_isRunning = true;

	return true;
}

void Server::close ()
{
	::close (_socketFd);
	_socketFd = -1;

	for (ConnectionIt it = _connections.begin (); it != _connections.end (); it++)
		it = disconnect (it);
}

Connection *Server::acceptIncomingConnection ()
{
	sockaddr_in addr;
	socklen_t len = sizeof (addr);

	int fd = ::accept (_socketFd, (sockaddr *)&addr, &len);
	if (fd < 0)
		return NULL;

	_connections.push_back (Connection (fd, addr));

	return &_connections.back ();
}

void Server::pollConnectionEvents (int timeout)
{
	pollfd *pollfds = new pollfd[_connections.size ()];

	int i = 0;
	for (std::list<Connection>::iterator it = _connections.begin (); it != _connections.end (); it++, i++)
	{
		pollfds[i].fd = it->fd;
		pollfds[i].events = POLLIN | POLLOUT;
		pollfds[i].revents = 0;
	}

	int res = poll (pollfds, _connections.size (), timeout);
	if (res < 0)
		return;

	i = 0;
	for (std::list<Connection>::iterator it = _connections.begin (); it != _connections.end (); it++, i++)
	{
		it->isReadable = (pollfds[i].revents & POLLIN) == POLLIN;
		it->isWritable = (pollfds[i].revents & POLLOUT) == POLLOUT;
	}

	delete[] pollfds;
}

void Server::receiveDataFromConnections ()
{
	for (std::list<Connection>::iterator it = _connections.begin (); it != _connections.end (); it++)
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
	for (ConnectionIt it = _connections.begin (); it != _connections.end (); it++)
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

Server::ConnectionIt Server::disconnect (ConnectionIt connection)
{
	if (connection->lastReceivedBytes.length () != 0)
	{
		std::cout << "Connection still has pending data when disconnecting." << std::endl;
		std::cout << "data was: " << connection->lastReceivedBytes << std::endl;
	}

	assert (connection->lastReceivedLine.length () == 0, "Connection still has unprocessed message.");

	std::cout << "A client has disconnected." << std::endl;

	::close (connection->fd);

	return _connections.erase (connection);
}

bool Server::isRunning () const { return _isRunning; }
int Server::getMaxConnections () const { return _maxConnections; }
uint16_t Server::getPort () const { return ntohs (_addr.sin_port); }
