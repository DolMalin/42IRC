#include "Server.hpp"
#include "Reply.hpp"
#include "../common.hpp"

static bool isValidChannelName (const std::string &name)
{
	if (name.empty ())
		return false;
	if (name.length () > 50)
		return false;
	if (name[0] != '#')
		return false;
	
	for (size_t i = 0; i < name.length (); i++)
	{
		if (::isspace (name[i]) || name[i] == ',' || ::iscntrl (name[i]))
			return false;
	}

	return true;
}

void Server::join (User &u, const Message &msg)
{
	if (msg.argsCount () < 1)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	if (msg.arg (0) == "0")
	{
		for (ChannelIt it = _channels.begin (); it != _channels.end (); it++)
			partUser (u, *it, &u);

		return;
	}

	std::vector<std::string> channels = splitString (msg.arg (0), ",");
	std::vector<std::string> keys;

	if (msg.argsCount () > 1)
		keys = splitString (msg.arg (1), ",");

	for (size_t i = 0; i < channels.size (); i++)
	{
		const std::string &name = channels[i];

		if (!isValidChannelName (name))
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

			if (chan->modes.isInviteOnly && !chan->isInvited (u.nickname))
			{
				reply (u, Reply::errInviteOnlyChan (name));
				continue;
			}

			if (chan->findUser (&u) != chan->joinedUsers.end ())
				continue;
		}

		// Create channel if it does not exist
		if (!chan)
		{
			chan = addChannel (name, "");
			if (i < keys.size ())
				chan->key = keys[i];
	
			Channel::UserEntry *entry = chan->addUser (&u);
			entry->flags.isOperator = true;
		}
		else
		{
			chan->addUser (&u);	// This won't add the user if it already exists
			chan->useInvite (u.nickname);
		}

		if (!chan->modes.isQuiet)
		{
			Message singleJoinMsg = Message ().setIsRequest (true).setCommand ("JOIN").pushArg (chan->name);
			forwardToChannel (u, *chan, singleJoinMsg);
		}

		if (chan->topic.empty ())
			reply (u, Reply::noTopic (u.nickname, name));
		else
			reply (u, Reply::topic (u.nickname, name, chan->topic));

		if (!chan->modes.isQuiet)
			reply (u, Reply::nameReply (*chan));
	}

	reply (u, Reply::endOfNames (u.nickname, msg.arg (0)));
}
