#include "sheduler.hpp"
#include "utils.hpp"
#include "address.hpp"
#include "pollthread.hpp"
#include "tempprobe.hpp"

#include <chrono>
#include <thread>

using namespace automaton;


int main ()
{
	using namespace automaton;
	using namespace std::chrono;
	using namespace std::chrono_literals;

	Clock clk;
	Scheduler sched{ clk };
	TempProbe probe{ sched, false };

	probe.callback ([](auto id, auto temp, auto q) {
		std::printf ("%08llX %lf\n", id, temp * q);
	});

	probe.exec ();

	while(sched.next () || sched.wait ())
	{}

	return 0;
}