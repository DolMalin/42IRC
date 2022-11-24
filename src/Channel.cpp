#include "Channel.hpp"
#include <algorithm>

Channel::Channel (const std::string &name, const std::string &topic) : name (name), topic (topic), joinedUsers  ()
{}

Channel::UserIt Channel::findUser (User *user)
{
	return std::find (joinedUsers.begin (), joinedUsers.end (), user);
}

User *Channel::findUserByNickname (const std::string &nick)
{
	for (UserIt it = joinedUsers.begin (); it != joinedUsers.end (); it++)
	{
		if ((*it)->nickname == nick)
			return *it;
	}

	return NULL;
}

User *Channel::findUserByUsername (const std::string &name)
{
	for (UserIt it = joinedUsers.begin (); it != joinedUsers.end (); it++)
	{
		if ((*it)->username == name)
			return *it;
	}

	return NULL;
}

void Channel::addUser (User *user)
{
	assert (user != NULL);

	if (findUser (user) != joinedUsers.end ())
		return;
	
	joinedUsers.push_back (user);
}

void Channel::removeUser (User *user)
{
	UserIt it = findUser (user);
	if (it != joinedUsers.end ())
		joinedUsers.erase (it);
}

void Channel::removeDisconnectedUsers ()
{
	for (UserIt it = joinedUsers.begin (); it != joinedUsers.end (); it++)
	{
		User *u = *it;

		if (u->isDisconnected)
			it = joinedUsers.erase (it);
	}
}
