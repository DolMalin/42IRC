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
			return Message ().setCommand("ERROR").pushSuffix(msg);
		else 
			return Message ().setCommand("ERROR");
	}

	Message ping(const std::string &server)
	{
		return Message ().setCommand("PING").pushArg(server);
	}

	Message pong(const std::string &server)
	{
		return Message ().setCommand("PONG").pushArg(server);
	}


	Message kill (const std::string &msg)
	{
		return Message ().setCommand("KILL").pushSuffix(msg);
	}

	Message updateMode(const std::string &channel, const std::string &nickname, const std::string update)
	{
		return Message ().setCommand("MODE").pushArg(channel).pushArg(update).pushArg(nickname);
	}


	Message noTopic (const std::string &nick, const std::string &channel_name)
	{
		return Message ().setReplyCode (331).pushArg (nick).pushArg (channel_name).pushSuffix ("No topic is set");
	}

	Message topic (const std::string &nick, const std::string &channel_name, const std::string &topic)
	{
		return Message ().setReplyCode (332).pushArg (nick).pushArg (channel_name).pushSuffix (topic);
	}

	Message inviting (const std::string &by, const std::string &nickname, const std::string &channelName)
	{
		return Message ().setReplyCode (341).pushArg (by).pushArg (nickname).pushArg (channelName);
	}

	Message away (const std::string &nick, const std::string &awayMessage)
	{
		Message msg;
		msg.setReplyCode (301).pushArg (nick);

		if (!awayMessage.empty ())
			msg.pushSuffix (awayMessage);

		return msg;
	}

	Message unaway ()
	{
		return Message ().setReplyCode (305).pushSuffix ("You are no longer marked as being away");
	}

	Message nowAway ()
	{
		return Message ().setReplyCode (306).pushSuffix ("You have been marked as being away");
	}

	Message nameReply (const Channel &chan)
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
				names.append ("@");
			else if (it->flags.hasVoicePriviledge)
				names.append ("+");
			
			names.append (it->user->nickname);
		}

		return Message ().setReplyCode (353).pushArg (spec).pushArg (chan.name).pushSuffix (names);
	}

	Message endOfNames (const std::string &nick, const std::string &channel)
	{
		return Message ().setReplyCode (366).pushArg (nick).pushArg (channel).pushSuffix ("End of NAMES list");
	}

	Message list(const std::string &nickname, const std::string &name, const std::string n_users, const std::string &topic)
	{
		if (!topic.empty())
			return Message ().setReplyCode(322).pushArg(nickname).pushArg(name).pushArg(n_users).pushSuffix(topic);
		else
			return Message ().setReplyCode(322).pushArg(nickname).pushArg(name).pushArg(n_users).pushSuffix("No topic set");
	}

	Message listEnd(const std::string &nickname)
	{
		return Message ().setReplyCode(323).pushArg(nickname).pushSuffix("End of LIST");
		// (void)nickname;
		// return Message ().setReplyCode(323).pushSuffix("End of /LIST");
	}

	Message channelModeIs(const std::string &name, const std::string &modes)
	{
		if (!modes.empty())
			return Message ().setReplyCode(324).pushArg("dolmalin").pushArg(name).pushArg(modes);
		else
			return Message ().setReplyCode(324).pushArg("dolmalin").pushArg(name);
	}

	Message errUnknownMode(const std::string &channel, const std::string &mode)
	{
		return Message ().setReplyCode(472).pushArg(mode).pushSuffix(std::string("is unknown mode char to me for ") + channel);
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

	Message errInviteOnlyChan (const std::string &channel_name)
	{
		return Message ().setReplyCode (473).pushArg (channel_name).pushSuffix ("Cannot join channel (+i)");
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

	Message errCannotSendToChan (const std::string &channel)
	{
		return Message ().setReplyCode (404).pushArg (channel).pushSuffix ("Cannot send to channel");
	}

	Message errChanOpIsNeeded (const std::string &channel)
	{
		return Message ().setReplyCode (482).pushArg (channel).pushSuffix ("You're not a channel operator");
	}
	
	Message errUserNotInChannel (const std::string &nick, const std::string &channelName)
	{
		return Message ().setReplyCode (441).pushArg (nick).pushArg (channelName).pushSuffix ("They aren't on that channel");
	}

	Message errKeyset(const std::string &channel)
	{
		return Message ().setReplyCode(467).pushArg(channel).pushSuffix("Channel key already set");
	}

}
