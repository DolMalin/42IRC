#include "Server.hpp"
#include "Reply.hpp"

void Server::list(User &u, const Message &msg)
{
	if (msg.argsCount() < 1)
	{
		for (ChannelIt it = _channels.begin(); it != _channels.end(); it++)
		{
			if (it->modes.isSecret || it->modes.isPrivate)
				continue;

			reply(u, Reply::list(u.nickname, (*it).name, itos((*it).joinedUsers.size()) ,(*it).topic));
		}
		reply(u, Reply::listEnd(u.nickname));
		return ;
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");
	for (std::vector<std::string>::iterator it = channels.begin (); it != channels.end (); it++)
	{
		Channel *c = Server::findChannelByName(*it);
		if (!c || c->modes.isSecret || c->modes.isPrivate)
			reply(u, Reply::errNoSuchChannel(*it));
		else
			reply(u, Reply::list(u.nickname, c->name, itos((*c).joinedUsers.size()), c->topic));
	}
	reply(u, Reply::listEnd(u.nickname));
}
