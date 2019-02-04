#include <cassert>

#include "dgram.hpp"

using namespace automaton;
using namespace dgram;

#include "socket_base.hpp"

socket::socket ()
:	_value ((void*)::socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP))
{
	if (_value <= 0)
		throw std::runtime_error (__FUNCTION__ "::socket");
}

std::size_t socket::recv (full_address& addr, void* buff, std::size_t size, std::uint32_t flags) const
{
	assert (size == (int)size);
	sockaddr_in saddrin;
	int addrlen = sizeof (saddrin);
	auto length = recvfrom ((socket_type)_value, (char*)buff, (int)size, (int)flags, (sockaddr*)&saddrin, &addrlen);
	assert (addrlen == sizeof (saddrin));
	if (length < 0)
		throw std::runtime_error (__FUNCTION__ "::recvfrom");
	addr = saddr_to_full_address (saddrin);
	return length;
}

std::size_t socket::send (const full_address& addr, const void* buff, std::size_t size, std::uint32_t flags) const
{
	assert (size == (int)size);
	auto saddrin = full_address_to_saddr (addr);
	auto length = ::sendto ((socket_type)_value, (const char*)buff, (int)size, (int)flags, (sockaddr*)&saddrin, sizeof (saddrin));
	if (length < 0)
		throw std::runtime_error (__FUNCTION__ "::sendto");
	return length;
}
