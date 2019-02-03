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
