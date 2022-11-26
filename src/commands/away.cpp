#include "Server.hpp"
#include "Reply.hpp"

void Server::away (User &u, const Message &msg)
{
	if (msg.argsCount () == 0)
	{
		u.isAway = false;
		u.awayMessage.clear ();
		forwardToAllUsers (u, Reply::unaway ());
	}
	else
	{
		u.isAway = true;
		u.awayMessage = msg.arg (0);
		forwardToAllUsers (u, Reply::nowAway ());
	}
}
