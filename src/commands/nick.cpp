#include "Server.hpp"
#include "Reply.hpp"

static bool isValidNickname(const std::string &nick)
{
	if (nick == "anonymous")
		return false;

	for (size_t i = 0; i < nick.length(); i += 1)
	{
		if (!isalpha(nick[i]) && !isdigit(nick[i]) && nick[i] != '_')
			return false;
	}

	return true;
}

void Server::nick(User &u, const Message &msg)
{
	bool wasRegistered = u.isRegistered ();

	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNoNicknameGiven());
		return;
	}

	const std::string &nick = msg.arg(0);
	if (!isValidNickname(nick))
	{
		reply(u, Reply::errErroneousNickname(nick));
		return;
	}

	if (findUserByNickname(nick))
	{
		reply(u, Reply::errNicknameInUse(nick));
		return;
	}

	if (wasRegistered)
	{
		// Send nick changes to all channels the user is in, except the quiet ones
		for (ChannelIt it = _channels.begin (); it != _channels.end (); it++)
		{
			if (it->modes.isQuiet)
				continue;
			if (it->findUser (&u) == it->joinedUsers.end ())
				continue;

			forwardToChannel (u, *it, msg, false);
		}
	}

	reply (u, msg);	// Make sure the user itself is notified of the nick change

	u.nickname = nick;
	if (!wasRegistered && u.isRegistered ())
		reply (u, Reply::welcome (u.nickname, u.username, u.getAddressAsString ()));
}
