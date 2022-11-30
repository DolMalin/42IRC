#include "Server.hpp"
#include "Channel.hpp"
#include "Reply.hpp"

void Server::mode(User &u, const Message &msg)
{
	Channel *c = findChannelByName(msg.arg(0));
	Channel::UserIt it = c->findUser(&u);
	std::string channelModesCharset = "imnqpstk";
	std::string userModesCharset = "oktl";

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

	if (it == c->joinedUsers.end())
	{
		reply(u, Reply::errUserNotInChannel(u.nickname, c->name));
		return ;
	}

	if (!(*it).flags.isOperator)
	{
		reply(u, Reply::errChanOpIsNeeded(c->name));
		return ;
	}

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
			if ((userModesCharset + channelModesCharset).find(msg.arg(i).at(j)) == std::string::npos)
			{
				reply(u, Reply::errUnknownMode(c->name, std::string(&msg.arg(i).at(j), 1)));
				continue ;
			}

			if (channelModesCharset.find(msg.arg(i).at(j)) != std::string::npos)
				c->setMode(msg.arg(i).at(j), lastOperator);
			else
			{
				if (i + 1 >= msg.argsCount())
					reply(u, Reply::errNeedMoreParams(msg.command()));
				else if (!findUserByNickname(msg.arg(i + 1)))
					reply(u, Reply::errUserNotInChannel(c->name, msg.arg(i + 1)));
				else
					c->setMode(msg.arg(i).at(j), lastOperator, msg.arg(i + 1));
			}
		}
	}
	forwardToChannel(u, *c, Reply::channelModeIs(c->name, c->modeToString()));
}
// if (msg.arg(i).at(j) == 'k' && !c->key.empty())
// 	reply(u, Reply::errKeyset(c->name));