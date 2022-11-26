#include "Server.hpp"
#include "Reply.hpp"

void Server::topic (User &u, const Message &msg)
{
	if (msg.argsCount () < 1)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}
	
	const std::string &channelName = msg.arg (0);
	Channel *chan = findChannelByName (channelName);
	if (!chan)
	{
		reply (u, Reply::errNoSuchChannel (channelName));
		return;
	}

	Channel::UserIt it = chan->findUser (&u);
	if (it == chan->joinedUsers.end ())
	{
		reply (u, Reply::errNotOnChannel (channelName));
		return;
	}

	if (msg.argsCount () > 1)
	{
		const std::string &newTopic = msg.arg (1);
		if (!chan->modes.isTopicChangeable && !it->flags.isOperator)
		{
			reply (u, Reply::errChanOpIsNeeded (channelName));
			return;
		}

		chan->topic = newTopic;
	}

	if (chan->topic.empty ())
		reply (u, Reply::noTopic (u.nickname, channelName));
	else
		reply (u, Reply::topic (u.nickname, channelName, chan->topic));
}
