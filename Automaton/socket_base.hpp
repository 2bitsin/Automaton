	#pragma once

#if defined(_WIN32)
	#include <WinSock2.h>
	using socket_type = SOCKET;

	static inline void close (socket_type _value)
	{
		closesocket ((socket_type)_value);
	}
#else 
	using socket_type = int;
#endif

static inline ::sockaddr_in full_address_to_saddr (const automaton::full_address& local)
{
	sockaddr_in sinaddr;
	sinaddr.sin_family = AF_INET;
	sinaddr.sin_port = local.port (true);
	sinaddr.sin_addr.s_addr = local.addr ().value (true);
	return sinaddr;
}

static inline full_address saddr_to_full_address (const sockaddr_in& sinaddr)
{
	assert (sinaddr.sin_family == AF_INET);
	auto ip = sinaddr.sin_addr.s_addr;
	ip = ntohl (ip);
	return full_address (ntohs (sinaddr.sin_port), { ip });
}

Socket::Socket (void* val)
:	_value(val)
{}


Socket::~Socket ()
{
	if (_value)
		close ();
}

void Socket::close ()
{
	::close ((socket_type)_value);
	_value = nullptr;
}

Socket::Socket (Socket&& prev)
:	Socket (std::exchange(prev._value, nullptr))
{}

auto Socket::operator = (Socket&& prev) -> Socket&
{
	this->~Socket ();
	new (this) Socket (std::move (prev));
	return *this;
}

Socket::Socket (const full_address& local)
:	Socket()
{
	bind (local);
}


void Socket::bind (const full_address& local) const
{
	auto sinaddr = full_address_to_saddr (local);
	if (::bind ((socket_type)_value, (sockaddr*)&sinaddr, sizeof (sinaddr)) < 0)
		throw std::runtime_error (__FUNCTION__ "::bind");
}
