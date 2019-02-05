#include "sheduler.hpp"
#include "utils.hpp"
#include "address.hpp"
#include "pollthread.hpp"
#include "tempprobe.hpp"
#include "stream.hpp"

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
		std::printf ("Client accepted : %s\n", addr.addr ().to_string ().c_str ());
		clients.emplace_back ([client = std::move(client)]() mutable
		{
			try
			{
				static thread_local char buff[2048];
				std::size_t len{};
				std::string req{};

				do {
					len = client.recv (buff, sizeof (buff));
					req.append (buff, len);
				} while (len >= sizeof (buff));
				std::istringstream iss (req);
				std::vector<std::string> lines;
				while (std::getline (iss, req))
					lines.emplace_back (req);
				req =
					"HTTP/1.0 200 OK\r\n"
					"Connection: close\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 41\r\n"
					"\r\n"
					"<html><body><h1>HELLO!</h1></body></html>";
				len = client.send (req.data (), req.size ());
				//client.close ();
			}
			catch (const std::exception& ex)
			{
				std::printf ("ERROR: %s\n", ex.what ());
			}
		});
	}

	return 0;
}