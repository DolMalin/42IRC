#include "Message.hpp"

static Message::Command getCommandId(std::string str)
{
	if (str == "JOIN")
		return Message::JOIN;
	else if (str == "KICK")
		return Message::KICK;
	else if (str == "MODE")
		return Message::MODE;
	else if (str == "NOTICE")
		return Message::NOTICE;
	else if (str == "PART")
		return Message::PART;
	else if (str == "PASS")
		return Message::PASS;
	else if (str == "PING")
		return Message::PING;
	else if (str == "PONG")
		return Message::PONG;
	else if (str == "PRIVMSG")
		return Message::PRIVMSG;
	else if (str == "QUIT")
		return Message::QUIT;
	else if (str == "USER")
		return Message::USER;
	else
		return Message::BLANK;
}


Opt<Message> Message::parseRequest(std::string str)
{
	Message		message;
	size_t		pos;
	std::string	token;
	
	pos = 0;
	message._prefix = "";
	message._command = BLANK;
	message._argsLen = 0;
	memset(&message._args, 0, sizeof(message._args));
	message._isRequest = true;
	if (str.empty())
		return make_opt(message, false);
	while ((pos = str.find(" ")) != std::string::npos)
	{
		token = str.substr(0, pos);
		if (message._argsLen >= 15)
			return make_opt(message, false);
		else if (!token.empty() && token.at(0) == ':' && message._prefix.empty())
			message._prefix = token.substr(1);
		else if (message._command == BLANK)
			message._command = getCommandId(token);
		else if (token.at(0) == ':')
		{
			message._args[message._argsLen] = str.substr(1);
			message._argsLen++;
			break;
		}
		else
		{
			message._args[message._argsLen] = token;
			message._argsLen++;
		}
		str.erase(0, pos + 1);
	}
	if (message._command == BLANK)
		return make_opt(message, false);
	return make_opt(message, true);
}

Opt<Message> Message::makeReply(std::string prefix, unsigned short replyCode, std::string target, std::string messageContent)
{
	Message message;

	message._isRequest = false;
	if (prefix.length() < 3 || prefix.at(0) != ':')
		return make_opt(message, false);
	message._prefix = prefix;
	if (replyCode > 399)
		return make_opt(message, false);
	message._replyCode = replyCode;
	if (prefix.empty())
		return make_opt(message, false);
	message._prefix = prefix;
	if(target.empty())
		return make_opt(message, false);
	message._args[0] = target;
	if (messageContent.empty())
		return make_opt(message, false);
	message._args[1] = messageContent;
	message._argsLen = 2;
	return make_opt(message, true);
}
