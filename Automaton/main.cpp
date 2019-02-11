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
	Scheduler scheduler {clk};

	stream::Socket listener {{ 1000, { 0, 0, 0, 0 } }};
	listener.listen (10);
	HttpEndpoint api {listener, scheduler};

	api.attach ("/hello", 
	[] (auto& req) 
	{
		req.client_stream 
			<< "HTTP/1.0 200 OK\r\n"
			<< "Content-Length: 47\r\n"
			<< "\r\n"
			<< "<html><body><h1>Hello World!</h1></body></html>"
		;
	});
	api.attach ("/world", 
	[] (auto& req) 
	{
		req.client_stream 
			<< "HTTP/1.0 200 OK\r\n"
			<< "Content-Length: 47\r\n"
			<< "\r\n"
			<< "<html><body><h1>World! Hello</h1></body></html>"
		;		
	});

	std::atomic_bool shouldQuit {false};
	auto _wait = std::thread ([&] ()
	{
		api.process (shouldQuit);
	});

	while (scheduler.next () || scheduler.wait ());

	shouldQuit = true;
	_wait.join();
	return 0;
}