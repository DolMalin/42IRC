#pragma once

#include <iostream>
#include <time.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>

#ifndef debugBreak
# define debugBreak() asm ("int3")
#endif

#ifndef assert

# define assert(expr, ...) {\
	if (!(expr)) {\
		std::cout << "Assertion failed at file " << __FILE__ << ", line " << __LINE__ << ": (" #expr ")\n";\
		std::cout << "" __VA_ARGS__ << std::endl;\
		debugBreak ();\
	} }
	
#endif

template<class T, class Err = bool>
struct Opt
{
	union
	{
		Err err;
		Err ok;	// If Err is a bool, this is a bit more explicit to use `if (opt.ok)` than `if (opt.err)`
	};
	T val;

	Opt (T val, Err err) : err (err), val (val)
	{}
};

template<class T, class Err>
Opt<T, Err> make_opt (T val, Err err)
{
	Opt<T, Err> result (val, err);

	return result;
}


std::vector<std::string> splitString (const std::string &str, const std::string &delim);