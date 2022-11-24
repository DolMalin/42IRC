#include "Server.hpp"
#include "Reply.hpp"

#include <vector>

Server::Server(int maxUsers) : _socketFd(-1), _addr(), _maxUsers(maxUsers), _isRunning(true), _users(), _channels (), _commands()
{
	_commands["NICK"] = &Server::nick;
	_commands["USER"] = &Server::user;
	_commands["QUIT"] = &Server::quit;
	_commands["CAP"] = NULL;
	_commands["JOIN"] = &Server::join;
	_commands["PING"] = &Server::ping;
	_commands["PONG"] = &Server::pong;
	// _commands["ERROR"] = &Server::error;
}

Server::Server(const Server &) {}
Server &Server::operator=(const Server &) { return *this; }

bool Server::init(uint16_t port)
{
	_socketFd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd < 0)
		return false;

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);

	if (::bind(_socketFd, (sockaddr *)&_addr, sizeof(_addr)) < 0)
		return false;

	if (::fcntl(_socketFd, F_SETFL, O_NONBLOCK) < 0)
		return false;

	::listen(_socketFd, _maxUsers);

	int opt_val = 1;
	::setsockopt(_socketFd, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(opt_val));

	_isRunning = true;

	return true;
}

void Server::close()
{
	::close(_socketFd);
	_socketFd = -1;

	for (UserIt it = _users.begin(); it != _users.end(); it++)
	{
		disconnect (*it);
		it = _users.erase (it);
	}
}

User *Server::acceptIncomingUser()
{
	sockaddr_in addr;
	socklen_t len = sizeof(addr);

	int fd = ::accept(_socketFd, (sockaddr *)&addr, &len);
	if (fd < 0)
		return NULL;

	_users.push_back(User(fd, addr));
	User *conn = &_users.back();

	std::cout << conn->getAddressAsString() << " has connected to the server." << std::endl;

	return conn;
}

void Server::pollUserEvents(int timeout)
{
	pollfd *pollfds = new pollfd[_users.size()];

	int i = 0;
	for (UserIt it = _users.begin(); it != _users.end(); it++, i++)
	{
		pollfds[i].fd = it->fd;
		pollfds[i].events = POLLIN | POLLOUT;
		pollfds[i].revents = 0;
	}

	int res = poll(pollfds, _users.size(), timeout);
	if (res < 0)
		return;

	i = 0;
	for (UserIt it = _users.begin(); it != _users.end(); it++, i++)
	{
		it->isReadable = (pollfds[i].revents & POLLIN) == POLLIN;
		it->isWritable = (pollfds[i].revents & POLLOUT) == POLLOUT;
	}

	delete[] pollfds;
}

void Server::receiveDataFromUsers()
{
	for (UserIt it = _users.begin(); it != _users.end(); it++)
	{
		if (it->isDisconnected)
			continue;

		if (it->isReadable)
		{
			ssize_t bytesRead = it->receiveBytes();
			if (bytesRead == 0)
				disconnect (*it);
		}
	}
}

void Server::processReceivedMessages()
{
	std::string line;
	for (UserIt it = _users.begin(); it != _users.end(); it++)
	{
		if (it->isDisconnected)
			continue;

		while (true)
		{
			line.clear();

			size_t endOfLine = it->lastReceivedBytes.find("\r\n");
			if (endOfLine != std::string::npos)
			{
				line.assign(it->lastReceivedBytes, 0, endOfLine);
				it->lastReceivedBytes.erase(0, endOfLine + 2);
			}
			else
			{
				endOfLine = it->lastReceivedBytes.find("\n");
				if (endOfLine != std::string::npos)
				{
					line.assign(it->lastReceivedBytes, 0, endOfLine);
					it->lastReceivedBytes.erase(0, endOfLine + 1);
				}
			}

			if (line.empty())
				break;

			Opt<Message> msgOpt = Message::parseRequest(line);
			if (!msgOpt.ok)
			{
				std::cerr << "Invalid message (" << line << ")" << std::endl;

				continue;
			}

			Message msg = msgOpt.val;
			std::cout << "Received: " << msg.stringify() << std::endl;

			executeCommand(*it, msg);
		}
	}
}

void Server::disconnect(User &user)
{
	assert (!user.isDisconnected, "User has already been disconnected");

	if (user.lastReceivedBytes.length() != 0)
	{
		std::cout << "User " << user.getAddressAsString() << " still has pending data when disconnecting." << std::endl;
		std::cout << "data was: " << user.lastReceivedBytes << std::endl;
	}

	std::cout << "Client " << user.getAddressAsString() << " has disconnected." << std::endl;

	user.flush();
	::close(user.fd);
	user.fd = -1;
	user.isDisconnected = true;
}

void Server::executeCommand(User &user, const Message &msg)
{
	std::map<std::string, CommandProc>::iterator it = _commands.find(msg.command());
	if (it == _commands.end())
	{
		reply(user, Reply::errUnknownCommand(msg.command()));
		return;
	}

	CommandProc proc = it->second;
	if (proc)
		(this->*proc)(user, msg);
}

