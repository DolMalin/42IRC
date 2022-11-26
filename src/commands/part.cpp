#include "Server.hpp"
#include "Reply.hpp"

void Server::partUser (User &u, Channel &channel, User *target, const std::string &reason)
{
	// Usually, u and target are the same, but when kicking people they might differ

	if (channel.removeUser (target))
	{
		Message partMsg = Message ().setIsRequest (true).setCommand ("PART").pushArg (channel.name);
		if (!reason.empty ())
			partMsg.pushSuffix (reason);

		if (!channel.modes.isQuiet)
			forwardToChannel (*target, channel, partMsg);

		reply (*target, partMsg);	// target is not in channel anymore, so reply manually
	}
	else
	{
		if (&u == target)
			reply (u, Reply::errNotOnChannel (channel.name));
		else
			reply (u, Reply::errUserNotInChannel (target->nickname, channel.name));
	}
}

void Server::part (User &u, const Message &msg)
{
	if (msg.argsCount () < 1)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	std::string partMsg;
	if (msg.argsCount () > 1)
	{
		partMsg = msg.arg (1);
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");
	for (size_t i = 0; i < channels.size (); i++)
	{
		Channel *chan = findChannelByName (channels[i]);
		if (!chan)
		{
			reply (u, Reply::errNoSuchChannel (channels[i]));
			continue;
		}

		if (chan->removeUser (&u))
		{
			if (!chan->modes.isQuiet)
				forwardToChannel (u, *chan, msg);

			reply (u, msg);
		}
		else
		{
			reply (u, Reply::errNotOnChannel (channels[i]));
		}
	}
}
