#include "Server.hpp"
#include "Reply.hpp"

void Server::kick (User &u, const Message &msg)
{
	if (msg.argsCount () < 2)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	std::vector<std::string> channelNames = splitString (msg.arg (0), ",");
	std::vector<std::string> nicknames = splitString (msg.arg (1), ",");

	if (channelNames.empty () || nicknames.empty ())
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	if (channelNames.size () > 1 && nicknames.size () != channelNames.size ())
	{
		reply (u, Reply::error ("Invalid number of channels or users"));
		return;
	}

	Channel *chan;
	if (channelNames.size () == 1)
	{
		chan = findChannelByName (channelNames[0]);
		if (!chan)
		{
			reply (u, Reply::errNoSuchChannel (channelNames[0]));
			return;
		}

		Channel::UserIt it = chan->findUser (&u);
		if (it == chan->joinedUsers.end ())
		{
			reply (u, Reply::errNotOnChannel (chan->name));
			return;
		}

		if (!it->flags.isOperator)
		{
			reply (u, Reply::errChanOpIsNeeded (chan->name));
			return;
		}
	}

	for (size_t i = 0; i < nicknames.size (); i++)
	{
		if (channelNames.size () != 1)
		{
			chan = findChannelByName (channelNames[i]);
			if (!chan)
			{
				reply (u, Reply::errNoSuchChannel (channelNames[i]));
				continue;
			}

			Channel::UserIt it = chan->findUser (&u);
			if (it == chan->joinedUsers.end ())
			{
				reply (u, Reply::errNotOnChannel (chan->name));
				return;
			}

			if (!it->flags.isOperator)
			{
				reply (u, Reply::errChanOpIsNeeded (chan->name));
				return;
			}
		}

		User *user = findUserByNickname (nicknames[i]);
		
		if (!user || !chan->removeUser (user))
		{
			reply (u, Reply::errUserNotInChannel (nicknames[i], chan->name));
			continue;
		}
		else
		{
			Message replyMsg;
			replyMsg.setIsRequest (true).setCommand ("PART").pushArg (chan->name);

			std::string kickMessage = "Kicked by " + u.nickname; 
			if (msg.argsCount () > 2)
			{
				kickMessage += ". Reason: ";
				kickMessage += msg.arg (2);
			}

			replyMsg.pushSuffix (kickMessage);

			forwardToAllUsers (*user, replyMsg);
		}
	}
}
