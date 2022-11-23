#include "Server.hpp"
#include "Reply.hpp"

Server::Server(int maxUsers) : _socketFd(-1), _addr(), _maxUsers(maxUsers), _users(), _isRunning(true), _commands()
{
	_commands["NICK"] = &Server::nick;
	_commands["USER"] = &Server::user;
	_commands["QUIT"] = &Server::quit;
	_commands["CAP"] = &Server::cap;
	_commands["PING"] = &Server::ping;
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

	for (User::UserIt it = _users.begin(); it != _users.end(); it++)
		it = disconnect(it);
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
	for (std::list<User>::iterator it = _users.begin(); it != _users.end(); it++, i++)
	{
		pollfds[i].fd = it->fd;
		pollfds[i].events = POLLIN | POLLOUT;
		pollfds[i].revents = 0;
	}

	int res = poll(pollfds, _users.size(), timeout);
	if (res < 0)
		return;

	i = 0;
	for (std::list<User>::iterator it = _users.begin(); it != _users.end(); it++, i++)
	{
		it->isReadable = (pollfds[i].revents & POLLIN) == POLLIN;
		it->isWritable = (pollfds[i].revents & POLLOUT) == POLLOUT;
	}

	delete[] pollfds;
}

void Server::receiveDataFromUsers()
{
	for (std::list<User>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (it->isReadable)
		{
			ssize_t bytesRead = it->receiveBytes();
			if (bytesRead == 0)
				it = disconnect(it);
		}
	}
}

void Server::processReceivedMessages()
{
	std::string line;
	for (User::UserIt it = _users.begin(); it != _users.end(); it++)
	{
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

User::UserIt Server::disconnect(User::UserIt user)
{
	if (user->lastReceivedBytes.length() != 0)
	{
		std::cout << "User " << user->getAddressAsString() << " still has pending data when disconnecting." << std::endl;
		std::cout << "data was: " << user->lastReceivedBytes << std::endl;
	}

	std::cout << "Client " << user->getAddressAsString() << " has disconnected." << std::endl;

	user->flush();
	::close(user->fd);

	return _users.erase(user);
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
	(this->*proc)(user, msg);
}

void Server::reply(User &user, const Message &msg)
{
	std::string str = msg.stringify();

	std::cout << "Replying: " << str << std::endl;
	user.sendBytes(str + "\r\n");
}

User *Server::findUserByNickname(const std::string &nick)
{
	for (User::UserIt it = _users.begin(); it != _users.end(); it++)
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
	::close(u.fd);
	if (msg.hasSuffix())
		std::cout << msg.suffix() << std::endl;
	reply(u, Reply::error(""));
}

void Server::cap(User &, const Message &) {}

void Server::ping(User &u, const Message &msg)
{
	if(msg.argsCount() < 1)
	{
		reply(u, Reply::errNoOrigin());
		return ;
	}
	reply(u, Reply::pong("42IRC"));
}

bool Server::isRunning() const { return _isRunning; }
int Server::getMaxConnections() const { return _maxUsers; }
uint16_t Server::getPort() const { return ntohs(_addr.sin_port); }
