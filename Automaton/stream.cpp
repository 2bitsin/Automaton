#include <cassert>

#include "stream.hpp"

using namespace automaton;
using namespace automaton::stream;

#include "socket_base.hpp"


Socket::Socket ()
:	Socket((void*)::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
	if (_value <= 0)
		throw std::runtime_error (__FUNCTION__ "::socket");
}

void Socket::listen (int backlog) const 
{
	if (::listen ((socket_type)_value, backlog))
		throw std::runtime_error (__FUNCTION__ "::listen");
}

std::size_t Socket::recv (void* buff, std::size_t size, std::uint32_t flags) const
{	
	assert (size == (int)size);
	auto length = ::recv ((socket_type)_value, (char*)buff, (int)size, (int)flags);
	if (length < 0)
		throw std::runtime_error (__FUNCTION__ "::recv");
	return length;
}

std::size_t Socket::send (const void* buff, std::size_t size, std::uint32_t flags) const
{
	assert (size == (int)size);
	auto length = ::send ((socket_type)_value, (const char*)buff, (int)size, (int)flags);
	if (length < 0)
		throw std::runtime_error (__FUNCTION__ "::send");
	return length;
}


auto Socket::accept (full_address& addr) const -> Socket
{
	sockaddr_in saddrin;
	int size = sizeof (saddrin);
	auto _newsock = ::accept ((socket_type)_value, (sockaddr*)&saddrin, &size);
	if (_newsock <= 0)
		throw std::runtime_error (__FUNCTION__ "::accept");
	addr = saddr_to_full_address (saddrin);
	return {(void*)_newsock};
}