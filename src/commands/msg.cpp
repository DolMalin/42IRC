#include "Server.hpp"
#include "Reply.hpp"

void Server::messageBase (User &u, const Message &msg, bool isNotice)
{
	if (msg.argsCount () < 1)
	{
		if (!isNotice)
			reply (u, Reply::errNoRecipient (msg.command ()));
		return;
	}

	if (msg.argsCount () < 2)
	{
		if (!isNotice)
			reply (u, Reply::errNoTextToSend ());
		return;
	}

	const std::string &target = msg.arg (0);

	if (target[0] == '#')	// Send to channel
	{
		Channel *chan = findChannelByName (target);
		if (!chan)
		{
			if (!isNotice)
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
			if (!isNotice)
				reply (u, Reply::errCannotSendToChan (target));
			return;
		}

		if (chan->modes.isModerated && !entry->flags.isOperator && !entry->flags.hasVoicePriviledge)
		{
			// In a moderated channel, only operators and users
			// who have the voice priviledge can speak
			if (!isNotice)
				reply (u, Reply::errCannotSendToChan (target));
			return;
		}

		forwardToChannel (u, *chan, msg, false);
	}
	else	// Send to user
	{
		User *user = findUserByNickname (target);
		if (!user)
		{
			if (!isNotice)
				reply (u, Reply::errNoSuchNick (target));
			return;
		}

		if (!isNotice && user->isAway)
			reply (u, Reply::away (user->nickname, user->awayMessage));

		forward (u, *user, msg, false);
	}
}

void Server::privmsg (User &u, const Message &msg)
{
	messageBase (u, msg, false);
}

void Server::notice (User &u, const Message &msg)
{
	messageBase (u, msg, true);
}
