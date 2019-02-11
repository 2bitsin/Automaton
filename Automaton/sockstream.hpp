#pragma once

#include "stream.hpp"
#include <iostream>
#include <streambuf>
#include <fstream>

namespace automaton
{
	template <typename _Sock_type = stream::Socket, typename _Char_type = char, typename _Char_traits = std::char_traits<_Char_type>>
	struct basic_sockstream
	:	public std::basic_iostream
		<	_Char_type, 
			_Char_traits
		>
	{
		using char_traits = _Char_traits;
		using char_type = typename _Char_traits::char_type;
		using int_type = typename _Char_traits::int_type;
		using streambuf = std::basic_streambuf<_Char_type, _Char_traits>;
		using socket_type = _Sock_type;
	private:
		struct internal_rdbuf
		:	public streambuf
		{
			stream::Socket& _socket;

			static constexpr inline auto buffer_size = 1024u;

			char_type _ibuff[buffer_size];
			char_type _obuff[buffer_size];

			internal_rdbuf(stream::Socket& s)
			:	_socket(s)
			{
				streambuf::setg (_ibuff, _ibuff + buffer_size, _ibuff + buffer_size);
				streambuf::setp (_obuff, _obuff, _obuff + buffer_size);
			}

			int_type underflow () override
			{				
				auto len = _socket.recv (_ibuff, sizeof(_ibuff));
				if (len > 0)
				{
					streambuf::setg (_ibuff, _ibuff, _ibuff + buffer_size);
					return *_ibuff;
				}
				return char_traits::eof ();
			}

			int_type overflow(int_type c) override
			{
				auto leng = (streambuf::pptr () - streambuf::pbase ()) / sizeof(char_type);
				if (leng < 1)
					return char_traits::eof();
				auto sent = _socket.send (streambuf::pbase (), leng);				
				if (sent >= leng)
				{
					*_obuff = char_traits::to_char_type (c);
					streambuf::setp (_obuff, _obuff + 1u, _obuff + buffer_size);
					return c;
				}
				return char_traits::eof ();
			}

			int_type sync () override
			{
				overflow (char_traits::eof());
				return 0;
			}

			~internal_rdbuf () override
			{
				streambuf::pubsync ();
			}
		};

		socket_type _socket;
		internal_rdbuf _rdbuff;
	public:
		template <typename _SocketType>
		basic_sockstream (_SocketType&& s)
		:	std::basic_iostream<_Char_type, _Char_traits>{ &_rdbuff },
			_socket{ std::forward<_SocketType> (s) },
			_rdbuff{ _socket }
		{}

		basic_sockstream (const basic_sockstream&) = default;
		basic_sockstream (basic_sockstream&&) = default;
		basic_sockstream& operator = (const basic_sockstream&) = default;
		basic_sockstream& operator = (basic_sockstream&&) = default;

	};

	template <typename _Socket>
	using sockstream = basic_sockstream<_Socket, char>;

}