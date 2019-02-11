#pragma once

#include <string>
#include <vector>

namespace automaton
{
	namespace utils
	{
		std::string date_and_time ();
		std::string trim (std::string);
		std::string url_decode (std::string);
		std::vector<std::string> split_string (std::string what, std::string how);

		namespace detail
		{
			template <std::size_t... _Index, typename U>
			auto tie_at (U&& what, std::integer_sequence<std::size_t, _Index...>)
			{
				return std::make_tuple (std::forward<U>(what).at (_Index)...);
			}
		}

		template<std::size_t _Count, typename U, typename... T>
		auto v2t (const std::vector<U, T...>& what)
		{
			using seq = std::make_index_sequence<_Count>;
			return detail::tie_at (what, seq {});
		}

		template<std::size_t _Count, typename U, typename... T>
		auto v2t (std::vector<U, T...>& what)
		{
			using seq = std::make_index_sequence<_Count>;
			return detail::tie_at (what, seq {});
		}
	}

}


