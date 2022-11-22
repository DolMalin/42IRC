#pragma once

#include <string>
#include <cstring>
#include <cmath>
#include <sstream>
#include "common.hpp"

class Message
{
private:
	std::string	_prefix;
	std::string	_command;
	std::string	_args[15];
	size_t		_argsCount;
	std::string	_suffix;
	bool		_isRequest;
	uint16_t	_replyCode;

	Message ();

public:
	static Opt<Message> parseRequest(const std::string &str);
	static Opt<Message> makeReply(const std::string &prefix, uint16_t replyCode, const std::string &suffix);
	
	std::string	stringify(void);
	void		pushArg(const std::string &arg);

	const std::string	&prefix () const;
	const std::string	&command () const;
	const std::string	&arg (size_t index) const;
	size_t				argsCount () const;
	const std::string	&suffix () const;
	bool				isRequest () const;
	uint16_t			replyCode () const;
};
