#include "Server.hpp"
#include "Reply.hpp"

void Server::list(User &u, const Message &msg)
{
	if (msg.argsCount() < 1)
	{
		for (ChannelIt it = _channels.begin(); it != _channels.end(); it++)
			reply(u, Reply::list((*it).name, (*it).topic));
		reply(u, Reply::listEnd());
		return ;
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");
	for (std::vector<std::string>::iterator it = channels.begin (); it != channels.end (); it++)
	{
		Channel *c = Server::findChannelByName(*it);
		if (!c)
			reply(u, Reply::errNoSuchChannel(*it));
		else
			reply(u, Reply::list(c->name, c->topic));
	}
	reply(u, Reply::listEnd());
}
