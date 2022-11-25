#include "Server.hpp"
#include "Reply.hpp"

void Server::user(User &u, const Message &msg)
{
	if (msg.argsCount() < 4)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}

	if (u.isRegistered ())
	{
		reply(u, Reply::errAlreadyRegistered());
		return;
	}

	if (u.givenPassword != _password)
	{
		reply(u, Reply::errPassWdMissMatch(u.nickname));
		return ;
	}

	u.username = msg.arg (0);
	// @Todo: handle mode
	u.realname = msg.arg (3);

	if (u.isRegistered ())
		reply(u, Reply::welcome(u.nickname, u.username, u.getAddressAsString()));

	// Don't forward, since this might be a security issue
	// (I don't think other users should know about other users' username)
}
