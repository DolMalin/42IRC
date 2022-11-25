#include "common.hpp"

std::vector<std::string> splitString (const std::string &str, const std::string &delim)
{
	std::vector<std::string> result;

	size_t last_pos = 0;
	while (true)
	{
		size_t pos = str.find (delim, last_pos);
		if (pos == std::string::npos)
			break;

		if (pos - last_pos > 0)
			result.push_back (str.substr (last_pos, pos - last_pos));

		last_pos = pos + 1;
	}

	if (last_pos < str.length ())
		result.push_back (str.substr (last_pos, str.length () - last_pos));

	return result;
}