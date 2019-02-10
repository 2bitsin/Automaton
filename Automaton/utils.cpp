#include "utils.hpp"

#include <chrono>
#include <ctime>

std::string utils::date_and_time ()
{
	using namespace std::chrono;

	auto tt = system_clock::to_time_t (system_clock::now ());
	char date[2048];
	struct tm *ts = nullptr;
	ts = std::localtime (&tt);
	std::strftime (date, sizeof (date), "%Y-%m-%d %H:%M:%S", ts);


	return std::string (date);
}

auto utils::split_string (std::string what, std::string how)
	-> std::vector<std::string>
{
	std::size_t index = 0, last = 0;
	std::vector<std::string> output;

	while((index = what.find(how, last)) != what.npos)
	{ 
		output.emplace_back (what.substr (last, index - last));
		last = index + how.length ();
	}
	if (last < what.length ())
		output.emplace_back (what.substr(last));
	return output;
}
