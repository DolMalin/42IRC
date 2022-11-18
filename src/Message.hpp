#pragma once

#include <string>
#include <cstring>
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
	std::string	_prefix;
	Command 	_command;
	std::string	_args[15];
	int			_argsLen;
	bool		_isMessage;
	short		_replyCode;

	Message () {}

public:
	static Opt<Message> parseRequest(std::string str);
	/*
	@TODO:	- makeReply
			- stringify
	*/
	// static Opt<Message> makeReply(std::string prefix, short replyCode, std::string target, std::string message);
};