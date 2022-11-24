#pragma once

#include "common.hpp"
#include "User.hpp"

class Channel
{
public:
	std::string name;
	std::string topic;
	std::list<User *> joinedUsers;

	typedef std::list<User *>::iterator UserIt;

public:
	explicit Channel (const std::string &name = "", const std::string &topic = "");
	
	UserIt findUser (User *user);
	User *findUserByNickname (const std::string &name);
	User *findUserByUsername (const std::string &name);

	void addUser (User *user);
	void removeUser (User *user);
	void removeDisconnectedUsers ();
};
