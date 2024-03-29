#include "Channel.hpp"
#include <algorithm>

Channel::UserFlags::UserFlags () :
	isOperator (false), hasVoicePriviledge (false)
{}

Channel::UserEntry::UserEntry (User *user) :
	user (user), flags ()
{}

Channel::Modes::Modes () :
	isInviteOnly (), isModerated (), acceptMessagesFromOutside (), isQuiet (), isPrivate (), isSecret (), isTopicChangeable ()
{}

Channel::Channel (const std::string &name, const std::string &topic, int userLimit) :
	name (name), topic (topic), key (), userLimit (userLimit), modes (), joinedUsers  (), invites ()
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
	assert (user != NULL, "User is null");

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

bool Channel::isInvited (const std::string &nickname)
{
	return std::find (invites.begin (), invites.end (), nickname) != invites.end ();
}

void Channel::addInvite (const std::string &nickname)
{
	if (isInvited (nickname))
		return;
	invites.push_back (nickname);
}

bool Channel::useInvite (const std::string &nickname)
{
	std::list<std::string>::iterator it = std::find (invites.begin (), invites.end (), nickname);
	if (it == invites.end ())
		return false;

	invites.erase (it);

	return true;
}

void Channel::setMode(const char mode, char c)
{
	bool val = c == '+' ? true : false;

	switch(mode)
	{
		case 'i':
			modes.isInviteOnly = val;
			break;
		case 'm':
			modes.isModerated = val;
			break;
		case 'n':
			modes.acceptMessagesFromOutside = val;
			break;
		case 'q':
			modes.isQuiet = val;
			break;
		case 'p':
			modes.isPrivate = val;
			break;
		case 's':
			modes.isSecret = val;
			break;
		case 't':
			modes.isTopicChangeable = val;
			break;
		case 'k':
			if (!val)
				key.clear();
			break;

	}
}

void Channel::setMode(const char mode, char c, const std::string &arg)
{
	User *u = findUserByNickname(arg);
	UserIt it = findUser(u);
	bool val = c == '+' ? true : false;

	switch(mode)
	{
		case 'o':
			(*it).flags.isOperator = val;
			break;
		case 'v':
			(*it).flags.hasVoicePriviledge = val;
			break;

		case 'k':
			if (val)
				key = arg;
			else
				key.clear();
			break;
	}
}

std::string Channel::modeToString()
{
	std::string str;

	if (modes.isInviteOnly)
		str += 'i';
	if (modes.isModerated)
		str += 'm';
	if (modes.acceptMessagesFromOutside)
		str += 'n';
	if (modes.isQuiet)
		str += 'q';
	if (modes.isPrivate)
		str += 'p';
	if (modes.isSecret)
		str += 's';
	if (modes.isTopicChangeable)
		str += 't';
	if (!key.empty())
		str += 'k';
	return str;
}
