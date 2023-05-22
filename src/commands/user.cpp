#include "Server.hpp"
#include "Reply.hpp"

void Server::user(User &u, const Message &msg)
{
	if (msg.argsCount() < 4)
	{
		reply(u, Reply::errNeedMoreParams(msg.command()));
		return;
	}

    if (u.nickname.empty ())
    {
		reply(u, Reply::errNoNicknameGiven ());
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
		disconnect(u);
		return ;
	}

	u.username = msg.arg (0);
	u.realname = msg.arg (3);

	if (u.isRegistered ())
		reply(u, Reply::welcome(u.nickname, u.username, u.getAddressAsString()));
}
