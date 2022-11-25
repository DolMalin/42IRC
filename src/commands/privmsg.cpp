#include "Server.hpp"
#include "Reply.hpp"

void Server::privmsg (User &u, const Message &msg)
{
	if (msg.argsCount () < 1)
	{
		reply (u, Reply::errNoRecipient (msg.command ()));
		return;
	}

	if (msg.argsCount () < 2)
	{
		reply (u, Reply::errNoTextToSend ());
		return;
	}

	const std::string &target = msg.arg (0);

	if (target[0] == '#')	// Send to channel
	{
		Channel *chan = findChannelByName (target);
		if (!chan)
		{
			reply (u, Reply::errNoSuchChannel (target));
			return;
		}

		// Check if the user can send to the channel
		Channel::UserIt entry = chan->findUser (&u);
		if (entry == chan->joinedUsers.end ()
			&& (!chan->modes.acceptMessagesFromOutside || chan->modes.isModerated))
		{
			// If the user is not in the channel and it does not accept
			// messages from outside or it is moderated, reply error
			reply (u, Reply::errCannotSendToChan (target));
			return;
		}

		if (chan->modes.isModerated && !entry->flags.isOperator && !entry->flags.hasVoicePriviledge)
		{
			// In a moderated channel, only operators and users
			// who have the voice priviledge can speak
			reply (u, Reply::errCannotSendToChan (target));
			return;
		}

		forwardChannel (u, *chan, msg, false);
	}
	else	// Send to user
	{
		User *user = findUserByNickname (target);
		if (!user)
		{
			reply (u, Reply::errNoSuchNick (target));
			return;
		}

		// @Todo: handle AWAY
		forward (u, *user, msg, false);
	}
}
