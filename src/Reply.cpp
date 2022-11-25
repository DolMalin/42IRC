#include "Reply.hpp"
#include "Channel.hpp"

namespace Reply
{
	Message welcome (const std::string &nick, const std::string &user, const std::string &host)
	{
		return Message ().setReplyCode (1).pushArg (nick).pushSuffix (std::string ("Welcome to the Internet Relay Network ") + nick + "!" + user + "@" + host);
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


	Message kill (const std::string &msg)
	{
		return Message ().setIsRequest(true).setCommand("KILL").pushSuffix(msg);
	}

	Message topic (const std::string &nick, const std::string &channel_name, const std::string &topic)
	{
		return Message ().setReplyCode (332).pushArg (nick).pushArg (channel_name).pushSuffix (topic);
	}

	Message namReply (const Channel &chan)
	{
		std::string spec;
		if (chan.modes.isPrivate)
			spec.assign ("*");
		else if (chan.modes.isSecret)
			spec.assign ("@");
		else
			spec.assign ("=");

		std::string names;
		for (std::list<Channel::UserEntry>::const_iterator it = chan.joinedUsers.begin (); it != chan.joinedUsers.end (); it++)
		{
			if (it != chan.joinedUsers.begin ())
				names.append (" ");

			if (it->flags.isOperator)
				names.append ("@ ");
			else
				names.append ("+ ");
			
			names.append (it->user->nickname);
		}

		return Message ().setReplyCode (353).pushArg (spec).pushArg (chan.name).pushSuffix (names);
	}

	Message endOfNames (const std::string &nick, const std::string &channel)
	{
		return Message ().setReplyCode (366).pushArg (nick).pushArg (channel).pushSuffix ("End of NAMES list");
	}

	Message list(const std::string &name, const std::string &topic)
	{
		return Message ().setReplyCode(322).pushArg("dolmalin").pushArg(name).pushSuffix(topic);
	}

	Message listEnd()
	{
		return Message ().setReplyCode(323).pushArg("dolmalin").pushSuffix("End of LIST");
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

	Message errPassWdMissMatch (const std::string &nickname)
	{
		return Message ().setReplyCode(464).pushArg(nickname).pushSuffix("Password incorrect");
	}

	Message errBadChannelKey (const std::string &channel_name)
	{
		return Message ().setReplyCode (475).pushArg (channel_name).pushSuffix ("Cannot join channel (+k)");
	}

	Message errChannelIsFull (const std::string &channel_name)
	{
		return Message ().setReplyCode (471).pushArg (channel_name).pushSuffix ("Cannot join channel (+l)");
	}

	Message errNotOnChannel (const std::string &channel_name)
	{
		return Message ().setReplyCode (442).pushArg (channel_name).pushSuffix ("You're not on that channel");
	}

	Message errNoRecipient (const std::string &command)
	{
		return Message ().setReplyCode (411).pushSuffix (std::string ("No recipient given (") + command + ")");
	}

	Message errNoTextToSend ()
	{
		return Message ().setReplyCode (412).pushSuffix ("No text to send");
	}
}
