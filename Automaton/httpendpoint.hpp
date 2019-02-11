#pragma once

#include <unordered_map>
#include <functional>
#include <optional>
#include <thread>
#include <atomic>
#include <memory>

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
			socket_type client_socket;
			sockstream_type client_stream;
			full_address address;
			std::string method;
			std::string path;
			int version_major;
			int version_minor;
			std::unordered_map<std::string, std::string> headers;
			std::unordered_map<std::string, std::string> query;

			Request
			(	socket_type _sock,
				full_address _addr,
				std::string _method,
				std::string _path,
				int _ver_maj,
				int _ver_min,
				std::unordered_map<std::string, std::string> _headers,
				std::unordered_map<std::string, std::string> _query)
			:	client_socket (std::move (_sock)),
				client_stream (client_socket),
				address (std::move (_addr)),
				method (std::move (_method)),
				path (std::move (_path)),
				version_major (_ver_maj),
				version_minor (_ver_min),
				headers (std::move (_headers)),
				query (std::move (_query))
			{}
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
						return _upcast ().initiate_request (std::move (client), std::move (address));
					},
					std::move (client),
					std::move (address)
				}.detach ();
			}
		}

	protected:
		auto initiate_request (socket_type client, full_address address)
		{
			using namespace std::string_literals;

			std::shared_ptr<request_type> req;
			{
				std::string line;
				sockstream_type clio (client);

				if (!std::getline (clio, line))
					return _upcast ().respond_with_error (clio, 400);

				line = utils::trim (line);

				auto [method, pathQuery, verProto] = utils::v2t<3> (utils::split_string (line, " "));
				auto [protocol, version] = utils::v2t<2> (utils::split_string (verProto, "/"));
				auto [sVerMaj, sVerMin] = utils::v2t<2> (utils::split_string (version, "."));
				auto pathComponents = utils::split_string (pathQuery, "?");
				auto path = pathComponents.at (0);

				std::unordered_map<std::string, std::string> headers;
				std::unordered_map<std::string, std::string> query;

				if (pathComponents.size () > 1)
				{
					auto queryParts = utils::split_string (pathComponents.at (1), "&");
					for (auto line: queryParts)
					{
						auto [lhs, rhs] = utils::v2t<2> (utils::split_string (utils::trim (line), "="));
						query.emplace (utils::trim (lhs), utils::url_decode (utils::trim (rhs)));
					}
				}

				assert (protocol == "HTTP");

				while (std::getline (clio, line) && line != "\r" && line != "")
				{
					line = utils::trim (line);
					auto [lhs, rhs] = utils::v2t<2> (utils::split_string (line, ":"));
					headers.emplace (utils::trim (lhs), utils::trim (rhs));
				}
			
				req = std::make_shared<request_type>
				(	std::move (client),
					std::move (address),
					std::move (method),
					std::move (path),
					std::stoi (sVerMaj),
					std::stoi (sVerMin),
					std::move (headers),
					std::move (query)
				);
			}

			_scheduler.post (
			[this, pRequest = std::move (req)] (auto& sched, auto delay)
			{
				auto& req = *pRequest;
				try
				{
					_routing.at (req.path) (req);

				}
				catch (...)
				{
					req.client_stream << get_static_response (404);
				}
			});
		}

		auto respond_with_error (sockstream_type& clio, int code, const full_address& address = full_address {})
		{
			clio << get_static_response (code);
		}

	private:
		auto&& _upcast ()
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