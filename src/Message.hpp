#pragma once

#include <string>
#include <cstring>
#include <cmath>
#include <sstream>
#include "common.hpp"


class Message
{
public:
	enum Command
	{
		BLANK,
		JOIN,
		KICK,
		MODE,
		NOTICE,
		PART,
		PASS,
		PING,
		PONG,
		PRIVMSG,
		QUIT,
		USER
	};


private:
	std::string		_prefix;
	Command 		_command;
	std::string		_args[15];
	std::string		_suffix;
	int				_argsLen;
	bool			_isRequest;
	unsigned short	_replyCode;

	Message () {}

public:
	static Opt<Message> parseRequest(std::string str);
	static Opt<Message> makeReply(std::string prefix, unsigned short replyCode, std::string suffix);
	
	std::string	stringify(void);
	void		pushArg(std::string arg);
	// @TODO: Push arg function to add elem in args
};