void Server::reply(User &user, const Message &msg)
{
	std::string str = msg.stringify();

	std::cout << "Replying: " << str << std::endl;
	user.sendBytes(str + "\r\n");
}

void Server::removeDisconnectedUsers ()
{
	for (ChannelIt it = _channels.begin (); it != _channels.end (); it++)
	{
		it->removeDisconnectedUsers ();
	}

	for (UserIt it = _users.begin (); it != _users.end (); it++)
	{
		if (it->isDisconnected)
		{
			it = _users.erase (it);
		}
	}
}

Channel *Server::addChannel (const std::string &name, const std::string &topic)
{
	assert (findChannelByName (name) == NULL, "Channel already exists.");

	_channels.push_back (Channel (name, topic));

	return &_channels.back ();
}

Channel *Server::findChannelByName (const std::string &name)
{
	for (ChannelIt it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->name == name)
			return &(*it);
	}

	return NULL;
}

User *Server::findUserByNickname(const std::string &nick)
{
	for (UserIt it = _users.begin(); it != _users.end(); it++)
	{
		if (it->nickname == nick)
			return &(*it);
	}

	return NULL;
}

static bool isValidNickname(const std::string &nick)
{
	for (size_t i = 0; i < nick.length(); i += 1)
	{
		if (!isalpha(nick[i]) && !isdigit(nick[i]) && nick[i] != '_')
			return false;
	}

	return true;
}

void	Server::testPings()
{
	time_t now = std::time(NULL);

	for (UserIt it = _users.begin(); it != _users.end(); it++)
	{
		if (now - it->lastPing > SENDPING_T)
		{
			reply(*it, Reply::ping("42IRC"));
			it->updateLastPing();
		}

		if (it->lastPong < it->lastPing && now - it->lastPing > PONG_DELAY)
		{
			//@TODO: Reply kill
			disconnect(*it);
		}
	}

}

/* ================ ~ COMMANDS ~ ===============*/
void Server::nick(User &u, const Message &msg)
{
	// @Todo: reply ERR_NICKCOLLISION
	// @Todo: reply ERR_UNAVAILRESOURCE
	// @Todo: reply ERR_RESTRICTED

	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNoNicknameGiven());
		return;
	}

	const std::string &nick = msg.arg(0);
	if (!isValidNickname(nick))
	{
		reply(u, Reply::errErroneousNickname(nick));
		return;
	}

	if (findUserByNickname(nick))
	{
		reply(u, Reply::errNicknameInUse(nick));
		return;
	}

	u.nickname = nick;
}

void Server::user(User &u, const Message &msg)
{
	if (msg.argsCount() < 4)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}

	if (u.isRegistered)
	{
		reply(u, Reply::errAlreadyRegistered());
		return;
	}

	// @TODO : check nick is not empty
	u.username = msg.arg(3);
	// @Todo: handle mode
	u.isRegistered = true;

	reply(u, Reply::welcome(u.nickname, u.username, u.getAddressAsString()));
}

void Server::quit(User &u, const Message &msg)
{
	if (msg.hasSuffix())
		reply(u, Reply::error(msg.suffix()));
	else
		reply(u, Reply::error(""));
	disconnect (u);
}

void Server::ping(User &u, const Message &msg)
{
	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNoOrigin());
		return ;
	}

	if (msg.arg(0).empty())
	{
		reply(u, Reply::errNoOrigin());
		return ;
	}

	reply(u, Reply::pong("42IRC"));
}

void Server::pong(User &u, const Message &msg)
{
	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNoOrigin());
		return ;
	}

	if (msg.arg(0).empty())
	{
		reply(u, Reply::errNoOrigin());
		return ;
	}
	u.updateLastPong();
}

static std::vector<std::string> splitString (const std::string &str, const std::string &delim)
{
	std::vector<std::string> result;

	size_t last_pos = 0;
	while (true)
	{
		size_t pos = str.find (delim, last_pos);
		if (pos == std::string::npos)
			break;

		if (pos - last_pos > 0)
			result.push_back (str.substr (last_pos, pos - last_pos));

		last_pos = pos + 1;
	}

	if (last_pos < str.length ())
		result.push_back (str.substr (last_pos, str.length () - last_pos));

	return result;
}

void Server::join (User &u, const Message &msg)
{
	if (msg.argsCount () < 1)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");

	for (std::vector<std::string>::iterator it = channels.begin (); it != channels.end (); it++)
	{
		const std::string &name = *it;
		std::cout << name << std::endl;

		// @Todo: handle secret and private channels
		if (name[0] != '#')
			reply (u, Reply::errNoSuchChannel (name));

		Channel *chan = findChannelByName (name);

		// Create channel if it does not exist
		// @Todo: make this user the chanop on creation
		if (!chan)
			chan = addChannel (name, "Newly created channel");

		chan->addUser (&u);
		reply (u, Reply::topic (u.nickname, name, chan->topic));
	}
}

bool Server::isRunning() const { return _isRunning; }
int Server::getMaxConnections() const { return _maxUsers; }
uint16_t Server::getPort() const { return ntohs(_addr.sin_port); }
