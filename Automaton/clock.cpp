#include "clock.hpp"

#include <thread>

using namespace automaton;

auto Clock::now () const -> Clock::time_type
{
	return std::chrono::steady_clock::now ();
}

void Clock::wait_until (time_type t0) const
{
	std::this_thread::sleep_until (t0);
}

void Clock::wait_for (duration_type dt) const
{
	std::this_thread::sleep_for (dt);
}

