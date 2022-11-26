#include "Server.hpp"
#include "Reply.hpp"

void Server::invite (User &u, const Message &msg)
{
	// @Todo: RPL_AWAY

	if (msg.argsCount () < 2)
	{
		reply (u, Reply::errNeedMoreParams (msg.command ()));
		return;
	}

	const std::string &nickname = msg.arg (0);
	User *user = findUserByNickname (nickname);
	if (!user)
	{
		reply (u, Reply::errNoSuchNick (nickname));
		return;
	}

	const std::string &channelName = msg.arg (1);
	Channel *channel = findChannelByName (channelName);
	if (!channel)
	{
		reply (u, Reply::errNoSuchChannel (channelName));
		return;
	}

	Channel::UserIt it = channel->findUser (&u);
	if (it == channel->joinedUsers.end ())
	{
		reply (u, Reply::errNotOnChannel (channelName));
		return;
	}
	
	if (channel->modes.isInviteOnly && !it->flags.isOperator)
	{
		reply (u, Reply::errChanOpIsNeeded (channelName));
		return;
	}

	channel->addInvite (nickname);

	Message inviting = Reply::inviting (u.nickname, nickname, channelName);
	forward (u, *user, inviting);
	reply (u, inviting);
}
