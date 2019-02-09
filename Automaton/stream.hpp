#pragma once

#include "address.hpp"

namespace automaton
{
	namespace stream
	{
		struct Socket
		{
			~Socket ();
			Socket ();
			Socket (const Socket&) = delete;
			Socket& operator = (const Socket&) = delete;
			Socket (Socket&&);
			Socket& operator = (Socket&&);

			Socket (const full_address& local);
			void bind (const full_address& local) const;
			void listen (int backlog) const;
			std::size_t recv (		void* buff, std::size_t size, std::uint32_t flags = 0) const;
			std::size_t send (const void* buff, std::size_t size, std::uint32_t flags = 0) const;
			void close ();
			Socket accept (full_address&) const;

			static constexpr auto peek_flag = 0x2;
			static constexpr auto waitall_flag = 0x8;

		protected:
			Socket (void*);
		private:

			void* _value{ nullptr };
		};
		
	}
}

