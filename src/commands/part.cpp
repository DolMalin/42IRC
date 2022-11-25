#include "Server.hpp"
#include "Reply.hpp"

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

		if (!chan->removeUser (&u))
		{
			reply (u, Reply::errNotOnChannel (channels[i]));
		}
	}
}
