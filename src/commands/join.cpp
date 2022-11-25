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
	// @Todo: handle invite only channels

	if (msg.argsCount () < 1)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");
	std::vector<std::string> keys;

	if (msg.argsCount () > 1)
		keys = splitString (msg.arg (1), ",");

	for (size_t i = 0; i < channels.size (); i++)
	{
		const std::string &name = channels[i];

		if (name[0] != '#')
		{
			reply (u, Reply::errNoSuchChannel (name));
			continue;
		}

		Channel *chan = findChannelByName (name);

		if (chan)
		{
			if (!chan->key.empty () && (i >= keys.size () || keys[i] != chan->key))
			{
				reply (u, Reply::errBadChannelKey (name));
				continue;
			}

			if (chan->joinedUsers.size () >= (size_t)chan->userLimit)
			{
				reply (u, Reply::errChannelIsFull (name));
				continue;
			}
		}

		// Create channel if it does not exist
		// @Todo: make this user the chanop on creation
		if (!chan)
		{
			chan = addChannel (name, "Newly created channel");
			if (i < keys.size ())
				chan->key = keys[i];
		}

		chan->addUser (&u);	// This won't add the user if it already exists
		reply (u, Reply::topic (u.nickname, name, chan->topic));
		reply (u, Reply::namReply (*chan));
	}

	reply (u, Reply::endOfNames (u.nickname, msg.arg (0)));
}
