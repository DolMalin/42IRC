#include "User.hpp"

User::User () :
	fd (0), addr (), isReadable (false), isWritable (false), isRegistered(false), lastReceivedBytes (), lastReceivedLine (), nickname (), username ()
{}


User::User (int fd, sockaddr_in addr) :
	fd (fd), addr (addr), isReadable (false), isWritable (false), isRegistered(false), lastReceivedBytes (), lastReceivedLine (), nickname (), username ()
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

bool User::sendBytes (const void *buff, size_t len)
{
	if (isWritable)
	{
		if (!bytesToSend.empty ())
		{
			bytesToSend.append ((const char *)buff, len);
			flush ();
		}
		else
		{
			::send (fd, buff, len, MSG_DONTWAIT);
			// @Todo: error checking
		}

		return true;
	}
	else
	{
		bytesToSend.append ((const char *)buff, len);

		return false;
	}
}

bool User::sendBytes (const std::string &str)
{
	return sendBytes (str.data (), str.length ());
}

bool User::flush ()
{
	if (isWritable && !bytesToSend.empty ())
	{
		::send (fd, bytesToSend.data (), bytesToSend.length (), MSG_DONTWAIT);
		// @Todo: error checking
		bytesToSend.clear ();

		return true;
	}

	return false;
}

std::string User::getAddressAsString () const
{
	char *s = inet_ntoa (addr.sin_addr);
	
	return std::string (s);
}
