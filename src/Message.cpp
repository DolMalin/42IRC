#include "Message.hpp"

static Message::Command getCommandId(const std::string &str)
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
	else if (str == "NICK")
		return Message::NICK;
	else
		return Message::BLANK;
}

static std::string getCommandName(Message::Command name)
{
	if (name == Message::JOIN)
		return "JOIN";
	else if (name == Message::KICK)
		return "KICK";
	else if (name == Message::MODE)
		return "MODE";
	else if (name == Message::NOTICE)
		return "NOTICE";
	else if (name == Message::PART)
		return "PART";
	else if (name == Message::PASS)
		return "PASS";
	else if (name == Message::PING)
		return "PING";
	else if (name == Message::PONG)
		return "PONG";
	else if (name == Message::PRIVMSG)
		return "PRIVMSG";
	else if (name == Message::QUIT)
		return "QUIT";
	else if (name == Message::USER)
		return "USER";
	else if (name == Message::NICK)
		return "NICK";
	else
		return "";
}

static std::string formatShort(unsigned short n)
{
	std::string			output;
	std::stringstream	ss;
	int 				n_len;

	n_len = int(log10(n) + 1);
	ss << n;
	ss >> output;
	for (int i = 0; i < 3 - n_len; i++)
		output.insert(0, "0");
	return output;
}

Opt<Message> Message::parseRequest(const std::string &str)
{
	Message		message;
	size_t		pos;
	std::string	token;
	std::string	output;
	
	pos = 0;
	message._command = BLANK;
	message._argsLen = 0;
	memset(&message._args, 0, sizeof(message._args));
	message._isRequest = true;
	output = str;
	if (output.empty())
		return make_opt(message, false);
	output += " ";
	while ((pos = output.find(" ")) != std::string::npos)
	{
		token = output.substr(0, pos);
		if (token.empty())
		{
			output.erase(0, pos + 1);
			continue;
		}
		if (message._argsLen >= 15)
			return make_opt(message, false);
		else if (token.at(0) == ':' && message._prefix.empty() && message._command == BLANK)
			message._prefix = token.substr(1);
		else if (message._command == BLANK)
			message._command = getCommandId(token);
		else if (token.at(0) == ':')
		{
			message._suffix = output.substr(1);
			break;
		}
		else
		{
			message._args[message._argsLen] = token;
			message._argsLen++;
		}
		output.erase(0, pos + 1);
	}
	if (message._command == BLANK)
		return make_opt(message, false);
	return make_opt(message, true);
}

Opt<Message> Message::makeReply(const std::string &prefix, unsigned short replyCode, const std::string &suffix)
{
	Message message;

	message._isRequest = false;
	message._argsLen = 0;
	memset(&message._args, 0, sizeof(message._args));
	if (prefix.length() < 3 || prefix.at(0) != ':')
		return make_opt(message, false);
	message._prefix = prefix;
	if (replyCode > 502)
		return make_opt(message, false);
	message._replyCode = replyCode;
	if (prefix.empty())
		return make_opt(message, false);
	message._prefix = prefix;
	if (suffix.empty())
		return make_opt(message, false);
	message._suffix = suffix;
	return make_opt(message, true);
}

std::string Message::stringify(void)
{
	std::string	output;

	if (this->_isRequest)
	{
		if (!this->_prefix.empty())
			output = output + ":" + this->_prefix + " ";
		output = output + getCommandName(this->_command);
		for (int i = 0; i < this->_argsLen; i++)
			output = output + " " + this->_args[i] ;
		if (!this->_suffix.empty())
			output = output + " " + ":" + this->_suffix;
	}
	else
	{
		output = this->_prefix + " ";
		output += formatShort(this->_replyCode);
		for (int i = 0; i < this->_argsLen; i++)
			output = output + " " + this->_args[i] ;
		output = output + " :" + this->_suffix;
	}
	return output;
}

void	Message::pushArg(const std::string &arg)
{
	if (arg.empty() || this->_argsLen >= 15)
		return ;
	this->_args[this->_argsLen] = arg;
	this->_argsLen++;
}