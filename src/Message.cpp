#include "Message.hpp"

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
	Message	message;
	message._isRequest = true;
	message._hasPrefix = false;
	std::string output = str;
	if (output.empty())
		return make_opt(message, false);
		
	output += " ";

	size_t	pos = 0;
	bool	commandSet = false;
	while ((pos = output.find(" ")) != std::string::npos)
	{
		std::string token = output.substr(0, pos);

		if (token.empty())
		{
			output.erase(0, pos + 1);
			continue;
		}
		if (message._argsCount >= 15)
		{
			return make_opt(message, false);
		}
		else if (token.at(0) == ':' && message._prefix.empty() && !commandSet)
		{
			message._prefix = token.substr(1);
		}
		else if (!commandSet)
		{
			message._command = token;
			commandSet = true;
		}
		else if (token.at(0) == ':')
		{
			message.pushArg (output.substr(1));
			message._hasPrefix = true;
			break;
		}
		else
		{
			message.pushArg (token);
		}
		
		output.erase(0, pos + 1);
	}

	return make_opt(message, true);
}

Opt<Message> Message::makeReply(const std::string &prefix, uint16_t replyCode)
{
	Message message;

	message._isRequest = false;
	message._argsCount = 0;
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
	if (!message._hasPrefix)
		return make_opt(message, false);
	return make_opt(message, true);
}

Message::Message () :
	_prefix (), _command (), _args (), _argsCount (), _isRequest (), _replyCode ()
{}

std::string Message::stringify(void)
{
	std::string	output;

	if (this->_isRequest)
	{
		if (!this->_prefix.empty())
			output = output + ":" + this->_prefix + " ";
		
		output = output + this->_command;

		for (size_t i = 0; i < this->_argsCount; i++)
			output = output + " " + this->_args[i] ;
		
		if (this->_hasPrefix)
			output = output + " " + ":" + this->_args[this->_argsCount];
	}
	else
	{
		output = this->_prefix + " ";
		output += formatShort(this->_replyCode);

		for (size_t i = 0; i < this->_argsCount; i++)
			output = output + " " + this->_args[i] ;

		output = output + " :" + this->_args[this->_argsCount];
	}

	return output;
}

void	Message::pushArg(const std::string &arg)
{
	assert (_argsCount < 15, "Message cannot have more than 15 arguments.");

	if (arg.empty() || this->_argsCount >= 15)
		return ;
	this->_args[this->_argsCount] = arg;
	this->_argsCount++;
}

const std::string &Message::prefix () const { return _prefix; }
const std::string &Message::command () const { return _command; }
const std::string &Message::arg (size_t index) const { assert (index < _argsCount); return _args[index]; }
size_t Message::argsCount () const { return _argsCount; }
bool Message::isRequest () const { return _isRequest; }
uint16_t Message::replyCode () const { return _replyCode; }
