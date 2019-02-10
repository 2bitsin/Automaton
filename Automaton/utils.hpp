#pragma once

#include <string>
#include <vector>

struct utils
{
	static std::string date_and_time ();

	static std::vector<std::string> split_string (std::string what, std::string how);
};