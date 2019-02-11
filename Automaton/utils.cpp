#include "utils.hpp"

#include <chrono>
#include <ctime>
#include <algorithm>

namespace automaton
{
	namespace utils
	{
		std::string date_and_time ()
		{
			using namespace std::chrono;

			auto tt = system_clock::to_time_t (system_clock::now ());
			char date [2048];
			struct tm *ts = nullptr;
			ts = std::localtime (&tt);
			std::strftime (date, sizeof (date), "%Y-%m-%d %H:%M:%S", ts);


			return std::string (date);
		}

		std::string trim (std::string what)
		{
			auto is_wspace = [] (auto c)
			{
				switch (c)
				{
				case ' ':
				case '\n':
				case '\t':
				case '\r':
				case '\b':
					return true;
				default:
					return false;
				}
			};
			auto beg = std::find_if_not (what.begin (), what.end (), is_wspace);
			auto end = std::find_if_not (what.rbegin (), what.rend (), is_wspace).base ();
			if (beg >= end)
				return {};
			return {beg, end};
		}

		auto utils::split_string (std::string what, std::string how)
			-> std::vector<std::string>
		{
			std::size_t index = 0, last = 0;
			std::vector<std::string> output;

			while ((index = what.find (how, last)) != what.npos)
			{
				output.emplace_back (what.substr (last, index - last));
				last = index + how.length ();
			}
			if (last < what.length ())
				output.emplace_back (what.substr (last));
			return output;
		}

		std::string url_decode (std::string url)
		{
			std::string out;
			out.reserve (url.size ());
			for (auto i = 0; i < url.size(); ++i)
			{ 
				if (url [i] == '+')
				{
					out.push_back (' ');
				}
				else if (url [i] == '%')
				{
					auto hex = url.substr (i + 1, 2);
					std::size_t idx = 0;
					auto val = std::stoi (hex, &idx, 16);
					out.push_back ((char)val);
					i += 2;
				}
				else
				{
					out.push_back (url [i]);
				}			
			}
			return out;
		}


	}
}

