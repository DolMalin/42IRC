#include "Server.hpp"
#include "Channel.hpp"
#include "Reply.hpp"

void Server::mode(User &u, const Message &msg)
{
	std::string channelModes = "imnqpst";
	std::string channelModesArgs = "k";
	std::string userModesArgs = "ov";

	Channel *chan = findChannelByName(msg.arg(0));

	if (!chan)
	{
		reply(u, Reply::errNoSuchChannel(msg.arg(0)));
		return ;
	}

	Channel::UserIt it = chan->findUser(&u);

	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}

	if (it == chan->joinedUsers.end())
	{
		reply(u, Reply::errUserNotInChannel(u.nickname, chan->name));
		return ;
	}

	if (!(*it).flags.isOperator)
	{
		reply(u, Reply::errChanOpIsNeeded(chan->name));
		return ;
	}

	for (size_t i = 1; i < msg.argsCount(); i++)
	{
		if (msg.arg(i).at(0) != '+' && msg.arg(i).at(0) != '-')
			return ;

		char lastOperator = msg.arg(i).at(0);
		for (size_t j = 1; j < msg.arg(i).length(); j++)
		{
			const char c = msg.arg(i).at(j);

			if (c == '+' || c == '-')
				lastOperator = c;

			if ((userModesArgs + channelModes + channelModesArgs).find(c) == std::string::npos)
				reply(u, Reply::errUnknownMode(chan->name, std::string(&c, 1)));
			else if (channelModes.find(c) != std::string::npos || (c == 'k' && lastOperator == '-'))
				chan->setMode(c, lastOperator);
			else
			{
				if (i + 1 >= msg.argsCount())
				{
					reply(u, Reply::errNeedMoreParams(msg.command()));
					return ;
				}

				if ((userModesArgs + channelModes + channelModesArgs).find(c) == std::string::npos)
				{
					reply(u, Reply::errUnknownMode(chan->name, std::string(&c, 1)));
					return ;
				}

				if (c == 'k' && lastOperator == '+' && !chan->key.empty())
					reply(u, Reply::errKeyset(chan->name));
				else if (c == 'k')
					chan->setMode(c, lastOperator, msg.arg(i + 1));
				else
				{
					forwardToChannel(u, *chan, Reply::updateMode(chan->name, msg.arg(i + 1),  std::string(&lastOperator, 1) + c));
					chan->setMode(c, lastOperator, msg.arg(i + 1));
				}

			}
		}
	}
	forwardToChannel(u, *chan, Reply::channelModeIs(u.nickname, chan->name, chan->modeToString()));
}
