#pragma once

#include <unordered_map>
#include <functional>
#include <optional>
#include <thread>
#include <atomic>

#include "address.hpp"
#include "sockstream.hpp"
#include "utils.hpp"

namespace automaton
{
	namespace detail
	{
		struct HttpEndpointBase
		{
			static const std::unordered_map<int, std::string> _static_responses;
			static std::string get_static_response (int code);
		};
	}


	template
	<	typename _Scheduler,
		typename _Socket,
		typename _Mixin = void
		>
	struct HttpEndpoint:
		detail::HttpEndpointBase
	{

		using scheduler_type = _Scheduler;
		using socket_type = _Socket;
		using clock_type = typename scheduler_type::clock_type;
		using request_type = struct Request;
		using response_type = std::function<void (request_type&)>;
		using sockstream_type = sockstream<socket_type&>;

		struct Request
		{
			socket_type sockclient;
			sockstream_type ioclient;
			full_address address;
			std::string method;
			std::string path;
			int version_minor;
			int version_major;
			std::unordered_map<std::string, std::string> query;
			std::unordered_map<std::string, std::string> headers;
		};

		HttpEndpoint (socket_type& sock, scheduler_type& sched)
		:	_sockstream (sock),
			_scheduler (sched)
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
		void process (_Quit&& _stop)
		{
			std::vector<std::thread> clhandler;
			while (!_stop)
			{
				full_address address;
				auto client = _sockstream.accept (address);
				std::thread 
				{
					[this] (auto client, auto address) 
					{ 
						return _upcast().initiate_request (std::move (client), std::move(address)); 
					},
					std::move (client),
					std::move (address)
				}.detach ();			
			}
		}

	protected:
		auto initiate_request (socket_type client, full_address address)
		{
			std::string line;
			sockstream_type clio (client);
			if (!std::getline (clio, line))
				return _upcast().respond_with_error (clio, 400);
			auto prologue	= utils::split_string (line, " ");
			auto method		= prologue.at (0);
			auto urlpath	= prologue.at (1);
			auto protocol	= prologue.at (2);
		}

		auto respond_with_error (sockstream_type& clio, int code, const full_address& address = full_address {})
		{
			clio << get_static_response (code);
		}

	private:
		auto&& _upcast()
		{ 
			if constexpr (std::is_base_of_v<HttpEndpoint, _Mixin>)
				return (_Mixin&)*this;
			else				
				return *this;
		}
		auto&& _upcast () const
		{
			if constexpr (std::is_base_of_v<HttpEndpoint, _Mixin>)
				return (const _Mixin&)*this;
			else
				return *this;
		}

	private:
		socket_type& _sockstream;
		scheduler_type&	_scheduler;
		std::unordered_map<std::string, response_type> _routing;
		
	};
}