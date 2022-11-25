#include "Server.hpp"
#include "Reply.hpp"

void Server::pass(User &u, const Message &msg)
{
	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}
	if (u.isRegistered)
	{
		reply(u, Reply::errAlreadyRegistered());
		return ;
	}
	u.givenPassword = msg.arg(0);
}
