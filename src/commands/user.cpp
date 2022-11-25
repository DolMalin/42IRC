#include "Server.hpp"
#include "Reply.hpp"

void Server::user(User &u, const Message &msg)
{
	// @Todo: make sure username is valid
	// @Todo: make sure realname is valid
	// @Todo: handle modes

	if (msg.argsCount() < 4)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}

	if (u.isRegistered)
	{
		reply(u, Reply::errAlreadyRegistered());
		return;
	}

	u.username = msg.arg (0);
	// @Todo: handle mode
	u.realname = msg.arg (3);
	u.isRegistered = true;

	reply(u, Reply::welcome(u.nickname, u.username, u.getAddressAsString()));
}
