#include "Reply.hpp"

namespace Reply
{
	Message welcome (const std::string &nick, const std::string &user, const std::string &host)
	{
		return Message ().setReplyCode (1).pushSuffix (std::string ("Welcome to the Internet Relay Network ") + nick + "!" + user + "@" + host);
	}

	Message errUnknownCommand (const std::string &command)
	{
		return Message ().setReplyCode (421).pushArg (command).pushSuffix ("Unknown command");
	}

	Message	errNoNicknameGiven ()
	{
		return Message ().setReplyCode (431).pushSuffix ("No nickname given");
	}
	
	Message	errErroneousNickname (const std::string &nick)
	{
		return Message ().setReplyCode (432).pushArg (nick).pushSuffix ("Erroneous nickname");
	}

	Message	errNicknameInUse (const std::string &nick)
	{
		return Message ().setReplyCode (433).pushArg (nick).pushSuffix ("Nickname is already in use");
	}

	Message	errNickCollision (const std::string &nick, const std::string &user, const std::string &host)
	{
		return Message ().setReplyCode (436).pushArg (nick).pushSuffix (std::string ("Nickname collision KILL from ") + user + "@" + host);
	}

	Message errUnavailResource (const std::string &nick_or_channel)
	{
		return Message ().setReplyCode (437).pushArg (nick_or_channel).pushSuffix ("Nick/channel is temporarily unavailable");
	}

	Message	errRestricted ()
	{
		return Message ().setReplyCode (484).pushSuffix ("Your connection is restricted!");
	}

	Message	errNeedMoreParams (const std::string &command)
	{
		return Message ().setReplyCode (461).pushArg (command).pushSuffix ("Not enough parameters");
	}

	Message	errAlreadyRegistered ()
	{
		return Message ().setReplyCode (462).pushSuffix ("Unauthorized command (already registered)");
	}
}
