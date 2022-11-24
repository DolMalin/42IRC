#include "Reply.hpp"

namespace Reply
{
	Message welcome (const std::string &nick, const std::string &user, const std::string &host)
	{
		return Message ().setReplyCode (1).pushSuffix (std::string ("Welcome to the Internet Relay Network ") + nick + "!" + user + "@" + host);
	}

	Message error(const std::string &msg)
	{
		if (!msg.empty())
			return Message ().setIsRequest(true).setCommand("ERROR").pushSuffix(msg);
		else 
			return Message ().setIsRequest(true).setCommand("ERROR");
	}

	Message ping(const std::string &server)
	{
		return Message ().setIsRequest(true).setCommand("PING").pushArg(server);
	}

	Message pong(const std::string &server)
	{
		return Message ().setIsRequest(true).setCommand("PONG").pushArg(server);
	}

	Message topic (const std::string &channel_name, const std::string &topic)
	{
		return Message ().setReplyCode (332).pushArg (channel_name).pushSuffix (topic);
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

	Message	errNoSuchNick (const std::string &nick)
	{
		return Message ().setReplyCode(401).pushArg(nick).pushSuffix("No such nick/channel");
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

	Message errNoOrigin()
	{
		return Message ().setReplyCode(409).pushSuffix ("No origin specified");
	}

	Message errNoSuchChannel (const std::string &channel_name)
	{
		return Message ().setReplyCode (403).pushArg (channel_name).pushSuffix ("No such channel");
	}
}
