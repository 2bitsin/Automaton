#include "dgram.hpp"
#include <cassert>

#if defined(_WIN32)
	#include <winsock.h>
	using socket_type = SOCKET;
#else 
	using socket_type = int;
#endif

using namespace automaton;
using namespace dgram;


static sockaddr_in full_address_to_saddr (const full_address& local)
{
	sockaddr_in sinaddr;
	sinaddr.sin_family = AF_INET;
	sinaddr.sin_port = local.port(true);
	sinaddr.sin_addr.s_addr = local.addr().value(true);
	return sinaddr;
}

socket::socket ()
:	_value ((void*)::socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP))
{
	if (_value <= 0)
		throw std::runtime_error (__FUNCTION__ "::socket");
}

socket::socket (const full_address& local)
:	socket()
{
	bind (local);
}

socket::socket (socket&& _prev)
:	_value(std::exchange(_prev._value, nullptr))
{
}

dgram::socket& socket::operator=(socket&& prev)
{
	_value = std::exchange (prev._value, nullptr);
	return *this;
}

void socket::bind (const full_address& local)
{
	auto sinaddr = full_address_to_saddr (local);
	if (::bind ((socket_type)_value, (sockaddr*)&sinaddr, sizeof (sinaddr)) < 0)
		throw std::runtime_error (__FUNCTION__ "::bind");
}

void socket::close ()
{
#ifdef _WIN32
	closesocket ((socket_type)_value);
#else
	close ((socket_type)_value);
#endif
	_value = nullptr;
}

socket::~socket ()
{
	if (_value)
		close ();
}

std::size_t socket::recv (full_address& addr, void* buff, std::size_t size)
{
	assert (size == (int)size);
	sockaddr_in saddrin;
	int addrlen = sizeof (saddrin);
	auto length = recvfrom ((socket_type)_value, (char*)buff, (int)size, 0, (sockaddr*)&saddrin, &addrlen);
	assert (addrlen == sizeof (saddrin));
	if (length < 0)
		throw std::runtime_error (__FUNCTION__ "::recvfrom");
	return length;
}

std::size_t socket::send (const full_address& addr, const void* buff, std::size_t size)
{
	assert (size == (int)size);
	auto saddrin = full_address_to_saddr (addr);
	auto length = ::sendto ((socket_type)_value, (const char*)buff, (int)size, 0, (sockaddr*)&saddrin, sizeof (saddrin));
	if (length < 0)
		throw std::runtime_error (__FUNCTION__ "::sendto");
	return length;
}
