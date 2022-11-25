#include "Server.hpp"
#include "Reply.hpp"

void Server::quit(User &u, const Message &msg)
{
	forwardToAllUsers (u, msg);

	if (msg.hasSuffix())
		reply(u, Reply::error(msg.suffix()));
	else
		reply(u, Reply::error(""));

	disconnect (u);
}
