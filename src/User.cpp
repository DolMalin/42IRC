#include "User.hpp"

User::User () :
	fd (0), addr (), isReadable (false), isWritable (false), lastReceivedBytes (), lastReceivedLine ()
{}


User::User (int fd, sockaddr_in addr) :
	fd (fd), addr (addr), isReadable (false), isWritable (false), lastReceivedBytes (), lastReceivedLine ()
{}


ssize_t User::receiveBytes ()
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

std::string User::getAddressAsString () const
{
	char *s = inet_ntoa (addr.sin_addr);
	
	return std::string (s);
}
