#pragma once

#include <cstddef>
#include <cstdint>

#include <chrono>
#include <condition_variable>

namespace automaton
{

	struct Clock
	{
		using time_type = std::chrono::steady_clock::time_point;
		using duration_type = std::chrono::milliseconds;

		auto now () const -> time_type;
		void wait_until (time_type) const;
		void wait_for (duration_type) const;

		template <typename _Cvar, typename _Lock>
		auto wait_until (time_type t0, _Cvar& cvar, _Lock& lock) const
		{
			return cvar.wait_until (lock, t0);
		}

		template <typename _Cvar, typename _Lock>
		auto wait_for (duration_type dt, _Cvar& cvar, _Lock& lock) const
		{
			return cvar.wait_for (lock, dt);
		}
	};
}