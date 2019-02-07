#pragma once

#include "stream.hpp"
#include <iostream>

namespace automaton
{
	template <typename _Char_type = char, typename _Char_traits = std::char_traits<_Char_type>>
	struct basic_sockstream
	:	public std::basic_iostream
		<	_Char_type, 
			_Char_traits>
	{
		using char_traits = _Char_traits;
		using char_type = typename _Char_traits::char_type;
		using int_type = typename _Char_traits::int_type;
		using streambuf = std::basic_streambuf<_Char_type, _Char_traits>;
	private:
		struct internal_rdbuf
		:	public streambuf
		{
			stream::socket& _socket;
			char _chbuff[2048u];

			internal_rdbuf(stream::socket& s)
			:	_socket(s)
			{}

			int_type underflow () override
			{				
				auto len = _socket.recv (&_chbuff[0], sizeof (_chbuff));
				std::printf ("Received %zu bytes.\n", len);
				if (len > 0)
				{
					streambuf::setg (&_chbuff[0], &_chbuff[0], &_chbuff[len]);
					return _chbuff[0];
				}
				return char_traits::eof ();
			}

			int_type overflow(int_type c) override
			{
				char_type cc = char_traits::to_char_type (c);
				if (_socket.send(&cc, sizeof(cc)))
					return cc;
				return char_traits::eof();
			}
		};

		stream::socket _socket;
		internal_rdbuf _rdbuff;
	public:
		basic_sockstream (stream::socket s)
		:	std::basic_iostream<_Char_type, _Char_traits>{ &_rdbuff },
			_socket{ std::move (s) },
			_rdbuff{ _socket }
		{}

	};

	using sockstream = basic_sockstream<char>;

}