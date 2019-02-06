#include "sheduler.hpp"
#include "utils.hpp"
#include "address.hpp"
#include "pollthread.hpp"
#include "tempprobe.hpp"
#include "stream.hpp"
#include "sockstream.hpp"

#include <chrono>
#include <thread>
#include <sstream>

using namespace automaton;


int main ()
{
	using namespace automaton;
	using namespace std::chrono;
	using namespace std::chrono_literals;

	//Clock clk;
	//Scheduler sched{ clk };
	//TempProbe probe{ sched, false };
	//
	//probe.callback ([](auto id, auto temp, auto q) {
	//	std::printf ("%08llX %lf\n", id, temp * q);
	//});
	//
	//probe.start ();
	//
	//while(sched.next () || sched.wait ())
	//{}


	stream::socket serv{ { 1000, { 0, 0, 0, 0 } } };
	serv.listen (10);
	std::vector<std::thread> clients;
	for (;;)
	{
		full_address addr;
		
		std::printf ("Client accepted : %s\n", addr.addr ().to_string ().c_str ());
		clients.emplace_back ([] (sockstream client) mutable
		{
			try
			{
				std::string line{};
				std::vector<std::string> lines;
				while (std::getline (client, line))
					lines.emplace_back (line);
				client << 
					"HTTP/1.0 200 OK\r\n"
					"Connection: close\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 41\r\n"
					"\r\n"
					"<html><body><h1>HELLO!</h1></body></html>";
			}
			catch (const std::exception& ex)
			{
				std::printf ("ERROR: %s\n", ex.what ());
			}
		}, serv.accept (addr));
	}

	return 0;
}