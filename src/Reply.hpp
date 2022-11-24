#pragma once

#include "Message.hpp"

namespace Reply
{
	Message welcome (const std::string &nick, const std::string &user, const std::string &host);
	Message error (const std::string &msg);
	Message ping (const std::string &server);
	Message pong (const std::string &server);
	Message topic (const std::string &channel_name, const std::string &topic);

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

	Message errNoSuchChannel (const std::string &channel_name);
}
