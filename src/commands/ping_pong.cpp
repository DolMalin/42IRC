#include "Server.hpp"
#include "Reply.hpp"

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
