#include "Channel.hpp"
#include <algorithm>

Channel::UserFlags::UserFlags () :
	isOperator (false), hasVoicePriviledge (false)
{}

Channel::UserEntry::UserEntry (User *user) :
	user (user), flags ()
{}

Channel::Modes::Modes () :
	isInviteOnly (), isModerated (), acceptMessagesFromOutside (), isQuiet (), isPrivate (), isSecret (), hasTopic ()
{}

Channel::Channel (const std::string &name, const std::string &topic, int userLimit) :
	name (name), topic (topic), key (), userLimit (userLimit), modes (), joinedUsers  ()
{}

Channel::UserIt Channel::findUser (User *user)
{
	for (UserIt it = joinedUsers.begin (); it != joinedUsers.end (); it++)
	{
		if (it->user == user)
			return it;
	}

	return joinedUsers.end ();
}

User *Channel::findUserByNickname (const std::string &nick)
{
	for (UserIt it = joinedUsers.begin (); it != joinedUsers.end (); it++)
	{
		if (it->user->nickname == nick)
			return it->user;
	}

	return NULL;
}

User *Channel::findUserByUsername (const std::string &name)
{
	for (UserIt it = joinedUsers.begin (); it != joinedUsers.end (); it++)
	{
		if (it->user->username == name)
			return it->user;
	}

	return NULL;
}

Channel::UserEntry *Channel::addUser (User *user)
{
	assert (user != NULL);

	UserIt it = findUser (user);
	if (it != joinedUsers.end ())
		return &(*it);

	joinedUsers.push_back (UserEntry (user));

	return &joinedUsers.back ();
}

bool Channel::removeUser (User *user)
{
	UserIt it = findUser (user);
	if (it != joinedUsers.end ())
	{
		joinedUsers.erase (it);

		return true;
	}

	return false;
}

void Channel::removeDisconnectedUsers ()
{
	for (UserIt it = joinedUsers.begin (); it != joinedUsers.end (); it++)
	{
		if (it->user->isDisconnected)
			it = joinedUsers.erase (it);
	}
}
