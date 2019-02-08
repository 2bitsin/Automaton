#pragma once

#include <variant>
#include <string>

namespace automaton
{
	namespace ipv4
	{
		struct address
		{
			address (const std::string& ip);
			address ();
			address (std::uint32_t ip);
			address (std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d);

			static address broadcast ();
			static address anyaddres ();
			static address localhost ();

			std::string to_string () const;
			std::uint32_t value (bool nbo) const;
		private:
			std::uint32_t _value; // Ipv4 address
		};
	}

	using address = ipv4::address;

	struct full_address
	{
		full_address (std::uint16_t port = 0, const address& addr = address::broadcast ());
		address addr () const;
		std::uint16_t port (bool nbo = false) const;
	
		std::string to_string () const
		{
			using namespace std::string_literals;
			return addr ().to_string () + ":"s + std::to_string (port ());
		}
	protected:
		address _addr;
		std::uint16_t _port;
	};

}