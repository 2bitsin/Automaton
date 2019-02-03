#ifdef _WIN32

#pragma comment(lib, "ws2_32")
#include <winsock.h>
#include <cstdlib>

static WSADATA _Wsa_data;
static const auto _Wsa_avalable = ([]() {
	std::atexit ([]() { WSACleanup (); });
	return WSAStartup (MAKEWORD (2, 2), &_Wsa_data) == 0;
})();
#endif