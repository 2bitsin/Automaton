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
		auto client = serv.accept (addr);
		std::printf ("Client accepted : %s\n", addr.to_string ().c_str ());
		clients.emplace_back ([] (sockstream client) mutable
		{
			try
			{
				std::string line{};
				std::vector<std::string> lines;
				while (std::getline (client, line) && line != "\r")
					lines.emplace_back (line);
				assert (lines.size () > 1);
				line = "<html><body><h1>" + utils::date_and_time() + "</h1></body></html>";
				client 
					<< "HTTP/1.0 200 OK\r\n"
					<< "Connection: close\r\n"
					<< "Content-Type: text/html\r\n"
					<< "Content-Length: " << line.size () << "\r\n"
					<< "Refresh: 1; url=/\r\n"
					<< "\r\n"				
					<< line;
			}
			catch (const std::exception& ex)
			{
				std::printf ("ERROR: %s\n", ex.what ());
			}
		}, std::move (client));
	}

	return 0;
}