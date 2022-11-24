#include "Server.hpp"
#include "Reply.hpp"

static bool isValidNickname(const std::string &nick)
{
	for (size_t i = 0; i < nick.length(); i += 1)
	{
		if (!isalpha(nick[i]) && !isdigit(nick[i]) && nick[i] != '_')
			return false;
	}

	return true;
}

void Server::nick(User &u, const Message &msg)
{
	// @Todo: reply ERR_NICKCOLLISION
	// @Todo: reply ERR_UNAVAILRESOURCE
	// @Todo: reply ERR_RESTRICTED

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

	u.nickname = nick;
}
