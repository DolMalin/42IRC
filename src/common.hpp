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
#include <sstream>

#ifndef debugBreak
# define debugBreak() asm ("int3")
#endif

#undef assert

#define assert(expr, msg) {\
	if (!(expr)) {\
		std::cout << "Assertion failed at file " << __FILE__ << ", line " << __LINE__ << ": (" #expr ")\n";\
		std::cout << msg << std::endl;\
		debugBreak ();\
	} }

std::vector<std::string> splitString (const std::string &str, const std::string &delim);
std::string itos(int n);
