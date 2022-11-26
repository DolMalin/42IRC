#include "Server.hpp"
#include "Channel.hpp"
#include "Reply.hpp"

void Server::mode(User &u, const Message &msg)
{
	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}

	if (!findChannelByName(msg.arg(0)))
	{
		reply(u, Reply::errNoSuchChannel(msg.arg(0)));
		return ;
	}
	
	std::string modesCharset = "imnqpstk";
	Channel *c = findChannelByName(msg.arg(0));
	for (size_t i = 1; i < msg.argsCount(); i++)
	{
		if (msg.arg(i).at(0) != '+' && msg.arg(i).at(0) != '-')
			return ;
		char lastOperator = msg.arg(i).at(0);
		for (size_t j = 1; j < msg.arg(i).length(); j++)
		{
			if (msg.arg(i).at(j) == '+' || msg.arg(i).at(j) == '-')
			{
				lastOperator = msg.arg(i).at(j);
				continue;
			}
			if (modesCharset.find(msg.arg(i).at(j)) == std::string::npos)
			{
				reply(u, Reply::errUnknownMode(c->name, std::string(&msg.arg(i).at(j), 1)));
				continue ;
			}
			if (lastOperator == '+')
				c->setMode(msg.arg(i).at(j), true);
			else
				c->setMode(msg.arg(i).at(j), false);
		}
	}
	forwardToChannel(u, *c, Reply::channelModeIs(c->name, c->modeToString()));
	
}