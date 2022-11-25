#include "Server.hpp"
#include "Reply.hpp"

void Server::names(User &u, const Message &msg)
{
	if (msg.argsCount() == 0)
	{
		std::string result;
		for (ChannelIt it = _channels.begin(); it != _channels.end(); it++)
		{
			reply(u, Reply::namReply(*it));
			if (it != _channels.begin())
				result = result.append(",").append((*it).name);
			else
				result = result.append((*it).name);
		}
		reply(u, Reply::endOfNames(u.nickname, result));
		return ;
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");
	for (std::vector<std::string>::iterator it = channels.begin (); it != channels.end (); it++)
	{
		Channel *c = Server::findChannelByName(*it);
		if (!c)
			reply(u, Reply::errNoSuchChannel(*it));
		else
		{
			std::string result;

			reply(u, Reply::namReply(*c));
			if (it != channels.begin())
				result = result.append(",").append((*c).name);
			else
				result = result.append((*c).name);
			reply(u, Reply::endOfNames(u.nickname, result));
		}
	}
}
