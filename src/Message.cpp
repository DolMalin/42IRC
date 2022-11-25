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

bool Message::parseRequest(const std::string &str)
{
	_isRequest = true;
	_hasSuffix = false;
	if (str.empty())
		return false;

	std::string output = str;
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
		if (_argsCount >= 15)
			return false;
		else if (token.at(0) == ':' && _prefix.empty() && !commandSet)
			_prefix = token.substr(1);
		else if (!commandSet)
		{
			_command = token;
			commandSet = true;
		}
		else if (token.at(0) == ':')
		{
			pushArg (output.substr(1));
			_hasSuffix = true;
			break;
		}
		else
			pushArg (token);
		
		output.erase(0, pos + 1);
	}

	return true;
}

Message::Message () :
	_prefix (), _command (), _args (), _argsCount (), _isRequest (), _hasSuffix (), _replyCode ()
{}

std::string Message::stringify(void) const
{
	std::string	output;

	if (!this->_prefix.empty())
		output += ":" + this->_prefix + " ";

	if (this->_isRequest)
		output += this->_command;
	else
		output += formatShort(this->_replyCode);

	for (size_t i = 0; _argsCount > 0 && i < this->_argsCount; i++)
	{
		if (i == _argsCount - 1 && this->_hasSuffix)
			output += " :" + this->_args[i];
		else
			output += " " + this->_args[i];
	}

	return output;
}

Message	&Message::setPrefix(const std::string &prefix)
{
	assert (prefix.length() >= 3 || prefix.at(0) == ':')
	this->_prefix = prefix;

	return *this;
}

Message &Message::setReplyCode(uint16_t replyCode)
{
	assert (replyCode <= 502);

	this->_replyCode = replyCode;
	return *this;
}

Message	&Message::pushArg(const std::string &arg)
{
	assert (!arg.empty (), "Argument is empty.");
	assert (_argsCount < 15, "Message cannot have more than 15 arguments.");

	this->_args[this->_argsCount] = arg;
	this->_argsCount++;

	return *this;
}

Message	&Message::pushSuffix(const std::string &arg)
{
	assert (!_hasSuffix, "Message already has a suffix.");

	pushArg (arg);
	_hasSuffix = true;
	
	return *this;
}

Message	&Message::setCommand(const std::string &command)
{
	assert (!this->_command.empty() || !command.empty());

	this->_command = command;
	return *this;
}

Message &Message::setIsRequest(bool isRequest)
{
	this->_isRequest = isRequest;
	return *this;
}

const std::string &Message::prefix () const { return _prefix; }
const std::string &Message::command () const { return _command; }
const std::string &Message::arg (size_t index) const { assert (index < _argsCount); return _args[index]; }
size_t Message::argsCount () const { return _argsCount; }
const std::string &Message::suffix () const { assert (_hasSuffix && _argsCount > 0); return _args[_argsCount - 1]; }
bool Message::isRequest () const { return _isRequest; }
bool Message::hasSuffix () const {return _hasSuffix; }
uint16_t Message::replyCode () const { return _replyCode; }
