#include "Server.hpp"
#include "Reply.hpp"
#include "../common.hpp"

void Server::join (User &u, const Message &msg)
{
	if (msg.argsCount () < 1)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");

	for (std::vector<std::string>::iterator it = channels.begin (); it != channels.end (); it++)
	{
		const std::string &name = *it;
		std::cout << name << std::endl;

		// @Todo: handle secret and private channels
		if (name[0] != '#')
			reply (u, Reply::errNoSuchChannel (name));

		Channel *chan = findChannelByName (name);

		// Create channel if it does not exist
		// @Todo: make this user the chanop on creation
		if (!chan)
			chan = addChannel (name, "Newly created channel");

		chan->addUser (&u);
		reply (u, Reply::topic (u.nickname, name, chan->topic));
	}
}
