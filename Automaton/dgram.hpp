#pragma once

#include <cstdint>
#include <cstddef>

#include "address.hpp"

namespace automaton
{
	namespace dgram
	{

		struct socket
		{
			socket ();
			socket (const full_address& local);
			socket (const socket&) = delete;
			socket& operator = (const socket&) = delete;
			socket (socket&&);
			socket& operator = (socket&&);
			void bind (const full_address& local);
			void close ();
			~socket ();
			std::size_t recv (      full_address& addr,       void* buff, std::size_t size);
			std::size_t send (const full_address& addr, const void* buff, std::size_t size);
		private:
			void* _value;
		};
	}
}