#include "Server.hpp"
#include "Reply.hpp"

void Server::kill(User &u, const Message &msg)
{
	if (msg.argsCount() < 1)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}
	if (!findUserByNickname(msg.arg(0)))
	{
		reply(u, Reply::errNoSuchNick(msg.arg(0)));
		return ;
	}
	/*
		@Todo:	- NO PRIVILEGE
				- CANT KILL SERVER
				- Add Killed nickname in list of unavailable nickname
	*/
	disconnect(u);
}
