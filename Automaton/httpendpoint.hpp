#pragma once

#include <unordered_map>
#include <functional>
#include <optional>
#include <thread>
#include <atomic>

#include "address.hpp"
#include "sockstream.hpp"

namespace automaton
{

	template 
	<	typename _Scheduler,
		typename _Socket>
	struct HttpEndpoint
	{

		using scheduler_type = _Scheduler;
		using socket_type = _Socket;
		using clock_type = typename scheduler_type::clock_type;
		using request_type = struct Request;
		using response_type = std::function<void (request_type&)>;
		using sockstream_type = sockstream<socket_type>;

		struct Request
		{
			sockstream_type& client;
			full_address address;
			std::string method;
			std::string path;
			int version_minor;
			int version_major;
			std::unordered_map<std::string, std::string> query;
			std::unordered_map<std::string, std::string> headers;			
		};
		
		HttpEndpoint (socket_type& sock, 
		              scheduler_type& sched)
		:	_sockstream(sock),
			_scheduler(sched)
		{}

		template <typename _Responder>
		auto attach (std::string pattern, _Responder&& responder)
		{
			_routing.emplace (pattern, std::forward<_Responder> (responder));
		}

		auto detach (std::string pattern)
		{
			_routing.erase (pattern);
		}

		template <typename _Quit>
		void process (_Quit& _stop)
		{
			std::vector<std::thread> clhandler;
			while (!_stop)
			{
				full_address address;
				auto csocket = _sockstream.accept (address);
				auto handler = [this](auto&&... args) 
				{
					return handle (std::forward<decltype(args)> (args)...); 
				};

				clhandler.emplace_back 
				(	std::move (handler),
					std::move (csocket), 
					std::move (address)
				);
			}

		}
	protected:

		void handle (socket_type socket, full_address address)
		{
			auto pstream = std::make_unique<sockstream_type> (std::move(socket));
			request_type request{ *pstream };

			std::string prologue;
			if (!std::getline (request.client, prologue))
			{
				//request.client 
					
			}
		}

	private:
		socket_type& _sockstream;
		scheduler_type&	_scheduler;
		std::unordered_map<std::string, response_type> _routing;
	};
}