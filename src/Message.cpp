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


/*
@TODO:	- Handle multi-targets
		- Put first arg after targetted commands in target
*/
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
	message._isMessage = true;
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



	// std::cout << "Prefix: " << message._prefix << std::endl;
	// std::cout << "Command: " << message._command << std::endl;
	// for (int i = 0; i < message._argsLen; i++)
	// 	std::cout << "Args[" << i << "]: " << message._args[i] << std::endl;