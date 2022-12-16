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

#undef assert

#define assert(expr, ...) {\
	if (!(expr)) {\
		std::cout << "Assertion failed at file " << __FILE__ << ", line " << __LINE__ << ": (" #expr ")\n";\
		std::cout << "" __VA_ARGS__ << std::endl;\
		debugBreak ();\
	} }

std::vector<std::string> splitString (const std::string &str, const std::string &delim);
