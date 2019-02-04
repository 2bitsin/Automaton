#pragma once

#include "address.hpp"

namespace automaton
{
	namespace stream
	{
		struct socket
		{
			~socket ();
			socket ();
			socket (const socket&) = delete;
			socket& operator = (const socket&) = delete;
			socket (socket&&);
			socket& operator = (socket&&);

			socket (const full_address& local);
			void bind (const full_address& local) const;
			void listen (int backlog) const;
			std::size_t recv (		void* buff, std::size_t size, std::uint32_t flags = 0) const;
			std::size_t send (const void* buff, std::size_t size, std::uint32_t flags = 0) const;
			void close ();
			socket accept (full_address&) const;

			static constexpr auto peek_flag = 0x2;
			static constexpr auto waitall_flag = 0x8;

		protected:
			socket (void*);
		private:

			void* _value{ nullptr };
		};
		
	}
}

