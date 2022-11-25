#pragma once

#include "Message.hpp"

class Channel;

namespace Reply
{
	Message welcome (const std::string &nick, const std::string &user, const std::string &host);
	Message error (const std::string &msg);
	Message ping (const std::string &server);
	Message pong (const std::string &server);
	Message topic (const std::string &nick, const std::string &channel_name, const std::string &topic);
	Message kill(const std::string &msg);
	Message inviting (const std::string &by, const std::string &nickname, const std::string &channelName);

	Message nameReply (const Channel &channel);
	Message endOfNames (const std::string &nick, const std::string &channel);
	Message list(const std::string &name, const std::string &topic);
	Message listEnd();

	Message errUnknownCommand (const std::string &command);
	Message	errNoNicknameGiven ();
	Message	errErroneousNickname (const std::string &nick);
	Message	errNicknameInUse (const std::string &nick);
	Message	errNoSuchNick (const std::string &nick);
	Message	errNickCollision (const std::string &nick, const std::string &user, const std::string &host);
	Message errUnavailResource (const std::string &nick_or_channel);
	Message	errRestricted ();
	Message	errNeedMoreParams (const std::string &command);
	Message	errAlreadyRegistered ();
	Message errNoOrigin ();
	Message errPassWdMissMatch (const std::string &nickname);
	Message errNoSuchChannel (const std::string &channel_name);
	Message errInviteOnlyChan (const std::string &channel_name);
	Message errBadChannelKey (const std::string &channel_name);
	Message errChannelIsFull (const std::string &channel_name);
	Message errNotOnChannel (const std::string &channel_name);
	Message errNoRecipient (const std::string &command);
	Message errNoTextToSend ();
	Message errCannotSendToChan (const std::string &channel);
	Message errChanOpIsNeeded (const std::string &channel);
}
