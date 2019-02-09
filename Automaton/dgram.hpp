#pragma once

#include <cstdint>
#include <cstddef>

#include "address.hpp"

namespace automaton
{
	namespace dgram
	{

		struct Socket
		{
			Socket ();
			Socket (const full_address& local);
			Socket (const Socket&) = delete;
			Socket& operator = (const Socket&) = delete;
			Socket (Socket&&);
			Socket& operator = (Socket&&);
			void bind (const full_address& local) const;
			void close ();
			~Socket ();
			std::size_t recv (      full_address& addr,       void* buff, std::size_t size, std::uint32_t flags = 0) const;
			std::size_t send (const full_address& addr, const void* buff, std::size_t size, std::uint32_t flags = 0) const;

			static constexpr auto peek_flag = 0x2;
		protected:
			Socket (void*);
		private:
			void* _value;
		};
	}
}