#include "address.hpp"
#if defined(_WIN32)
	#include <winsock.h>
#endif

using namespace automaton;
using namespace ipv4;

static inline std::uint32_t from_string (const std::string& ip)
{
	return inet_addr (ip.data ());
}

address::address (const std::string& ip)
:	_value (from_string (ip))
{}

address::address (std::uint32_t ip)
:	_value (htonl (ip))
{}

address::address ()
:	address (anyaddres())
{}

address::address (std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d)
:	address (a*0x1000000ul + b*0x10000ul + c*0x100ul + d*0x1ul)
{}

address address::broadcast ()
{
	return address{ 255, 255, 255, 255 };
}

address address::anyaddres ()
{
	return address{ 0, 0, 0, 0 };
}

address address::localhost ()
{
	return address{ 127, 0, 0, 1 };
}

std::string automaton::ipv4::address::to_string () const
{	
	in_addr iaddr;
	iaddr.s_addr = _value;
	return inet_ntoa (iaddr);
}

std::uint32_t automaton::ipv4::address::value (bool nbo) const
{
	return nbo ? _value : ntohl (_value);
}

full_address::full_address (std::uint16_t port, const address & addr)
:	_addr (addr),
	_port (htons (port))
{}

address full_address::addr () const
{
	return _addr;
}

std::uint16_t full_address::port (bool nbo) const
{
	return nbo ? _port : ntohs (_port);
}
