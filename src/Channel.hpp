#pragma once

#include "common.hpp"
#include "User.hpp"

class Channel
{
public:
	struct UserFlags
	{
		bool isAway : 1; // a // @odo
		bool isOperator : 1;	// o // @Todo
		bool hasVoicePriviledge : 1;	// v // @Todo

	
		UserFlags ();
	
		bool fromString (const std::string &str);	// @Todo
		std::string toString () const;	// @Todo
	};

	struct UserEntry
	{
		User *user;
		UserFlags flags;
	
		UserEntry (User *user);
	};

	struct Modes
	{
		//bool isAnonymous : 1;	// Not handled for # channels
		bool isInviteOnly : 1;	// i // @Todo
		bool isModerated : 1;	// m // @Todo
		bool acceptMessagesFromOutside : 1;	// n // @Todo
		bool isQuiet : 1;	// q // @Todo
		bool isPrivate : 1;	// p // @Todo
		bool isSecret : 1;	// s // @Todo
		bool hasTopic : 1;	// t // @Todo

		Modes ();

		bool fromString (const std::string &str);	// @Todo
		std::string toString () const;	// @Todo
	};

public:
	std::string name;	// 50 characters long, prefixed with #, does not contain spaces, commas or ASCII 7 characters
	std::string topic;	// Set with the t mode
	std::string key;	// Set with the k mode
	int userLimit;	// Set with the l flag
	Modes modes;

	std::list<UserEntry> joinedUsers;

	typedef std::list<UserEntry>::iterator UserIt;

public:
	explicit Channel (const std::string &name = "", const std::string &topic = "", int userLimit = 150);
	
	UserIt findUser (User *user);
	User *findUserByNickname (const std::string &name);
	User *findUserByUsername (const std::string &name);

	UserEntry *addUser (User *user);
	bool removeUser (User *user);
	void removeDisconnectedUsers ();
};
