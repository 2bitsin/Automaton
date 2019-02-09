#include "sheduler.hpp"
#include "utils.hpp"
#include "address.hpp"
#include "pollthread.hpp"
#include "tempprobe.hpp"
#include "stream.hpp"
#include "sockstream.hpp"
#include "httpendpoint.hpp"

#include <chrono>
#include <thread>
#include <sstream>

using namespace automaton;


int main ()
{
	using namespace automaton;
	using namespace std::chrono;
	using namespace std::chrono_literals;

	Clock clk;
	Scheduler scheduler{ clk };

	stream::Socket listener{ { 1000, { 0, 0, 0, 0 } } };
	listener.listen (10);

	HttpEndpoint endpoint{ listener, scheduler };


	return 0;
}