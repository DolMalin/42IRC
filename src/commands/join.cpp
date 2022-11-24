#include "Server.hpp"
#include "Reply.hpp"
#include <vector>

static std::vector<std::string> splitString (const std::string &str, const std::string &delim)
{
	std::vector<std::string> result;

	size_t last_pos = 0;
	while (true)
	{
		size_t pos = str.find (delim, last_pos);
		if (pos == std::string::npos)
			break;

		if (pos - last_pos > 0)
			result.push_back (str.substr (last_pos, pos - last_pos));

		last_pos = pos + 1;
	}

	if (last_pos < str.length ())
		result.push_back (str.substr (last_pos, str.length () - last_pos));

	return result;
}

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
