#pragma once

#include "stream.hpp"
#include <iostream>

namespace automaton
{

	template <typename _Socket = stream::socket, typename _Ctype = char>
	struct basic_sockstream
	:	public std::basic_istream<_Ctype>,
		public std::basic_ostream<_Ctype>,
		protected std::basic_streambuf<_Ctype>
	{
		using socket_type = stream::socket;
		using char_type = _Ctype;
		using streambuf = std::basic_streambuf<char_type>;
		using traits = std::char_traits<char_type>;
		using int_type = typename streambuf::int_type;
		using streamsize = std::streamsize;

		
		basic_sockstream (socket_type _imbue)
		:	std::basic_istream<char_type>{this},
			std::basic_ostream<char_type>{this},
			_socket{ std::move (_imbue) }
		{}

		basic_sockstream (const basic_sockstream&) = delete;
		basic_sockstream (basic_sockstream&&) = default;
		basic_sockstream& operator = (const basic_sockstream&) = delete;
		basic_sockstream& operator = (basic_sockstream&&) = default;

	protected:

		int_type underflow () override
		{
			char_type c;
			if (_socket.recv (&c, sizeof (c)) < sizeof (c))
				return traits::eof ();
			return traits::to_int_type(c);
		}

		streamsize xsgetn (char* p, streamsize s) override
		{
			return _socket.recv (p, s);
		}

		int_type overflow (int_type cc) override
		{
			char_type c = traits::to_char_type (cc);
			if (_socket.send (&c, sizeof (c)) < sizeof (c))
				return traits::eof ();
			return cc;
		}

		streamsize xsputn (const char* p, streamsize s) override
		{
			return _socket.send (p, s);
		}
	private:
		char_type _buff[2048u];
		socket_type _socket;
	};


	using sockstream = basic_sockstream<>;
}